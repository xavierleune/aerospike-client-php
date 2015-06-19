#include "php.h"
#include "aerospike/as_error.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike.h"
#include "aerospike_common.h"
#include "aerospike/as_batch.h"
#include "aerospike/aerospike_batch.h"
#include "aerospike_policy.h"

static void
populate_result_for_get_exists_many(as_key *key_p, zval *outer_container_p,
        zval *inner_container_p, as_error *error_p, bool null_flag TSRMLS_DC)
{
    if (!(as_val*)(key_p->valuep)) {
        if (!null_flag) {
            if (0 != add_assoc_zval(outer_container_p, (char *) key_p->digest.value, inner_container_p)) {
                PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_SERVER,
                        "Unable to get key of a record");
                DEBUG_PHP_EXT_DEBUG("Unable to get key of a record");
            }
        } else {
            if (0 != add_assoc_null(outer_container_p, (char *) key_p->digest.value)) {
                PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_SERVER,
                        "Unable to get key of a record");
                DEBUG_PHP_EXT_DEBUG("Unable to get key of a record");
            }
        }
    } else {
        switch (((as_val*)(key_p->valuep))->type) {
            case AS_STRING:
                if (!null_flag) {
                    if (0 != add_assoc_zval(outer_container_p, key_p->value.string.value, inner_container_p)) {
                        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_SERVER,
                                "Unable to get key of a record");
                        DEBUG_PHP_EXT_DEBUG("Unable to get key of a record");
                    }
                } else {
                    if (0 != add_assoc_null(outer_container_p, key_p->value.string.value)) {
                        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_SERVER,
                                "Unable to get key of a record");
                        DEBUG_PHP_EXT_DEBUG("Unable to get key of a record");
                    }
                }
                break;
            case AS_INTEGER:
                if (!null_flag) {
                    if (FAILURE == add_index_zval(outer_container_p, key_p->value.integer.value,
                                inner_container_p)) {
                        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_SERVER,
                                "Unable to get key of a record");
                        DEBUG_PHP_EXT_DEBUG("Unable to get key of a record");
                    }
                } else {
                    if (0 != add_index_null(outer_container_p, key_p->value.integer.value)) {
                        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_SERVER,
                                "Unable to get key of a record");
                        DEBUG_PHP_EXT_DEBUG("Unable to get key of a record");
                    }
                }
                break;
            default:
                break;
        }
    }
}

/*
 ******************************************************************************************************
 * This callback will be called with the results of aerospike_batch_get() and aerospike_batch_exists().
 * 
 * @param results                   An array of n as_batch_read entries.
 * @param n                         The number of results from the batch request.
 * @param udata                     The zval return value to be filled with the
 *                                  result of existsMany().
 *
 ******************************************************************************************************
 */

bool
batch_exists_cb(const as_batch_read* results, uint32_t n, void* udata)
{
    TSRMLS_FETCH();
    foreach_callback_udata*     udata_ptr = (foreach_callback_udata*) udata;
    uint32_t                    i = 0;
    bool                        null_flag = false;

    for (i = 0; i < n; i++) {
        zval *record_metadata_p = NULL;
        if (results[i].result == AEROSPIKE_OK) {
            MAKE_STD_ZVAL(record_metadata_p);
            array_init(record_metadata_p);
            if (0 != add_assoc_long(record_metadata_p, PHP_AS_RECORD_DEFINE_FOR_GENERATION,
                        results[i].record.gen)) {
                DEBUG_PHP_EXT_DEBUG("Unable to get generation of a record");
                PHP_EXT_SET_AS_ERR(udata_ptr->error_p, AEROSPIKE_ERR_SERVER,
                        "Unable to get generation of a record");
                goto cleanup;
            }

            if (0 != add_assoc_long(record_metadata_p, PHP_AS_RECORD_DEFINE_FOR_TTL,
                    results[i].record.ttl)) {
                DEBUG_PHP_EXT_DEBUG("Unable to get ttl of a record");
                PHP_EXT_SET_AS_ERR(udata_ptr->error_p, AEROSPIKE_ERR_SERVER,
                        "Unable to get ttl of a record");
                goto cleanup;
            }
            null_flag = false;
        } else if (results[i].result == AEROSPIKE_ERR_RECORD_NOT_FOUND) {
            null_flag = true;
        } else {
            return false;
        }

        populate_result_for_get_exists_many((as_key *) results[i].key,
                udata_ptr->udata_p, record_metadata_p, udata_ptr->error_p,
                null_flag TSRMLS_CC);

        if (AEROSPIKE_OK != udata_ptr->error_p->code) {
            DEBUG_PHP_EXT_DEBUG("%s", udata_ptr->error_p->message);
            goto cleanup;
        } else {
            continue;
        }
cleanup:
        if (record_metadata_p && (AEROSPIKE_OK != udata_ptr->error_p->code)) {
            zval_ptr_dtor(&record_metadata_p);
        }
    }

exit:
    if (udata_ptr->error_p->code != AEROSPIKE_OK) {
        return false;
    }
    return true;
}

