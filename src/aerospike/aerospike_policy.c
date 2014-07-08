#include "php.h"

#include "aerospike/as_policy.h"
#include "aerospike/as_status.h"

#include "aerospike_common.h"
#include "aerospike_policy.h"

#define MAX_CONSTANT_STR_SIZE 512

typedef struct Aerospike_Constants {
    int constantno;
    char constant_str[MAX_CONSTANT_STR_SIZE];
} AerospikeConstants;


static 
AerospikeConstants aerospike_constants[] = {
    { OPT_CONNECT_TIMEOUT               ,   "OPT_CONNECT_TIMEOUT"               },
    { OPT_READ_TIMEOUT                  ,   "OPT_READ_TIMEOUT"                  },
    { OPT_WRITE_TIMEOUT                 ,   "OPT_WRITE_TIMEOUT"                 },
    { OPT_POLICY_RETRY                  ,   "OPT_POLICY_RETRY"                  },
    { OPT_POLICY_EXISTS                 ,   "OPT_POLICY_EXISTS"                 },
    { POLICY_RETRY_NONE                 ,   "POLICY_RETRY_NONE"                 },
    { POLICY_RETRY_ONCE                 ,   "POLICY_RETRY_ONCE"                 },
    { POLICY_EXISTS_IGNORE              ,   "POLICY_EXISTS_IGNORE"              },
    { POLICY_EXISTS_CREATE              ,   "POLICY_EXISTS_CREATE"              },
    { POLICY_EXISTS_UPDATE              ,   "POLICY_EXISTS_UPDATE"              },
    { POLICY_EXISTS_REPLACE             ,   "POLICY_EXISTS_REPLACE"             },
    { POLICY_EXISTS_CREATE_OR_REPLACE   ,   "POLICY_EXISTS_CREATE_OR_REPLACE"   }
};

#define AEROSPIKE_CONSTANTS_ARR_SIZE (sizeof(aerospike_constants)/sizeof(AerospikeConstants))

extern
as_status declare_policy_constants_php(zend_class_entry *Aerospike_ce)
{
    int32_t i;
    as_status   status = AEROSPIKE_OK;

    if (!Aerospike_ce) {
       status = AEROSPIKE_ERR;
       goto exit;
    }

    for (i = 0; i <= AEROSPIKE_CONSTANTS_ARR_SIZE; i++) {
        zend_declare_class_constant_long(
                Aerospike_ce, aerospike_constants[i].constant_str,
                    strlen(aerospike_constants[i].constant_str),
                        aerospike_constants[i].constantno TSRMLS_CC);
    }

exit:
    return status;
}

as_status set_policy(as_policy_read *read_policy, as_policy_write *write_policy, zval *options)
{
    as_status error_code = AEROSPIKE_OK;
    int32_t initialize = 1;

    if ((!read_policy) && (!write_policy)) {
        error_code = AEROSPIKE_ERR;
        goto failure;
    }

    if ((read_policy) && (write_policy)) {
        initialize = 0;
    }

    if (read_policy && initialize) {
        as_policy_read_init(read_policy);
    }

    if (write_policy && initialize) {
        as_policy_write_init(write_policy);
    }
    
    if (options != NULL) {
        HashTable *options_array = Z_ARRVAL_P(options);
        HashPosition options_pointer;
        zval **options_value;
        char *options_key;

        foreach_hashtable(options_array, options_pointer, options_value) {
            uint options_key_len;
            ulong options_index;

            if (zend_hash_get_current_key_ex(options_array, &options_key, &options_key_len, &options_index, 0, &options_pointer) != HASH_KEY_IS_LONG) {
                /*
                 * For now, using AEROSPIKE_ERR. Need to have specific
                 * error code here.
                 */
                error_code = AEROSPIKE_ERR;
                goto failure;
            }
            switch((int) options_index) {
                case OPT_READ_TIMEOUT:
                    if ((!read_policy) || (Z_TYPE_PP(options_value) != IS_LONG)) {
                        error_code = AEROSPIKE_ERR_TIMEOUT;
                        goto failure;
                    }
                    read_policy->timeout = (uint32_t) Z_LVAL_PP(options_value);
                    break;
                case OPT_WRITE_TIMEOUT:
                    if ((!write_policy) || (Z_TYPE_PP(options_value) != IS_LONG)) {
                        error_code = AEROSPIKE_ERR_TIMEOUT;
                        goto failure;
                    }
                    write_policy->timeout = (uint32_t) Z_LVAL_PP(options_value);
                    break;
                case OPT_POLICY_EXISTS:
                    if ((!write_policy) || (Z_TYPE_PP(options_value) != IS_LONG)) {
                        /*
                         * For now, using AEROSPIKE_ERR. Need to have specific
                         * error code here.
                         */
                        error_code = AEROSPIKE_ERR;
                        goto failure;
                    }
                    if ((Z_LVAL_PP(options_value) & AS_POLICY_EXISTS) == AS_POLICY_EXISTS) {
                        write_policy->exists = Z_LVAL_PP(options_value) - AS_POLICY_EXISTS + 1;
                    } else {
                        /*
                         * For now, using AEROSPIKE_ERR. Need to have specific
                         * error code here.
                         */
                        error_code = AEROSPIKE_ERR;
                        goto failure;
                    }
                    break;
                case OPT_POLICY_RETRY:
                    if ((!write_policy) || (Z_TYPE_PP(options_value) != IS_LONG)) {
                        /*
                         * For now, using AEROSPIKE_ERR. Need to have specific
                         * error code here.
                         */
                        error_code = AEROSPIKE_ERR;
                        goto failure;
                    }
                    if ((Z_LVAL_PP(options_value) & AS_POLICY_RETRY) == AS_POLICY_RETRY) {
                        write_policy->retry = Z_LVAL_PP(options_value) - AS_POLICY_RETRY + 1;
                    } else {
                        /*
                         * For now, using AEROSPIKE_ERR. Need to have specific
                         * error code here.
                         */
                        error_code = AEROSPIKE_ERR;
                        goto failure;
                    }
                    break;
                default:
                    /*
                     * For now, using AEROSPIKE_ERR. Need to have specific
                     * error code here.
                     */
                    error_code = AEROSPIKE_ERR;
                    goto failure;
            }
        }
    }
failure:
    return error_code;
}

