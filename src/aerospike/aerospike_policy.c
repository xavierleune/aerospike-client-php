#include "php.h"

#include "aerospike/aerospike_key.h"
#include "aerospike/as_config.h"
#include "aerospike/as_policy.h"
#include "aerospike/as_status.h"

#include "aerospike_common.h"
#include "aerospike_policy.h"

#define MAX_CONSTANT_STR_SIZE 512
#define NESTING_DEPTH_PHP_INI INI_STR("aerospike.nesting_depth") ? atoi(INI_STR("aerospike.nesting_depth")) : 0
#define CONNECT_TIMEOUT_PHP_INI INI_STR("aerospike.connect_timeout") ? (uint32_t) atoi(INI_STR("aerospike.connect_timeout")) : 0
#define READ_TIMEOUT_PHP_INI INI_STR("aerospike.read_timeout") ? (uint32_t) atoi(INI_STR("aerospike.read_timeout")) : 0
#define WRITE_TIMEOUT_PHP_INI INI_STR("aerospike.write_timeout") ? (uint32_t) atoi(INI_STR("aerospike.write_timeout")) : 0
#define LOG_PATH_PHP_INI INI_STR("aerospike.log_path") ? INI_STR("aerospike.log_path") : NULL
#define LOG_LEVEL_PHP_INI INI_STR("aerospike.log_level") ? INI_STR("aerospike.log_level") : NULL

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

/*
 * as_config_p, read_policy, write_policy pointers and not checked.
 * Calling functions should check them.
 */
static void
check_and_set_default_policies(as_config *as_config_p, 
                               as_policy_read *read_policy, 
                               as_policy_write *write_policy, 
                               as_policy_operate *operate_policy,
                               as_policy_remove *remove_policy)
{
    uint32_t ini_timeout = 0;
    if ((ini_timeout = READ_TIMEOUT_PHP_INI) && read_policy) {
        read_policy->timeout = ini_timeout;
    }
    if ((ini_timeout = WRITE_TIMEOUT_PHP_INI) && write_policy) {
        write_policy->timeout = ini_timeout;
    }
    if ((ini_timeout = WRITE_TIMEOUT_PHP_INI) && operate_policy) {
        operate_policy->timeout = ini_timeout;
    }
    if ((ini_timeout = WRITE_TIMEOUT_PHP_INI) && remove_policy) {
        remove_policy->timeout = ini_timeout;
    }
    if ((ini_timeout = CONNECT_TIMEOUT_PHP_INI) && as_config_p) {
        as_config_p->conn_timeout_ms = ini_timeout;
    }
}

