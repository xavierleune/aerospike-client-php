#include "php.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_error.h"
#include "aerospike/as_record.h"

#include "aerospike_common.h"
#include "aerospike_policy.h"

/*
 *******************************************************************************************************
 * Wrapper function to perform an aerospike_key_exists within the C client.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param as_key_p              The C client's as_key that identifies the record.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param metadata_p            The return metadata for the exists/getMetadata API to be 
 *                              populated by this function.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status aerospike_record_operations_exists(aerospike* as_object_p,
                                                    as_key* as_key_p,
                                                    as_error *error_p,
                                                    zval* metadata_p,
                                                    zval* options_p TSRMLS_DC)
{
    as_status                   status = AEROSPIKE_OK;
    as_policy_read              read_policy;
    as_record*                  record_p = NULL;
    /*Aerospike_object *aerospike_object = PHP_AEROSPIKE_GET_OBJECT;
    TSRMLS_FETCH_FROM_CTX(aerospike_object->ts);*/

    if ( (!as_key_p) || (!error_p) ||
         (!as_object_p) || (!metadata_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    set_policy(&read_policy, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            options_p, error_p TSRMLS_CC);
    if (AEROSPIKE_OK != (status = (error_p->code))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_key_exists(as_object_p, error_p,
                    &read_policy, as_key_p, &record_p))) {
        goto exit;
    }

    add_assoc_long(metadata_p, "generation", record_p->gen);
    add_assoc_long(metadata_p, "ttl", record_p->ttl);

exit:
    if (record_p) {
        as_record_destroy(record_p);
    }

    return(status);   
}


