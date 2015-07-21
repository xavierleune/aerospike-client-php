#include "php.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_error.h"
#include "aerospike/as_record.h"
#include "string.h"
#include "aerospike_common.h"
#include "aerospike_policy.h"

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
 * @param offset                The offset to be incremented by in case of operation: increment,
 *                              or to be initialized if record/bin does not already exist.
 * @param time_to_live          The ttl for the record in case of operation: touch.
 * @param operation             The operation type.
 *
 *******************************************************************************************************
 */
static as_status
aerospike_record_operations_ops(aerospike* as_object_p,
                                as_key* as_key_p,
                                zval* options_p,
                                as_error* error_p,
                                char* bin_name_p,
                                char* str,
                                u_int64_t offset,
                                u_int32_t time_to_live,
                                u_int64_t operation,
                                as_operations* ops,
                                as_record** get_rec TSRMLS_DC)
{
    as_val*             value_p = NULL;
    const char          *select[] = {bin_name_p, NULL};


    switch (operation) {
        case AS_OPERATOR_APPEND:
            if (!as_operations_add_append_str(ops, bin_name_p, str)) {
                PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to append");
                DEBUG_PHP_EXT_DEBUG("Unable to append");
                goto exit;
            }
            break;
        case AS_OPERATOR_PREPEND:
            if (!as_operations_add_prepend_str(ops, bin_name_p, str)) {
                PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to prepend");
                DEBUG_PHP_EXT_DEBUG("Unable to prepend");
                goto exit;
            }
            break;
        case AS_OPERATOR_INCR:
            if (!as_operations_add_incr(ops, bin_name_p, offset)) {
                PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to increment");
                DEBUG_PHP_EXT_DEBUG("Unable to increment");
                goto exit;
            }
            break;
        case AS_OPERATOR_TOUCH:
            ops->ttl = time_to_live;
            if (!as_operations_add_touch(ops)) {
                PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to touch");
                DEBUG_PHP_EXT_DEBUG("Unable to touch");
                goto exit;
            }
            break;
        case AS_OPERATOR_READ:
            if (!as_operations_add_read(ops, bin_name_p)) {
                PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to read");
                DEBUG_PHP_EXT_DEBUG("Unable to read");
                goto exit;
            }
            break;

        case AS_OPERATOR_WRITE:
            if (str) {
                if (!as_operations_add_write_str(ops, bin_name_p, str)) {
                    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to write");
                    DEBUG_PHP_EXT_DEBUG("Unable to write");
                    goto exit;
                }
            } else {
                if (!as_operations_add_write_int64(ops, bin_name_p, offset)) {
                    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to write");
                    DEBUG_PHP_EXT_DEBUG("Unable to write");
                    goto exit;
                }
            }
            break;

        default:
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Invalid operation");
            DEBUG_PHP_EXT_DEBUG("Invalid operation");
            goto exit;
    }

exit:
     return error_p->code;
}

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
    as_policy_read              read_policy;
    as_record*                  record_p = NULL;

    if ((!as_key_p) || (!error_p) || (!as_object_p) || (!metadata_p)) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Cannot perform exists");
        DEBUG_PHP_EXT_DEBUG("Cannot perform exists");
        goto exit;
    }

    set_policy(&as_object_p->config, &read_policy, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, options_p, error_p TSRMLS_CC);
    if (AEROSPIKE_OK != error_p->code) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    if (AEROSPIKE_OK != aerospike_key_exists(as_object_p, error_p,
                    &read_policy, as_key_p, &record_p)) {
        goto exit;
    }

    add_assoc_long(metadata_p, "generation", record_p->gen);
    add_assoc_long(metadata_p, "ttl", record_p->ttl);

exit:
    if (record_p) {
        as_record_destroy(record_p);
    }

    return error_p->code;
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
    as_policy_remove            remove_policy;
    aerospike*                  as_object_p = aerospike_obj_p->as_ref_p->as_p;
    TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);

    if ( (!as_key_p) || (!error_p) || (!as_object_p)) {
        DEBUG_PHP_EXT_DEBUG("Unable to remove key");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to remove key");
        goto exit;
    }

    set_policy(&as_object_p->config, NULL, NULL, NULL, &remove_policy,
            NULL, NULL, NULL, NULL, options_p, error_p TSRMLS_CC);
    if (AEROSPIKE_OK != error_p->code) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    get_generation_value(options_p, &remove_policy.generation, error_p TSRMLS_CC);
    aerospike_key_remove(as_object_p, error_p, &remove_policy, as_key_p);

exit: 
    return error_p->code;
}

