#include "php.h"

#include "aerospike/aerospike.h"
#include "aerospike/as_error.h"
#include "aerospike_common.h"
#include "aerospike_general_constants.h"

/*
 *******************************************************************************************************
 * Wrapper function to perform an aerospike_index_create within the C client.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param ns_p                  The namespace name for which index is to be
 *                              created.
 * @param set_p                 The set name for which index is to be created.
 * @param bin_p                 The bin name for which index is to be created.
 * @param type                  The type of the index to be created.
 * @param name_p                The index name for which index is to be
 *                              created.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_index_create_php(aerospike* as_object_p, as_error *error_p,
        char* ns_p, char* set_p, char* bin_p, uint32_t type,
        char *name_p, zval* options_p TSRMLS_DC)
{
    as_status                   status = AEROSPIKE_OK;
    as_policy_info              info_policy;

    set_policy(NULL, NULL, NULL, NULL, &info_policy, NULL, NULL, NULL,
            options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    if ((!error_p) || (!as_object_p)) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to create index");
        DEBUG_PHP_EXT_DEBUG("Unable to create index");
        goto exit;
    }

    switch (type) {
        case INDEX_TYPE_STRING:
            if (AEROSPIKE_OK !=
                    (status = aerospike_index_integer_create(as_object_p, error_p,
                                                             &info_policy, ns_p,
                                                             set_p, bin_p, name_p))) {
                DEBUG_PHP_EXT_DEBUG(error_p->message);
                goto exit;
            }
            break;

        case INDEX_TYPE_INTEGER:
            if (AEROSPIKE_OK !=
                    (status = aerospike_index_string_create(as_object_p, error_p,
                                                            &info_policy, ns_p,
                                                            set_p, bin_p, name_p))) {
                DEBUG_PHP_EXT_DEBUG(error_p->message);
                goto exit;
            }
            break;

        default:
            status = AEROSPIKE_ERR_PARAM;
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Invalid index type");
            DEBUG_PHP_EXT_DEBUG("Invalid index type");
            goto exit;
    }

exit:
    return(status);
}

/*
 *******************************************************************************************************
 * Wrapper function to perform an aerospike_index_remove within the C client.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param ns_p                  The namespace name for which index is to be
 *                              removed.
 * @param name_p                The index name for which index is to be
 *                              removed.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_index_remove_php(aerospike* as_object_p, as_error *error_p,
        char* ns_p, char *name_p, zval* options_p TSRMLS_DC)
{
    as_status                   status = AEROSPIKE_OK;
    as_policy_info              info_policy;

    set_policy(NULL, NULL, NULL, NULL, &info_policy, NULL, NULL, NULL,
            options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    if ((!error_p) || (!as_object_p)) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to drop index");
        DEBUG_PHP_EXT_DEBUG("Unable to drop index");
        goto exit;
    }

    if (AEROSPIKE_OK !=
            (status = aerospike_index_remove(as_object_p, error_p,
                                             &info_policy, ns_p, name_p))) {
        DEBUG_PHP_EXT_DEBUG(error_p->message);
        goto exit;
    }

exit:
    return(status);
}