/*
 ******************************************************************************************************
 * Aerospike::existsMany - check if a batch of records exist in the Aerospike database.
 *
 * @param as_object_p               The C client's aerospike object.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param keys_p                    An array of initialized keys, each an array
 *                                  with keys ['ns','set','key'] or ['ns','set','digest'].
 * @param metadata_p                Metadata of records.
 * @param options_p                 Optional parameters.
 *
 ******************************************************************************************************
 */
extern as_status
aerospike_batch_operations_exists_many(aerospike* as_object_p, as_error* error_p,
        zval* keys_p, zval* metadata_p, zval* options_p TSRMLS_DC)
{
    as_status                   status = AEROSPIKE_OK;
    as_policy_read              read_policy;
    as_policy_batch             batch_policy;
    as_batch                    batch;
    HashTable*                  keys_array = NULL;
    HashPosition                key_pointer;
    zval**                      key_entry;
    int16_t                     initializeKey = 0;
    int                         i = 0;
    bool                        is_batch_init = false;
    foreach_callback_udata      metadata_callback;

    if (!(as_object_p) || !(keys_p) || !(metadata_p)) {
        status = AEROSPIKE_ERR_PARAM;
        goto exit;
    }

    set_policy_batch(&as_object_p->config, &batch_policy, options_p,
            error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    /*
     * No need to set error
     * over here..
     */
    keys_array = Z_ARRVAL_P(keys_p);
    if (zend_hash_num_elements(keys_array) == 0) {
        goto exit;
    }

    as_batch_inita(&batch, zend_hash_num_elements(keys_array));
    is_batch_init = true;

    foreach_hashtable(keys_array, key_pointer, key_entry) {
        aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_PP(key_entry),
                as_batch_keyat(&batch, i), &initializeKey);
        i++;
    }

    metadata_callback.udata_p = metadata_p;
    metadata_callback.error_p = error_p;

    if (AEROSPIKE_OK != (status = aerospike_batch_exists(as_object_p, error_p,
                    &batch_policy, &batch, batch_exists_cb, &metadata_callback))) {
        DEBUG_PHP_EXT_DEBUG("Unable to get metadata of batch records");
        goto exit;
    }

exit:
    if (is_batch_init) {
        as_batch_destroy(&batch);
    }
    return status;
}

static as_status
process_filer_bins(HashTable *bins_array_p, const char **select_p TSRMLS_DC)
{
    as_status           status = AEROSPIKE_OK;
    HashPosition        pointer; 
    zval                **bin_names;
    int                 count = 0;

    foreach_hashtable (bins_array_p, pointer, bin_names) {
        switch (Z_TYPE_PP(bin_names)) {
            case IS_STRING:
                select_p[count++] = Z_STRVAL_PP(bin_names);
                break;
            default:
                status = AEROSPIKE_ERR_PARAM;
                DEBUG_PHP_EXT_DEBUG("Invalid type of bin");
                goto exit;
        }
    }
exit:
    return status;
}

/*
 ******************************************************************************************************
 * This callback will be called with the results of aerospike_batch_get().
 * 
 * @param results                   An array of n as_batch_read entries.
 * @param n                         The number of results from the batch request.
 * @param udata                     The zval return value to be filled with the
 *                                  result of getMany().
 *
 ******************************************************************************************************
 */