static as_status
set_policy_ex(as_config *as_config_p, 
              as_policy_read *read_policy_p, 
              as_policy_write *write_policy_p, 
              as_policy_operate *operate_policy_p, 
              as_policy_remove *remove_policy_p,
              zval *options_p)
{
    as_status error_code = AEROSPIKE_OK;
    int32_t initialize = 1;

    if ((!read_policy_p) && (!write_policy_p) && 
        (!operate_policy_p) && (!remove_policy_p)) {
        error_code = AEROSPIKE_ERR;
        goto failure;
    }

    // case: connect
    if ((read_policy_p) && (write_policy_p)) {
        initialize = 0;
    }

    // case: get
    if (read_policy_p && initialize) {
        as_policy_read_init(read_policy_p);
    }

    // case: put
    if (write_policy_p && initialize) {
        as_policy_write_init(write_policy_p);
    }
    
    // case: operate
    if (operate_policy_p && initialize) {
        as_policy_operate_init(operate_policy_p);
    }

    // case: remove
    if (remove_policy_p && initialize) {
        as_policy_remove_init(remove_policy_p);
    }

    if (options_p == NULL) {
        check_and_set_default_policies(as_config_p, read_policy_p, 
                       write_policy_p, operate_policy_p, remove_policy_p);
    } else {
        HashTable*          options_array = Z_ARRVAL_P(options_p);
        HashPosition        options_pointer;
        zval**              options_value;
        int8_t*             options_key;
        int16_t             read_flag = 0;
        int16_t             write_flag = 0;
        int16_t             connect_flag = 0;

        foreach_hashtable(options_array, options_pointer, options_value) {
            uint options_key_len;
            ulong options_index;

            if (zend_hash_get_current_key_ex(options_array, &options_key, 
                        &options_key_len, &options_index, 0, &options_pointer) != HASH_KEY_IS_LONG) {
                error_code = AEROSPIKE_ERR;
                goto failure;
            }
            switch((int) options_index) {
                case OPT_CONNECT_TIMEOUT:
                    if ((!as_config_p) || (Z_TYPE_PP(options_value) != IS_LONG)) {
                        error_code = AEROSPIKE_ERR_TIMEOUT;
                        goto failure;
                    }
                    as_config_p->conn_timeout_ms = (uint32_t) Z_LVAL_PP(options_value);
                    connect_flag = 1;
                    break;
                case OPT_READ_TIMEOUT:
                    if ((!read_policy_p) || (Z_TYPE_PP(options_value) != IS_LONG)) {
                        error_code = AEROSPIKE_ERR_TIMEOUT;
                        goto failure;
                    }
                    read_policy_p->timeout = (uint32_t) Z_LVAL_PP(options_value);
                    read_flag = 1;
                    break;
                case OPT_WRITE_TIMEOUT:
                    if ((Z_TYPE_PP(options_value) != IS_LONG)) {
                        error_code = AEROSPIKE_ERR_TIMEOUT;
                        goto failure;
                    }
                    if (write_policy_p) {
                        write_policy_p->timeout = (uint32_t) Z_LVAL_PP(options_value);
                    } else if(operate_policy_p) {
                        operate_policy_p->timeout = (uint32_t) Z_LVAL_PP(options_value);
                    } else if(remove_policy_p) {
                        remove_policy_p->timeout = (uint32_t) Z_LVAL_PP(options_value);
                    } else {
                        goto failure;
                    }
                    write_flag = 1;
                    break;
                case OPT_POLICY_EXISTS:
                    if ((!write_policy_p) || (Z_TYPE_PP(options_value) != IS_LONG)) {
                        error_code = AEROSPIKE_ERR;
                        goto failure;
                    }
                    if ((Z_LVAL_PP(options_value) & AS_POLICY_EXISTS) == AS_POLICY_EXISTS) {
                        write_policy_p->exists = Z_LVAL_PP(options_value) - AS_POLICY_EXISTS + 1;
                    } else {
                        error_code = AEROSPIKE_ERR;
                        goto failure;
                    }
                    break;
                case OPT_POLICY_RETRY:
                    if((Z_TYPE_PP(options_value) != IS_LONG) && ((Z_LVAL_PP(options_value) & AS_POLICY_RETRY) != AS_POLICY_RETRY)) {
                        error_code = AEROSPIKE_ERR;
                        goto failure;
                    }
                    if (write_policy_p) {
                        write_policy_p->retry = Z_LVAL_PP(options_value) - AS_POLICY_RETRY + 1;
                    } else if(operate_policy_p) {
                        operate_policy_p->retry = Z_LVAL_PP(options_value) - AS_POLICY_RETRY + 1;
                    } else if(remove_policy_p) {
                        remove_policy_p->retry = Z_LVAL_PP(options_value) - AS_POLICY_RETRY + 1;
                    } else {
                        error_code = AEROSPIKE_ERR;
                        goto failure;
                    }
                    break;
                default:
                    error_code = AEROSPIKE_ERR;
                    goto failure;
            }
        }
        if (!write_flag && write_policy_p) {
            check_and_set_default_policies((connect_flag ? NULL : as_config_p), NULL, write_policy_p, NULL, NULL);
            connect_flag = 1;
        } 
        if (!read_flag && read_policy_p) {
            check_and_set_default_policies((connect_flag ? NULL : as_config_p), read_policy_p, NULL, NULL, NULL);
            connect_flag = 1;
        } 
        if (!connect_flag && as_config_p) {
            check_and_set_default_policies(as_config_p, NULL, NULL, NULL, NULL);
        }
    }
failure:
    return error_code;
}

extern as_status
set_policy(as_policy_read *read_policy_p, 
           as_policy_write *write_policy_p, 
           as_policy_operate *operate_policy_p, 
           as_policy_remove *remove_policy_p, 
           zval *options_p)
{
    return set_policy_ex(NULL, read_policy_p, write_policy_p, operate_policy_p, remove_policy_p, options_p);
}

extern as_status
set_general_policies(as_config *as_config_p, 
                     zval *options_p)
{
    as_status     status = AEROSPIKE_OK;

    if (!as_config_p) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    status = set_policy_ex(as_config_p, &as_config_p->policies.read, &as_config_p->policies.write, 
                           NULL, NULL, options_p);
exit:
    return status;
}
