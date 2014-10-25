#include "php.h"
#include "aerospike/as_log.h"
#include "aerospike/as_key.h"
#include "aerospike/as_config.h"
#include "aerospike/as_error.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike.h"
#include "aerospike_common.h"
#include "aerospike/as_batch.h"
#include "aerospike_policy.h"

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
batch_read_cb(const as_batch_read* results, uint32_t n, void* udata)
{
    TSRMLS_FETCH();
    as_status                   status = AEROSPIKE_OK;
    foreach_callback_udata*     udata_ptr = (foreach_callback_udata*)udata;
    uint32_t                    i = 0;
    zval*                       record_metadata_p = NULL;

    if (n == 0) {
        goto exit;
    }

    for (i = 0; i < n; i++) {
        if (results[i].result == AEROSPIKE_OK) {
            MAKE_STD_ZVAL(record_metadata_p);
            array_init(record_metadata_p);
            if (0 != add_assoc_long(record_metadata_p, PHP_AS_RECORD_DEFINE_FOR_GENERATION,
                        results[i].record.gen)) {
                DEBUG_PHP_EXT_DEBUG("Unable to get generation of a record");
                status = AEROSPIKE_ERR;
                goto exit;
            }

            if (0 != add_assoc_long(record_metadata_p, PHP_AS_RECORD_DEFINE_FOR_TTL,
                    results[i].record.ttl)) {
                DEBUG_PHP_EXT_DEBUG("Unable to get ttl of a record");
                status = AEROSPIKE_ERR;
                goto exit;
            }

            switch (((as_val*)(results[i].key->valuep))->type) {
                case AS_STRING:
                    if (0 != add_assoc_zval(udata_ptr->udata_p, results[i].key->value.string.value, record_metadata_p)) {
                        DEBUG_PHP_EXT_DEBUG("Unable to get key of a record");
                        status = AEROSPIKE_ERR;
                        goto exit;
                    }
                    break;
                case AS_INTEGER:
                    if (FAILURE == add_index_zval(udata_ptr->udata_p, results[i].key->value.integer.value,
                            record_metadata_p)) {
                        DEBUG_PHP_EXT_DEBUG("Unable to get key of a record");
                        status = AEROSPIKE_ERR;
                        goto exit;
                    }
                    break;
                default:
                    break;
            }
        }  else if (results[i].result == AEROSPIKE_ERR_RECORD_NOT_FOUND) {
            switch (((as_val*)(results[i].key->valuep))->type) {
                case AS_STRING:
                    if (0 != add_assoc_null(udata_ptr->udata_p, results[i].key->value.string.value)) {
                        DEBUG_PHP_EXT_DEBUG("Unable to get key of a record");
                        status = AEROSPIKE_ERR;
                        goto exit;
                    }
                    break;
                case AS_INTEGER:
                    if (0 != add_index_null(udata_ptr->udata_p, results[i].key->value.integer.value)) {
                        printf("add_index_null\n");
                        DEBUG_PHP_EXT_DEBUG("Unable to get key of a record");
                        status = AEROSPIKE_ERR;
                        goto exit;
                    }
                    break;
                default:
                    break;
            }
        } else {
            return false;
        }
    }

exit:
    if (n == 0)
        return true;
    if (status != AEROSPIKE_OK && record_metadata_p) {
        zval_ptr_dtor(&record_metadata_p);
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
aerospike_existsMany(aerospike* as_object_p, as_error* error_p,
        zval* keys_p, zval* metadata_p, zval* options_p TSRMLS_DC)
{
    as_status                   status = AEROSPIKE_OK;
    as_policy_read              read_policy;
    as_policy_batch             batch_policy;
    as_batch                    batch;
    HashTable*                  keys_array = Z_ARRVAL_P(keys_p);
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

    set_policy_batch(&batch_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    as_batch_inita(&batch, zend_hash_num_elements(keys_array));
    is_batch_init = true;

    foreach_hashtable(keys_array, key_pointer, key_entry) {
        aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_PP(key_entry), as_batch_keyat(&batch, i), &initializeKey);
        i++;
    }

    metadata_callback.udata_p = metadata_p;
    metadata_callback.error_p = error_p;

    if (AEROSPIKE_OK != (status = aerospike_batch_exists(as_object_p, error_p,
                    batch_policy, &batch, batch_read_cb, &metadata_callback))) {
        DEBUG_PHP_EXT_DEBUG("Unable to get metadata of batch records");
        goto exit;
    }

exit:
    if (is_batch_init) {
        as_batch_destroy(&batch);
    }
    return status;
}
