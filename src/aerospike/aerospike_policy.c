#include "aerospike_policy.h"

int set_read_policy(as_policy_read *read_policy, zval *options)
{
    as_policy_write_init(read_policy);
    int error_code = AEROSPIKE_OK;
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
                    if (Z_TYPE_PP(options_value) != IS_LONG) {
                        error_code = AEROSPIKE_ERR_TIMEOUT;
                        goto failure;
                    }
                    read_policy->timeout = (uint32_t) Z_LVAL_PP(options_value);
                    break;
                case OPT_POLICY_EXISTS:
                    if (Z_TYPE_PP(options_value) != IS_LONG) {
                        /*
                         * For now, using AEROSPIKE_ERR. Need to have specific
                         * error code here.
                         */
                        error_code = AEROSPIKE_ERR;
                        goto failure;
                    }
                    if ((Z_LVAL_PP(options_value) & AS_POLICY_EXISTS) == AS_POLICY_EXISTS) {
                        read_policy->exists = Z_LVAL_PP(options_value) - AS_POLICY_EXISTS + 1;
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
                    if (Z_TYPE_PP(options_value) != IS_LONG) {
                        /*
                         * For now, using AEROSPIKE_ERR. Need to have specific
                         * error code here.
                         */
                        error_code = AEROSPIKE_ERR;
                        goto failure;
                    }
                    if ((Z_LVAL_PP(options_value) & AS_POLICY_RETRY) == AS_POLICY_RETRY) {
                        read_policy->retry = Z_LVAL_PP(options_value) - AS_POLICY_RETRY + 1;
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

int set_write_policy(as_policy_write *write_policy, zval *options)
{
    as_policy_write_init(write_policy);
    int error_code = AEROSPIKE_OK;
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
                case OPT_WRITE_TIMEOUT:
                    if (Z_TYPE_PP(options_value) != IS_LONG) {
                        error_code = AEROSPIKE_ERR_TIMEOUT;
                        goto failure;
                    }
                    write_policy->timeout = (uint32_t) Z_LVAL_PP(options_value);
                    break;
                case OPT_POLICY_EXISTS:
                    if (Z_TYPE_PP(options_value) != IS_LONG) {
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
                    if (Z_TYPE_PP(options_value) != IS_LONG) {
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
