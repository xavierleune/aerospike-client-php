

enum Aerospike_constants {
	OPT_CONNECT_TIMEOUT = 1, // value in milliseconds, default: 1000
	OPT_READ_TIMEOUT, // value in milliseconds, default: 1000
	OPT_WRITE_TIMEOUT, // value in milliseconds, default: 1000
	OPT_POLICY_RETRY, // set to a Aerospike::POLICY_RETRY_* value
	OPT_POLICY_EXISTS, // set to a Aerospike::POLICY_EXISTS_* value
};

/*
 * These values are redundant with the read/write policy
 * implementation of CSDK. It can be removed once CSDK
 * starts enforcing the policies. This implementation enforces
 * it in PHP SDK.
 */
#define AS_POLICY_RETRY 0x00000010
#define AS_POLICY_EXISTS 0x00000100

enum Aerospike_values {	
	POLICY_RETRY_NONE = AS_POLICY_RETRY, // do not retry an operation (default)
	POLICY_RETRY_ONCE, // allow for a single retry on an operation
	POLICY_EXISTS_IGNORE =  AS_POLICY_EXISTS, // write record regardless of existence 
	POLICY_EXISTS_CREATE, // create a record ONLY if it DOES NOT exist
	POLICY_EXISTS_UPDATE, // update a record ONLY if it exists
	POLICY_EXISTS_REPLACE, // replace a record ONLY if it exists
	POLICY_EXISTS_CREATE_OR_REPLACE // default behavior
};

#define MAX_CONSTANT_STR_SIZE 512

typedef struct Aerospike_Constants {
    int constantno;
    char constant_str[MAX_CONSTANT_STR_SIZE];
} AerospikeConstants;

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

#define EXPOSE_CONSTANTS_STR_ZEND(Aerospike_ce)                         \
do {                                                                    \
    int32_t i;                                                          \
    for (i = 0; i <= AEROSPIKE_CONSTANTS_ARR_SIZE; i++) {               \
        zend_declare_class_constant_long(                               \
                Aerospike_ce, aerospike_constants[i].constant_str,      \
                    strlen(aerospike_constants[i].constant_str),          \
                        aerospike_constants[i].constantno TSRMLS_CC);   \
    }                                                                   \
} while(0);

int set_read_policy(as_policy_read *read_policy, zval *options);
int set_write_policy(as_policy_write *write_policy, zval *options);
