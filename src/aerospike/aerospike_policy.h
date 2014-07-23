#ifndef __AEROSPIKE_POLICY_H__
#define __AEROSPIKE_POLICY_H__

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

extern as_status set_policy(as_policy_read *read_policy, as_policy_write *write_policy, zval *options);
extern as_status set_policy_operations(as_policy_operate *operate_policy, zval *options);
extern as_status set_general_policies(as_config* as_config_p, zval *options);
extern as_status set_policy_remove(as_policy_remove *remove_policy, zval *options);
extern as_status declare_policy_constants_php(zend_class_entry *Aerospike_ce);

#endif /* end of __AEROSPIKE_POLICY_H__ */
