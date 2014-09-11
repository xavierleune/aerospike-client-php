#include "php.h"
#include "aerospike/as_log.h"
#include "aerospike/as_key.h"
#include "aerospike/as_config.h"
#include "aerospike/as_error.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike.h"
#include "aerospike_common.h"
#include "aerospike/as_udf.h"
#include "aerospike/as_query.h"
#include "aerospike_policy.h"

#define BIN "bin"
#define OP "op"
#define VAL "val"

/*
 ******************************************************************************************************
 Initializes and defines an as_query object.
 *
 * @param as_query_p                The C client's as_query object to be
 *                                  initialized.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param namespace_p               The namespace to scan.
 * @param set_p                     The set to scan.
 * @param predicate_ht_p            The HashTable for Query Predicate array.
 * @param module_p                  The name of UDF module containing the function
 *                                  to execute.
 * @param function_p                The name of the function to be applied to
 *                                  the record.
 * @param args_list_p               An as_arraylist initialized with arguments for the UDF.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
static as_status
aerospike_query_define(as_query* query_p, as_error* error_p, char* namespace_p,
        char* set_p, HashTable *predicate_ht_p, const char* module_p,
        const char* function_p, as_list* args_list_p)
{
    zval**              op_pp = NULL;
    zval**              bin_pp = NULL;
    zval**              val_pp = NULL;

    if ((!zend_hash_exists(predicate_ht_p, BIN, sizeof(BIN))) ||
        (!zend_hash_exists(predicate_ht_p, OP, sizeof(OP)))  ||
        (!zend_hash_exists(predicate_ht_p, VAL, sizeof(VAL)))) {
        DEBUG_PHP_EXT_DEBUG("Aerospike::query() expects parameter 3 to include the keys 'bin','op', and 'val'.");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
                "Aerospike::query() expects parameter 3 to include the keys 'bin','op', and 'val'.");
        goto exit;
    }

    if ((FAILURE == zend_hash_find(predicate_ht_p, OP, sizeof(OP),
                    (void **) &op_pp)) ||
            (FAILURE == zend_hash_find(predicate_ht_p, BIN, sizeof(BIN),
                             (void **) &bin_pp)) ||
            (FAILURE == zend_hash_find(predicate_ht_p, VAL, sizeof(VAL),
                             (void **) &val_pp))) {
        DEBUG_PHP_EXT_DEBUG("Aerospike::query() expects parameter 3 to include the keys 'bin','op', and 'val'.");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
                "Aerospike::query() expects parameter 3 to include the keys 'bin','op', and 'val'.");
        goto exit;
    }

    convert_to_string_ex(op_pp);
    convert_to_string_ex(bin_pp);
    if (strncmp(Z_STRVAL_PP(op_pp), "=", 1) == 0) {
        switch(Z_TYPE_PP(val_pp)) {
            case IS_STRING:
                convert_to_string_ex(val_pp);
                if (!as_query_where(query_p, Z_STRVAL_PP(bin_pp),
                        string_equals(Z_STRVAL_PP(val_pp)))) {
                    DEBUG_PHP_EXT_DEBUG("Unable to set query predicate");
                    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
                            "Unable to set query predicate");
                }
                break;
            case IS_LONG:
                convert_to_long_ex(val_pp);
                if (!as_query_where(query_p, Z_STRVAL_PP(bin_pp),
                        integer_equals(Z_LVAL_PP(val_pp)))) {
                    DEBUG_PHP_EXT_DEBUG("Unable to set query predicate");
                    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
                            "Unable to set query predicate");
                }
                break;
            default:
                DEBUG_PHP_EXT_DEBUG("Aerospike::query() predicate 'val' must be either string or integer.");
                PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
                        "Aerospike::query() predicate 'val' must be either string or integer.");
                goto exit;
        }
    } else if (strncmp(Z_STRVAL_PP(op_pp), "BETWEEN", 7) == 0) {
        bool between_unpacked = false;
        if (Z_TYPE_PP(val_pp) == IS_ARRAY) {
            convert_to_array_ex(val_pp);
            zval **min_pp;
            zval **max_pp;
            if ((zend_hash_index_find(Z_ARRVAL_PP(val_pp), 0, (void **) &min_pp) == SUCCESS) &&
                (zend_hash_index_find(Z_ARRVAL_PP(val_pp), 1, (void **) &max_pp) == SUCCESS)) {
                convert_to_long_ex(min_pp);
                convert_to_long_ex(max_pp);
                if (Z_TYPE_PP(min_pp) == IS_LONG && Z_TYPE_PP(max_pp) == IS_LONG) {
                    between_unpacked = true;
                    if (!as_query_where(query_p, Z_STRVAL_PP(bin_pp),
                                integer_range(Z_LVAL_PP(min_pp),
                                    Z_LVAL_PP(max_pp)))) {
                        DEBUG_PHP_EXT_DEBUG("Unable to set query predicate");
                        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
                            "Unable to set query predicate");
                    }
                }
            }
        }
        if (!between_unpacked) {
            DEBUG_PHP_EXT_DEBUG("Aerospike::query() the BETWEEN 'op' requires an array of (min,max) integers.");
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
                "Aerospike::query() the BETWEEN 'op' requires an array of (min,max) integers.");
            goto exit;
        }
    } else {
        DEBUG_PHP_EXT_DEBUG("Aerospike::query() unsupported 'op' in parameter 3.");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Aerospike::query() unsupported 'op' in parameter 3.");
        goto exit;
    }

    PHP_EXT_SET_AS_ERR(error_p, DEFAULT_ERRORNO, DEFAULT_ERROR);

exit:
    return error_p->code;
}

/*
 ******************************************************************************************************
 Executes a query in the Aerospike DB.
 *
 * @param as_object_p               The C client's aerospike object.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param namespace_p               The namespace to scan.
 * @param set_p                     The set to scan.
 * @param user_func_p               The user's callback to be applied per record
 *                                  that is scanned.
 * @param bins_ht_p                 The HashTable for optional filter bins array.
 * @param predicate_p               The HashTable for Query Predicate array.
 * @param options_p                 The optional policy.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status
aerospike_query_run(aerospike* as_object_p, as_error* error_p, char* namespace_p,
        char* set_p, userland_callback* user_func_p, HashTable* bins_ht_p,
        HashTable* predicate_ht_p, zval* options_p)
{
    as_query            query;
    bool                is_init_query = false;
    as_policy_query     query_policy;

    if ((!as_object_p) || (!error_p) || (!namespace_p) || (!set_p)) {
        DEBUG_PHP_EXT_DEBUG("Unable to initiate query");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to initiate query");
        goto exit;
    }

    set_policy(NULL, NULL, NULL, NULL, NULL, NULL, &query_policy, NULL,
            options_p, error_p);
    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    as_query_init(&query, namespace_p, set_p);
    is_init_query = true;
    as_query_where_inita(&query, 1);
    if (AEROSPIKE_OK != (aerospike_query_define(&query, error_p, namespace_p,
                    set_p, predicate_ht_p, NULL, NULL, NULL))) {
        DEBUG_PHP_EXT_DEBUG("Unable to define scan");
        goto exit;
    }

    if (bins_ht_p) {
        as_query_select_inita(&query, zend_hash_num_elements(bins_ht_p));
        HashPosition pos;
        zval **bin_names_pp = NULL;
        foreach_hashtable(bins_ht_p, pos, bin_names_pp) {
            if (Z_TYPE_PP(bin_names_pp) != IS_STRING) {
                convert_to_string_ex(bin_names_pp);
            }
            if (!as_query_select(&query, Z_STRVAL_PP(bin_names_pp))) {
                DEBUG_PHP_EXT_DEBUG("Unable to apply filter bins to the query");
                PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR,
                        "Unable to apply filter bins to the query");
                goto exit;
            }
        }
        if (AEROSPIKE_OK != (aerospike_query_foreach(as_object_p, error_p,
                        &query_policy, &query,
                        aerospike_helper_record_stream_callback,
                        user_func_p))) {
            DEBUG_PHP_EXT_DEBUG(error_p->message);
            goto exit;
        }
    } else if (AEROSPIKE_OK != (aerospike_query_foreach(as_object_p, error_p,
                    NULL, &query, aerospike_helper_record_stream_callback,
                    user_func_p))) {
        DEBUG_PHP_EXT_DEBUG(error_p->message);
        goto exit;
    }
exit:
    if (is_init_query) {
        as_query_destroy(&query);
    }
    return error_p->code;
}

/*
 ******************************************************************************************************
 Executes a query aggregation in the Aerospike DB by applying the UDF.
 *
 * @param as_object_p               The C client's aerospike object.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param module_p                  The name of UDF module containing the function
 *                                  to execute.
 * @param function_p                The name of the function to be applied to
 *                                  the record.
 * @param args_pp                   An array of arguments for the UDF.
 * @param namespace_p               The namespace to scan.
 * @param set_p                     The set to scan.
 * @param bins_ht_p                 The HashTable for optional filter bins array.
 * @param predicate_p               The HashTable for Query Predicate array.
 * @param return_value_p            The return value of aggregation to be
 *                                  populated by this method.
 * @param options_p                 The optional policy.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status
aerospike_query_aggregate(aerospike* as_object_p, as_error* error_p,
        const char* module_p, const char* function_p, zval** args_pp,
        char* namespace_p, char* set_p, HashTable* bins_ht_p,
        HashTable* predicate_ht_p, zval* return_value_p, zval* options_p)
{
    as_arraylist                args_list;
    as_arraylist*               args_list_p = NULL;
    as_static_pool              udf_pool = {0};
    uint32_t                    serializer_policy = -1;
    as_policy_query             query_policy;
    as_query                    query;
    bool                        is_init_query = false;
    foreach_callback_udata      aggregate_result_callback_udata;

    if ((!as_object_p) || (!error_p) || (!module_p) || (!function_p) ||
            (!args_pp && (!(*args_pp))) || (!namespace_p) || (!set_p) ||
            (!predicate_ht_p) || (!return_value_p)) {
        DEBUG_PHP_EXT_DEBUG("Unable to initiate query aggregation");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to initiate query aggregation");
        goto exit;
    }

    set_policy(NULL, NULL, NULL, NULL, NULL, NULL, &query_policy,
            &serializer_policy, options_p, error_p);
    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    if ((*args_pp)) {
        as_arraylist_init(&args_list,
                zend_hash_num_elements(Z_ARRVAL_PP(args_pp)), 0);
        args_list_p = &args_list;
        AS_LIST_PUT(NULL, args_pp, &args_list, &udf_pool,
                serializer_policy, error_p);
        if (AEROSPIKE_OK != (error_p->code)) {
            DEBUG_PHP_EXT_DEBUG("Unable to create args list for UDF");
            goto exit;
        }
    }
 
    as_query_init(&query, namespace_p, set_p);
    is_init_query = true;
    as_query_where_inita(&query, 1);
    if (AEROSPIKE_OK != (aerospike_query_define(&query, error_p, namespace_p,
                    set_p, predicate_ht_p, NULL, NULL,
                    NULL))) {
        DEBUG_PHP_EXT_DEBUG("Unable to define query");
        goto exit;
    }

    if (module_p && function_p && (!as_query_apply(&query, module_p,
                    function_p, (as_list *) &args_list))) {
        DEBUG_PHP_EXT_DEBUG("Unable to initiate UDF on the query");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR,
                "Unable to initiate UDF on the query");
        goto exit;
    }

    aggregate_result_callback_udata.udata_p = return_value_p;
    aggregate_result_callback_udata.error_p = error_p;

    if (bins_ht_p) {
        as_query_select_inita(&query, zend_hash_num_elements(bins_ht_p));
        HashPosition pos;
        zval **bin_names_pp = NULL;
        foreach_hashtable(bins_ht_p, pos, bin_names_pp) {
            if (Z_TYPE_PP(bin_names_pp) != IS_STRING) {
                convert_to_string_ex(bin_names_pp);
            }
            if (!as_query_select(&query, Z_STRVAL_PP(bin_names_pp))) {
                DEBUG_PHP_EXT_DEBUG("Unable to apply filter bins to the query");
                PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR,
                        "Unable to apply filter bins to the query");
                goto exit;
            }
        }

        if (AEROSPIKE_OK != (aerospike_query_foreach(as_object_p, error_p,
                        &query_policy, &query,
                        aerospike_helper_aggregate_callback,
                        &aggregate_result_callback_udata))) {
            DEBUG_PHP_EXT_DEBUG(error_p->message);
            goto exit;
        }
    } else if (AEROSPIKE_OK != (aerospike_query_foreach(as_object_p, error_p,
                    &query_policy, &query, aerospike_helper_aggregate_callback,
                    &aggregate_result_callback_udata))) {
        DEBUG_PHP_EXT_DEBUG(error_p->message);
        goto exit;
    }
exit:
    if (args_list_p) {
        as_arraylist_destroy(args_list_p);
    }

    if (is_init_query) {
        as_query_destroy(&query);
    }

    aerospike_helper_free_static_pool(&udf_pool);
    return error_p->code;
}
