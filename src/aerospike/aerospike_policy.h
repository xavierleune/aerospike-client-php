#ifndef __AEROSPIKE_POLICY_H__
#define __AEROSPIKE_POLICY_H__

/*
 *******************************************************************************************************
 * Enum for PHP client's optional policy constant keys. (OPT_*)
 *******************************************************************************************************
 */
enum Aerospike_constants {
    OPT_CONNECT_TIMEOUT = 1,  /* value in milliseconds, default: 1000 */
    OPT_READ_TIMEOUT,         /* value in milliseconds, default: 1000 */
    OPT_WRITE_TIMEOUT,        /* value in milliseconds, default: 1000 */
    OPT_POLICY_RETRY,         /* set to a Aerospike::POLICY_RETRY_* value */
    OPT_POLICY_EXISTS,        /* set to a Aerospike::POLICY_EXISTS_* value */
    OPT_SERIALIZER            /* set the unsupported type handler */
};

/*
 *******************************************************************************************************
 * These values are redundant with the read/write policy
 * implementation of CSDK. It can be removed once CSDK
 * starts enforcing the policies. This implementation enforces
 * it in PHP SDK. 
 *
 * Usage:
 *
 * While adding any new Aerospike class constant, declare a routing
 * constant using next available Hexadecimal number (For eg. 0x00100000).
 * Declare your constant in Aerospike_values with value equal to the formerly
 * defined hexadecimal constant.
 *
 * This can be used to perform additional check on PHP client side to
 * distinguish between the different groups of constants and refrain the
 * overlapping constant values of C client to harm the intended behaviour.
 *
 * For instance, Consider the following POLICY_RETRY_* check in the source file
 * aerospike_policy.c:
 *
 * if ((Z_LVAL_PP(options_value) & AS_POLICY_RETRY) != AS_POLICY_RETRY) {
 *      goto exit;
 * } else {
 *     if (write_policy_p) {
 *         write_policy_p->retry = Z_LVAL_PP(options_value) - AS_POLICY_RETRY + 1;
 * }
 *
 * Here, the bitwise ANDing with AS_POLICY_RETRY ensures that the constant value
 * passed by user in Z_VAL_PP(options_value) is of type AS_POLICY_RETRY and only
 * then does it map the value passed in by user to the corresponding constant
 * for AS_POLICY_RETRY in the C client. (Here, the mapping is done by
 * subtracting the value of AS_POLICY_RETRY from the user's value and adding 1
 * to it.)
 *******************************************************************************************************
 */
#define AS_POLICY_RETRY 0x00000010
#define AS_POLICY_EXISTS 0x00000100
#define AS_SERIALIZER_TYPE 0x00001000
#define AS_UDF_TYPE 0x00010000

/*
 *******************************************************************************************************
 * Enum for PHP client's optional policy constant values. (POLICY_* or SERIALIZER_*)
 *******************************************************************************************************
 */
enum Aerospike_values {
    POLICY_RETRY_NONE      = AS_POLICY_RETRY,       /* do not retry an operation (default behavior for policy_retry) */
    POLICY_RETRY_ONCE,                              /* allow for a single retry on an operation */
    POLICY_EXISTS_IGNORE   = AS_POLICY_EXISTS,      /* write record regardless of existence */
    POLICY_EXISTS_CREATE,                           /* create a record ONLY if it DOES NOT exist */
    POLICY_EXISTS_UPDATE,                           /* update a record ONLY if it exists */
    POLICY_EXISTS_REPLACE,                          /* replace a record ONLY if it exists */
    POLICY_EXISTS_CREATE_OR_REPLACE,                /* default behavior for policy_exists */
    SERIALIZER_NONE        = AS_SERIALIZER_TYPE,
    SERIALIZER_PHP,                                 /* default handler for serializer type */
    SERIALIZER_JSON,
    SERIALIZER_USER,
    UDF_TYPE_LUA           = AS_UDF_TYPE            /* UDF language type */
};

#define MAX_CONSTANT_STR_SIZE 512
/* 
 *******************************************************************************************************
 * Structure to map constant number to constant name string for Aerospike constants.
 *******************************************************************************************************
 */
typedef struct Aerospike_Constants {
    int     constantno;
    char    constant_str[MAX_CONSTANT_STR_SIZE];
} AerospikeConstants;

#define AEROSPIKE_CONSTANTS_ARR_SIZE (sizeof(aerospike_constants)/sizeof(AerospikeConstants))

/* 
 *******************************************************************************************************
 * Instance of Mapper of constant number to constant name string for Aerospike constants.
 *******************************************************************************************************
 */
static 
AerospikeConstants aerospike_constants[] = {
    { OPT_CONNECT_TIMEOUT               ,   "OPT_CONNECT_TIMEOUT"               },
    { OPT_READ_TIMEOUT                  ,   "OPT_READ_TIMEOUT"                  },
    { OPT_WRITE_TIMEOUT                 ,   "OPT_WRITE_TIMEOUT"                 },
    { OPT_POLICY_RETRY                  ,   "OPT_POLICY_RETRY"                  },
    { OPT_POLICY_EXISTS                 ,   "OPT_POLICY_EXISTS"                 },
    { OPT_SERIALIZER                    ,   "OPT_SERIALIZER"                    },
    { POLICY_RETRY_NONE                 ,   "POLICY_RETRY_NONE"                 },
    { POLICY_RETRY_ONCE                 ,   "POLICY_RETRY_ONCE"                 },
    { POLICY_EXISTS_IGNORE              ,   "POLICY_EXISTS_IGNORE"              },
    { POLICY_EXISTS_CREATE              ,   "POLICY_EXISTS_CREATE"              },
    { POLICY_EXISTS_UPDATE              ,   "POLICY_EXISTS_UPDATE"              },
    { POLICY_EXISTS_REPLACE             ,   "POLICY_EXISTS_REPLACE"             },
    { POLICY_EXISTS_CREATE_OR_REPLACE   ,   "POLICY_EXISTS_CREATE_OR_REPLACE"   },
    { SERIALIZER_NONE                   ,   "SERIALIZER_NONE"                   },
    { SERIALIZER_PHP                    ,   "SERIALIZER_PHP"                    },
    { SERIALIZER_JSON                   ,   "SERIALIZER_JSON"                   },
    { SERIALIZER_USER                   ,   "SERIALIZER_USER"                   },
    { UDF_TYPE_LUA                      ,   "UDF_TYPE_LUA"                      }
};
/*
 *******************************************************************************************************
 * Extern declarations of policy functions.
 *******************************************************************************************************
 */
extern void
set_policy(as_policy_read *read_policy_p,
           as_policy_write *write_policy_p,
           as_policy_operate *operate_policy_p,
           as_policy_remove *remove_policy_p,
           as_policy_info *info_policy_p,
           uint32_t *serializer_policy_p,
           zval *options_p,
           as_error *error_p TSRMLS_DC);
extern void 
set_general_policies(as_config* as_config_p, 
                     zval *options_p,
                     as_error *error_p TSRMLS_DC);

#endif /* end of __AEROSPIKE_POLICY_H__ */