static as_status
aerospike_record_initialization(aerospike* as_object_p,
                                as_key* as_key_p,
                                zval* options_p,
                                as_error* error_p,
                                as_policy_operate* operate_policy,
                                int8_t* serializer_policy TSRMLS_DC)
{
    as_policy_operate_init(operate_policy);

    if ((!as_object_p) || (!error_p) || (!as_key_p)) {
        DEBUG_PHP_EXT_DEBUG("Unable to perform operate");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to perform operate");
        goto exit;
    }

    set_policy(&as_object_p->config, NULL, NULL, operate_policy, NULL, NULL,
            NULL, NULL, serializer_policy, options_p, error_p TSRMLS_CC);
    if (AEROSPIKE_OK != error_p->code) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }
exit:
     return error_p->code;
}

extern as_status
aerospike_record_operations_general(Aerospike_object* aerospike_obj_p,
                                as_key* as_key_p,
                                zval* options_p,
                                as_error* error_p,
                                char* bin_name_p,
                                char* str,
                                u_int64_t offset,
                                u_int64_t time_to_live,
                                u_int64_t operation)
{
    as_operations       ops;
    as_record*          get_rec = NULL;
    aerospike*          as_object_p = aerospike_obj_p->as_ref_p->as_p;
    as_policy_operate   operate_policy;
    /*
     * TODO: serializer_policy is not used right now.
     * Need to pass on serializer_policy to aerospike_record_operations_ops
     * in case of write operation, to write bytes to the database.
     */
    int8_t            serializer_policy;

    TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);
    as_operations_inita(&ops, 1);
    get_generation_value(options_p, &ops.gen, error_p TSRMLS_CC);
    if (AEROSPIKE_OK != aerospike_record_initialization(as_object_p, as_key_p,
                                                      options_p, error_p,
                                                      &operate_policy,
                                                      &serializer_policy TSRMLS_CC)) {
        DEBUG_PHP_EXT_ERROR("Initialization returned error");
        goto exit;
    }

    if (AEROSPIKE_OK != aerospike_record_operations_ops(as_object_p, as_key_p,
                                                      options_p, error_p,
                                                      bin_name_p, str,
                                                      offset, time_to_live, operation,
                                                      &ops, &get_rec TSRMLS_CC)) {
        DEBUG_PHP_EXT_ERROR("Prepend function returned an error");
        goto exit;
    }

    aerospike_key_operate(as_object_p, error_p,
                    &operate_policy, as_key_p, &ops, NULL);

exit: 
     if (get_rec) {
         as_record_destroy(get_rec);
     }
     as_operations_destroy(&ops);
     return error_p->code;
}