bool
batch_get_cb(const as_batch_read* results, uint32_t n, void* udata)
{
    TSRMLS_FETCH();
    foreach_callback_udata*       udata_ptr = (foreach_callback_udata *) udata;
    uint32_t                      i = 0;
    foreach_callback_udata        foreach_record_callback_udata;
    bool                          null_flag = false;

    for (i = 0; i < n; i++) {
        zval *record_p = NULL;
        zval *get_record_p = NULL;

        if (results[i].result == AEROSPIKE_OK) {
            MAKE_STD_ZVAL(record_p);
            array_init(record_p);
            ALLOC_INIT_ZVAL(get_record_p);
            array_init(get_record_p);
    
            foreach_record_callback_udata.udata_p = get_record_p;
            foreach_record_callback_udata.error_p = udata_ptr->error_p;
            foreach_record_callback_udata.obj = udata_ptr->obj;
            null_flag = false;
        } else if (results[i].result == AEROSPIKE_ERR_RECORD_NOT_FOUND) {
            null_flag = true;
        } else {
            return false;
        }

        populate_result_for_get_exists_many((as_key *) results[i].key,
                udata_ptr->udata_p, record_p, udata_ptr->error_p, null_flag TSRMLS_CC);
        if (AEROSPIKE_OK != udata_ptr->error_p->code) {
            DEBUG_PHP_EXT_DEBUG("%s", udata_ptr->error_p->message);
            goto cleanup;
        }

        if (null_flag) {
            goto cleanup;
        }

        if (AEROSPIKE_OK != aerospike_get_key_meta_bins_of_record(NULL, (as_record *) &results[i].record,
                    (as_key *) results[i].key, record_p, NULL, false TSRMLS_CC)) {
            PHP_EXT_SET_AS_ERR(udata_ptr->error_p, AEROSPIKE_ERR_SERVER,
                    "Unable to get metadata of a record");
            DEBUG_PHP_EXT_DEBUG("Unable to get metadata of a record");
            goto cleanup;
        }

        if (!as_record_foreach(&results[i].record, (as_rec_foreach_callback) AS_DEFAULT_GET,
            &foreach_record_callback_udata)) {
            PHP_EXT_SET_AS_ERR(udata_ptr->error_p, AEROSPIKE_ERR_SERVER,
                    "Unable to get bins of a record");
            DEBUG_PHP_EXT_DEBUG("Unable to get bins of a record");
            goto cleanup;
        }

        if (0 != add_assoc_zval(record_p, PHP_AS_RECORD_DEFINE_FOR_BINS, get_record_p)) {
            PHP_EXT_SET_AS_ERR(udata_ptr->error_p, AEROSPIKE_ERR_RECORD_NOT_FOUND,
                    "Unable to get a record");
            DEBUG_PHP_EXT_DEBUG("Unable to get a record");
            goto cleanup;
        }

        if (udata_ptr->error_p->code == AEROSPIKE_OK) {
            continue;
        }

cleanup:
        foreach_record_callback_udata.udata_p = NULL;
        if (get_record_p) {
            zval_ptr_dtor(&get_record_p);
        }
        if (record_p) {
            zval_ptr_dtor(&record_p);
        }
    }

exit:
    if (udata_ptr->error_p->code != AEROSPIKE_OK) {
        return false;
    }
    return true;
}
/*
 ******************************************************************************************************
 * Get all records identified by the array of keys.
 *
 * @param as_object_p               The C client's aerospike object.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param keys_p                    An array of initialized keys, each an array
 *                                  with keys ['ns','set','key'] or ['ns','set','digest'].
 * @param records_p                 The array of records to be populated with
 *                                  the result by this method.
 * @param filter_bins_p             The optional filter bins applicable to all
 *                                  records.
 * @param options_p                 Optional parameters.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 * ******************************************************************************************************
 */
extern as_status
aerospike_batch_operations_get_many(aerospike* as_object_p, as_error* error_p,
        zval* keys_p, zval* records_p, zval* filter_bins_p, zval* options_p TSRMLS_DC)
{
    as_policy_batch                     batch_policy;
    as_batch                            batch;
    HashTable*                          keys_ht_p = NULL;
    HashPosition                        key_pointer;
    zval**                              key_entry;
    int16_t                             initializeKey = 0;
    int                                 i = 0;
    bool                                is_batch_init = false;
    foreach_callback_udata              batch_get_callback_udata;
    int                                 filter_bins_count = 0;

    if (!(as_object_p) || !(keys_p) || !(records_p)) {
        DEBUG_PHP_EXT_DEBUG("Unable to initiate batch get");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Unable to initiate batch get");
        goto exit;
    }

    set_policy_batch(&as_object_p->config, &batch_policy, options_p, error_p TSRMLS_CC);
    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    if (Z_TYPE_P(keys_p) == IS_ARRAY) {
        keys_ht_p = Z_ARRVAL_P(keys_p);
    } else {
        DEBUG_PHP_EXT_DEBUG("Invalid type for keys");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Invalid type for keys");
    }

    if (zend_hash_num_elements(keys_ht_p) == 0) {
        /*
         * No need to set error here. This condition can be eliminated after C
         * SDK adds check for size.
         */
        goto exit;
    }

    as_batch_inita(&batch, zend_hash_num_elements(keys_ht_p));
    is_batch_init = true;

    foreach_hashtable(keys_ht_p, key_pointer, key_entry) {
        aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_PP(key_entry),
                as_batch_keyat(&batch, i), &initializeKey);
        i++;
    }

    batch_get_callback_udata.udata_p = records_p;
    batch_get_callback_udata.error_p = error_p;

    if (filter_bins_p) {
        filter_bins_count = zend_hash_num_elements(Z_ARRVAL_P(filter_bins_p));
        const char*                       select_p[filter_bins_count];
        process_filer_bins(Z_ARRVAL_P(filter_bins_p), select_p TSRMLS_CC);
        if (AEROSPIKE_OK != aerospike_batch_get_bins(as_object_p, error_p, &batch_policy,
                    &batch, select_p, filter_bins_count,
                    (aerospike_batch_read_callback) batch_get_cb,
                    &batch_get_callback_udata)) {
            DEBUG_PHP_EXT_DEBUG("Unable to get batch records");
            goto exit;
        }
    } else if (AEROSPIKE_OK != aerospike_batch_get(as_object_p, error_p, &batch_policy,
                &batch, (aerospike_batch_read_callback) batch_get_cb,
                &batch_get_callback_udata)) {
        DEBUG_PHP_EXT_DEBUG("Unable to get batch records");
        goto exit;
    }

exit:
    if (is_batch_init) {
        as_batch_destroy(&batch);
    }
    return error_p->code;
}
