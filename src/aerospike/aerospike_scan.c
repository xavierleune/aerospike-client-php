#include "php.h"
#include "aerospike/as_log.h"
#include "aerospike/as_key.h"
#include "aerospike/as_config.h"
#include "aerospike/as_error.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike.h"
#include "aerospike_common.h"
#include "aerospike/as_udf.h"
#include "aerospike/as_scan.h"
#include "aerospike_policy.h"

#define PROGRESS_PCT "progress_pct"
#define RECORDS_SCANNED "records_scanned"
#define STATUS "status"

/*
 ******************************************************************************************************
 Initializes and defines an as_scan object.
 *
 * @param as_scan_p                 The C client's as_scan object to be
 *                                  initialized.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param namespace_p               The namespace to scan.
 * @param set_p                     The set to scan.
 * @param percent                   The percentage of data to scan.
 * @param scan_priority             The priority levels for the scan operation.
 * @param concurrent                Whether to scan all nodes in parallel.
 * @param no_bins                   Whether to return only metadata (and no bins).
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
aerospike_scan_define(as_scan* scan_p, as_error* error_p, char* namespace_p,
        char* set_p, long percent, long scan_priority, bool concurrent,
        bool no_bins, char* module_p, char* function_p,
        as_list* args_list_p)
{
    /*
     * Please don't change location of as_scan_init().
     */
    as_scan_init(scan_p, namespace_p, set_p);

    if (percent < 0 || percent > 100) {
        DEBUG_PHP_EXT_DEBUG("Invalid value for scan percent");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR,
                "Invalid value for scan percent");
        goto exit;
    } else if (!as_scan_set_percent(scan_p, percent)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set scan percent");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR,
                "Unable to set scan percent");
        goto exit;
    }

    if ((scan_priority & AS_SCAN_PRIORITY) != AS_SCAN_PRIORITY) {
        DEBUG_PHP_EXT_DEBUG("Invalid value for scan priority");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR,
                "Invalid value for scan priority");
        goto exit;
    } else if (!as_scan_set_priority(scan_p,
                (scan_priority - AS_SCAN_PRIORITY))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set scan priority");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR,
                "Unable to set scan priority");
        goto exit;
    }

    if (!as_scan_set_concurrent(scan_p, concurrent)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set scan concurrency");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR,
                "Unable to set scan concurrency");
        goto exit;
    }

    if (!as_scan_set_nobins(scan_p, no_bins)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set scan no bins");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR,
                "Unable to set scan no bins");
        goto exit;
    }

    if (module_p && function_p && (!as_scan_apply_each(scan_p, module_p,
                    function_p, args_list_p))) {
        DEBUG_PHP_EXT_DEBUG("Unable to apply UDF on the scan");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR,
                "Unable to initiate background scan");
        goto exit;
    }

    PHP_EXT_SET_AS_ERR(error_p, DEFAULT_ERRORNO, DEFAULT_ERROR);

exit:
    return error_p->code;
}