/*
 *******************************************************************************************************
 * Wrapper function to perform an aerospike_key_remove within the C client.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param as_key_p              The C client's as_key that identifies the record.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_record_operations_remove(Aerospike_object* aerospike_obj_p,
                                   as_key* as_key_p,
                                   as_error *error_p,
                                   zval* options_p)
{
    as_status                   status = AEROSPIKE_OK;
    as_policy_remove            remove_policy;
    aerospike*                  as_object_p = aerospike_obj_p->as_ref_p->as_p;
    TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);

    if ( (!as_key_p) || (!error_p) ||
         (!as_object_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    set_policy(NULL, NULL, NULL, &remove_policy, NULL, NULL, NULL, NULL,
            options_p, error_p TSRMLS_CC);
    if (AEROSPIKE_OK != (status = (error_p->code))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_key_remove(as_object_p, error_p,
                    &remove_policy, as_key_p))) {
        goto exit;
    }
exit: 
    return(status);
}

/*
 *******************************************************************************************************
 * Wrapper function to perform an aerospike_key_oeprate within the C client.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param as_key_p              The C client's as_key that identifies the record.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param bin_name_p            The bin name to perform operation upon.
 * @param str                   The string to be appended in case of operation: append.
 * @param offset                The offset to be incremented by in case of operation: increment.
 * @param initial_value         The initial value to be set if record is absent
 *                              in case of operation: increment.
 * @param time_to_live          The ttl for the record in case of operation: touch.
 * @param operation             The operation type.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_record_operations_ops(Aerospike_object* aerospike_obj_p,
                                as_key* as_key_p,
                                zval* options_p,
                                as_error* error_p,
                                char* bin_name_p,
                                char* str,
                                u_int64_t offset,
                                u_int64_t initial_value,
                                u_int64_t time_to_live,
                                u_int64_t operation)
{
    as_status           status = AEROSPIKE_OK;
    as_policy_operate   operate_policy;
    uint32_t            serializer_policy;
    as_record*          get_rec = NULL;
    as_operations       ops;
    as_val*             value_p = NULL;
    as_integer          initial_int_val;
    int16_t             initialize_int = 0;
    const char *select[] = {bin_name_p, NULL};
    aerospike*          as_object_p = aerospike_obj_p->as_ref_p->as_p;
    TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);

    as_operations_inita(&ops, 1);
    as_policy_operate_init(&operate_policy);

    if ((!as_object_p) || (!error_p) || (!as_key_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    set_policy(NULL, NULL, &operate_policy, NULL, NULL, NULL, NULL,
            &serializer_policy, options_p, error_p TSRMLS_CC);
    if (AEROSPIKE_OK != (status = (error_p->code))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    switch(operation) {
        case AS_OPERATOR_APPEND:
            as_operations_add_append_str(&ops, bin_name_p, str);
            break;
        case AS_OPERATOR_PREPEND:
            as_operations_add_prepend_str(&ops, bin_name_p, str);
            break;
        case AS_OPERATOR_INCR:
            if (AEROSPIKE_OK != (status = aerospike_key_select(as_object_p,
                            error_p, NULL, as_key_p, select, &get_rec))) {
                goto exit;
            } else {
                if (NULL != (value_p = (as_val *) as_record_get (get_rec, bin_name_p))) {
                   if (AS_NIL == value_p->type) {
                       as_integer_init(&initial_int_val, initial_value);
                       initialize_int = 1;
                       if (!as_operations_add_write(&ops, bin_name_p,
                                   (as_bin_value*) &initial_int_val)) {
                           status = AEROSPIKE_ERR;
                           goto exit;
                       }
                   } else {
                       as_operations_add_incr(&ops, bin_name_p, offset);
                   }
                } else {
                    status = AEROSPIKE_ERR;
                    goto exit;
                }
            }
            break;
        case AS_OPERATOR_TOUCH:
            ops.ttl = time_to_live;
            as_operations_add_touch(&ops);
            break;
        default:
            status = AEROSPIKE_ERR;
            goto exit;
            break;
    }

    if (AEROSPIKE_OK != (status = aerospike_key_operate(as_object_p, error_p,
                    &operate_policy, as_key_p, &ops, &get_rec))) {
        goto exit;
    }

exit:
     as_operations_destroy(&ops);
     if (get_rec) {
         as_record_destroy(get_rec);
     }
     if (initialize_int) { 
         as_integer_destroy(&initial_int_val);
     }
     return status;
}

/*
 *******************************************************************************************************
 * Wrapper function to remove bin(s) from a record.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param as_key_p              The C client's as_key that identifies the record.
 * @param bins_p                The PHP array of bins to be removed from the record.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_record_operations_remove_bin(Aerospike_object* aerospike_obj_p,
                                       as_key* as_key_p,
                                       zval* bins_p,
                                       as_error* error_p,
                                       zval* options_p)
{
    as_status           status = AEROSPIKE_OK;
    as_record           rec;
    HashTable           *bins_array_p = Z_ARRVAL_P(bins_p);
    HashPosition        pointer;
    zval                **bin_names;
    as_policy_write     write_policy;
    aerospike*          as_object_p = aerospike_obj_p->as_ref_p->as_p;
    TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);

    as_record_inita(&rec, zend_hash_num_elements(bins_array_p));

    if ((!as_object_p) || (!error_p) || (!as_key_p) || (!bins_array_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    set_policy(NULL, &write_policy, NULL, NULL, NULL, NULL,
            NULL, NULL, options_p, error_p TSRMLS_CC);
    if (AEROSPIKE_OK != (status = (error_p->code))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }


    foreach_hashtable(bins_array_p, pointer, bin_names) {
        if (IS_STRING == Z_TYPE_PP(bin_names)) {
            if (!(as_record_set_nil(&rec, Z_STRVAL_PP(bin_names)))) {
                status = AEROSPIKE_ERR;
                goto exit;
            }
        } else {
             status = AEROSPIKE_ERR;
             goto exit;
        }
    }

    if (AEROSPIKE_OK != (status = aerospike_key_put(as_object_p, error_p,
                    NULL, as_key_p, &rec))) {
         goto exit;
    }

exit:
    as_record_destroy(&rec);
    return(status);
}

/*
 *******************************************************************************************************
 * Wrapper function to perform an aerospike_key_exists within the C client.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param as_key_p              The C client's as_key that identifies the record.
 * @param metadata_p            The return metadata for the exists/getMetadata API to be 
 *                              populated by this function.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_php_exists_metadata(Aerospike_object* aerospike_obj_p,
                              zval* key_record_p,
                              zval* metadata_p,
                              zval* options_p,
                              as_error* error_p)
{
    as_status              status = AEROSPIKE_OK;
    as_key                 as_key_for_put_record;
    int16_t                initializeKey = 0;
    aerospike*             as_object_p = aerospike_obj_p->as_ref_p->as_p;

    TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);
    if ((!as_object_p) || (!key_record_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p) ||
             ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("input parameters (type) for exist/getMetdata function not proper.");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(metadata_p)) {
        zval*         metadata_arr_p = NULL;

        MAKE_STD_ZVAL(metadata_arr_p);
        array_init(metadata_arr_p);
        ZVAL_ZVAL(metadata_p, metadata_arr_p, 1, 1);
    }

    if (AEROSPIKE_OK != (status =
                aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                    &as_key_for_put_record, &initializeKey))) {
        DEBUG_PHP_EXT_ERROR("unable to iterate through exists/getMetadata key params");
        goto exit;
    }

    if (AEROSPIKE_OK != (status =
                aerospike_record_operations_exists(as_object_p, &as_key_for_put_record,
                    error_p, metadata_p, options_p TSRMLS_CC))) {
        DEBUG_PHP_EXT_ERROR("exists/getMetadata: unable to fetch the record");
        goto exit;
    }

exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_put_record);
    }

    return status;
}