extern as_status
aerospike_record_operations_operate(Aerospike_object* aerospike_obj_p,
                                as_key* as_key_p,
                                zval* options_p,
                                as_error* error_p,
                                zval* returned_p,
                                HashTable* operations_array_p)
{
    as_operations               ops;
    as_record*                  get_rec = NULL;
    aerospike*                  as_object_p = aerospike_obj_p->as_ref_p->as_p;
    as_status                   status = AEROSPIKE_OK;
    as_policy_operate           operate_policy;
    HashPosition                pointer;
    HashPosition                each_pointer;
    HashTable*                  each_operation_array_p = NULL;
    char*                       bin_name_p;
    char*                       str;
    zval **                     operation;
    int                         offset = 0;
    long                        l_offset = 0;
    int                         op;
    zval**                      each_operation;
    zval**                      each_operation_back;
    int8_t                      serializer_policy;
    uint32_t                    ttl;
    foreach_callback_udata      foreach_record_callback_udata;

    TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);
    as_operations_inita(&ops, zend_hash_num_elements(operations_array_p));
    get_generation_value(options_p, &ops.gen, error_p TSRMLS_CC);

    if (AEROSPIKE_OK !=
            (status = aerospike_record_initialization(as_object_p, as_key_p,
                                                      options_p, error_p,
                                                      &operate_policy,
                                                      &serializer_policy TSRMLS_CC))) {
            DEBUG_PHP_EXT_ERROR("Initialization returned error");
            goto exit;
    }

    foreach_hashtable(operations_array_p, pointer, operation) {
        as_record *temp_rec = NULL;

        if (IS_ARRAY == Z_TYPE_PP(operation)) {
            each_operation_array_p = Z_ARRVAL_PP(operation);
            str = NULL;
            op = 0;
            ttl = 0;
            bin_name_p = NULL;
            foreach_hashtable(each_operation_array_p, each_pointer, each_operation) {
                uint options_key_len;
                ulong options_index;
                char* options_key;
                if (zend_hash_get_current_key_ex(each_operation_array_p, (char **) &options_key, 
                        &options_key_len, &options_index, 0, &each_pointer)
                                != HASH_KEY_IS_STRING) {
                    DEBUG_PHP_EXT_DEBUG("Unable to set policy: Invalid Policy Constant Key");
                    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
                        "Unable to set policy: Invalid Policy Constant Key");
                    goto exit;
                } else {
                    if (!strcmp(options_key, "op") && (IS_LONG == Z_TYPE_PP(each_operation))) {
                        op = (uint32_t) Z_LVAL_PP(each_operation);
                    } else if (!strcmp(options_key, "bin") && (IS_STRING == Z_TYPE_PP(each_operation))) {
                        bin_name_p = (char *) Z_STRVAL_PP(each_operation);
                    } else if (!strcmp(options_key, "val")) {
                        if (IS_STRING == Z_TYPE_PP(each_operation)) {
                            str = (char *) Z_STRVAL_PP(each_operation);
                            each_operation_back = each_operation;
                        } else if (IS_LONG == Z_TYPE_PP(each_operation)) {
                            offset = (uint32_t) Z_LVAL_PP(each_operation);
                        } else {
                            status = AEROSPIKE_ERR_CLIENT;
                            goto exit;
                        }
                    } else if (!strcmp(options_key, "ttl") && (IS_LONG == Z_TYPE_PP(each_operation))) {
                        ttl = (uint32_t) Z_LVAL_PP(each_operation);
                    } else {
                        status = AEROSPIKE_ERR_CLIENT;
                        DEBUG_PHP_EXT_DEBUG("Unable to set Operate: Invalid Optiopns Key");
                        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to set Operate: Invalid Options Key");
                        goto exit;
                    }
                }
            }
            if (op == AS_OPERATOR_INCR) {
                if (str) {
                    l_offset = (long) offset;
                    if  (!(is_numeric_string(Z_STRVAL_PP(each_operation_back), Z_STRLEN_PP(each_operation_back), &l_offset, NULL, 0))) {
                        status = AEROSPIKE_ERR_PARAM;
                        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "invalid value for increment operation");
                        DEBUG_PHP_EXT_DEBUG("Invalid value for increment operation");
                        goto exit;
                    }
                }
            }
            if (AEROSPIKE_OK != (status = aerospike_record_operations_ops(as_object_p,
                            as_key_p, options_p, error_p, bin_name_p, str,
                            offset, ttl, op, &ops, &temp_rec TSRMLS_CC))) {
                DEBUG_PHP_EXT_ERROR("Operate function returned an error");
                goto exit;
            }
            if (temp_rec) {
                as_record_destroy(temp_rec);
            }
        } else {
            status = AEROSPIKE_ERR_CLIENT;
            goto exit;
        }
    }

    if (AEROSPIKE_OK != (status = aerospike_key_operate(as_object_p, error_p,
                    &operate_policy, as_key_p, &ops, &get_rec))) {
        DEBUG_PHP_EXT_DEBUG("%s", error_p->message);
        goto exit;
    } else {
        if (get_rec) {
            foreach_record_callback_udata.udata_p = returned_p;
            foreach_record_callback_udata.error_p = error_p;
            foreach_record_callback_udata.obj = aerospike_obj_p;
            if (!as_record_foreach(get_rec, (as_rec_foreach_callback) AS_DEFAULT_GET,
                        &foreach_record_callback_udata)) {
                PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
                           "Unable to get bins of a record");
                DEBUG_PHP_EXT_DEBUG("Unable to get bins of a record");
            }
        }
     }

exit: 
     if (get_rec) {
         foreach_record_callback_udata.udata_p = NULL;
         as_record_destroy(get_rec);
     }
     as_operations_destroy(&ops);
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
        status = AEROSPIKE_ERR_CLIENT;
        goto exit;
    }

    set_policy(&as_object_p->config, NULL, &write_policy, NULL, NULL, NULL,
            NULL, NULL, NULL, options_p, error_p TSRMLS_CC);
    if (AEROSPIKE_OK != (status = (error_p->code))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }


    foreach_hashtable(bins_array_p, pointer, bin_names) {
        if (IS_STRING == Z_TYPE_PP(bin_names)) {
            if (!(as_record_set_nil(&rec, Z_STRVAL_PP(bin_names)))) {
                status = AEROSPIKE_ERR_CLIENT;
                goto exit;
            }
        } else {
             status = AEROSPIKE_ERR_CLIENT;
             goto exit;
        }
    }

    get_generation_value(options_p, &rec.gen, error_p TSRMLS_CC);
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
        DEBUG_PHP_EXT_DEBUG("Unable to perform exists");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to perform exists");
        status = AEROSPIKE_ERR_CLIENT;
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p) ||
             ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
                "input parameters (type) for exist/getMetdata function not proper.");
        DEBUG_PHP_EXT_ERROR("input parameters (type) for exist/getMetdata function not proper.");
        status = AEROSPIKE_ERR_PARAM;
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
        PHP_EXT_SET_AS_ERR(error_p, status,
                "unable to iterate through exists/getMetadata key params");
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