/*
 ******************************************************************************************************
 Scans a set in the Aerospike DB.
 *
 * @param as_object_p               The C client's aerospike object.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param namespace_p               The namespace to scan.
 * @param set_p                     The set to scan.
 * @param user_func_p               The user's callback to be applied per record
 *                                  that is scanned.
 * @param bins_ht_p                 The HashTable for optional filter bins array.
 * @param percent                   The percentage of data to scan.
 * @param scan_priority             The priority levels for the scan operation.
 * @param concurrent                Whether to scan all nodes in parallel.
 * @param no_bins                   Whether to return only metadata (and no bins).
 * @param options_p                 The optional policy.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status
aerospike_scan_run(aerospike* as_object_p, as_error* error_p, char* namespace_p,
        char* set_p, userland_callback* user_func_p, HashTable* bins_ht_p,
        long percent, long scan_priority, bool concurrent, bool no_bins,
        zval* options_p)
{
    as_scan             scan;
    as_scan*            scan_p = NULL;
    as_policy_scan      scan_policy;

    if ((!as_object_p) || (!error_p) || (!namespace_p) || (!set_p)) {
        DEBUG_PHP_EXT_DEBUG("Unable to initiate scan");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to initiate scan");
        goto exit;
    }

    set_policy(NULL, NULL, NULL, NULL, NULL, &scan_policy, NULL, NULL,
            options_p, error_p);
    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    scan_p = &scan;
    if (AEROSPIKE_OK != (aerospike_scan_define(scan_p, error_p, namespace_p,
                    set_p, percent, scan_priority, concurrent, no_bins,
                    NULL, NULL, NULL))) {
        DEBUG_PHP_EXT_DEBUG("Unable to define scan");
        goto exit;
    }

    if (bins_ht_p) {
        as_scan_select_inita(&scan, zend_hash_num_elements(bins_ht_p));
        HashPosition pos;
        zval **bin_names_pp;
        foreach_hashtable(bins_ht_p, pos, bin_names_pp) {
            if (Z_TYPE_PP(bin_names_pp) != IS_STRING) {
                convert_to_string_ex(bin_names_pp);
            }
            as_scan_select(&scan, Z_STRVAL_PP(bin_names_pp));
        }
        if (AEROSPIKE_OK != (aerospike_scan_foreach(as_object_p, error_p, &scan_policy,
                        &scan, aerospike_helper_record_stream_callback, user_func_p))) {
            goto exit;
        }
    } else {
        if (AEROSPIKE_OK != (aerospike_scan_foreach(as_object_p, error_p, NULL,
                        &scan, aerospike_helper_record_stream_callback, user_func_p))) {
            goto exit;
        }
    }
exit:
    if (scan_p) {
        as_scan_destroy(scan_p);
    }
    return error_p->code;
}

/*
 ******************************************************************************************************
 Scans a set in the Aerospike DB and applies UDF on it.
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
 * @param scan_id_p                 The id for the scan job, which can be used
 *                                  for querying the status of the scan. This
 *                                  value shall be set by this function on
 *                                  success.
 * @param percent                   The percentage of data to scan.
 * @param scan_priority             The priority levels for the scan operation.
 * @param concurrent                Whether to scan all nodes in parallel.
 * @param no_bins                   Whether to return only metadata (and no bins).
 * @param options_p                 The optional policy.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status
aerospike_scan_run_background(aerospike* as_object_p, as_error* error_p,
        char* module_p, char* function_p, zval** args_pp, char* namespace_p,
        char* set_p, zval* scan_id_p, long percent, long scan_priority,
        bool concurrent, bool no_bins, zval* options_p)
{
    as_arraylist                args_list;
    as_arraylist*               args_list_p = NULL;
    as_static_pool              udf_pool = {0};
    uint32_t                    serializer_policy = -1;
    as_policy_scan              scan_policy;
    as_scan                     scan;
    as_scan*                    scan_p = NULL;
    uint64_t                    scan_id = 0;

    if ((!as_object_p) || (!error_p) || (!module_p) || (!function_p) ||
            (!namespace_p) || (!set_p) || (!scan_id_p)) {
        DEBUG_PHP_EXT_DEBUG("Unable to initiate background scan");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to initiate background scan");
        goto exit;
    }

    set_policy(NULL, NULL, NULL, NULL, NULL, &scan_policy, NULL,
            &serializer_policy, options_p, error_p);
    if (AEROSPIKE_OK != (error_p->code)) {
        printf("aerospike_scan_run\n");
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    if ((*args_pp)) {
        as_arraylist_inita(&args_list,
                zend_hash_num_elements(Z_ARRVAL_PP(args_pp)));
        args_list_p = &args_list;
        AS_LIST_PUT(NULL, args_pp, args_list_p, &udf_pool,
                serializer_policy, error_p);
        if (AEROSPIKE_OK != (error_p->code)) {
            DEBUG_PHP_EXT_DEBUG("Unable to create args list for UDF");
            goto exit;
        }
    }

    scan_p = &scan;

    if (AEROSPIKE_OK != (aerospike_scan_define(scan_p, error_p, namespace_p,
                    set_p, percent, scan_priority, concurrent, no_bins,
                    module_p, function_p, (as_list *) args_list_p))) {
        DEBUG_PHP_EXT_DEBUG("Unable to define scan");
        goto exit;
    }

    if (AEROSPIKE_OK != (aerospike_scan_background(as_object_p,
            error_p, &scan_policy, scan_p, &scan_id))) {
        DEBUG_PHP_EXT_DEBUG(error_p->message);
        goto exit;
    }

    ZVAL_LONG(scan_id_p, scan_id);

exit:
    if (args_list_p) {
        as_arraylist_destroy(args_list_p);
    }

    if (scan_p) {
        as_scan_destroy(scan_p);
    }

    aerospike_helper_free_static_pool(&udf_pool);
    return error_p->code;
}

/*
 ******************************************************************************************************
 Check the progress of a background scan running on the database.
 *
 * @param as_object_p               The C client's aerospike object.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param scan_id                   The id for the scan job, which can be used
 *                                  for querying the status of the scan.
 * @param scan_info                 Information about this scan, to be populated
 *                                  by this operation.
 * @param options_p                 The optional policy.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status
aerospike_scan_get_info(aerospike* as_object_p, as_error* error_p,
        uint64_t scan_id, zval* scan_info_p, zval* options_p)
{
    as_scan_info                scan_info;
    as_policy_info              info_policy;

    set_policy(NULL, NULL, NULL, NULL, &info_policy, NULL, NULL, NULL,
            options_p, error_p);
    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    if (AEROSPIKE_OK != (aerospike_scan_info(as_object_p, error_p,
                    &info_policy, scan_id, &scan_info))) {
        DEBUG_PHP_EXT_DEBUG(error_p->message);
        goto exit;
    }

    add_assoc_long(scan_info_p, PROGRESS_PCT, scan_info.progress_pct);
    add_assoc_long(scan_info_p, RECORDS_SCANNED, scan_info.records_scanned);
    add_assoc_long(scan_info_p, STATUS, (scan_info.status + AS_SCAN_STATUS));
exit:
    return error_p->code;
}
