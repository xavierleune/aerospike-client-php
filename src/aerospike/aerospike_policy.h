enum Aerospike_constants {
	OPT_CONNECT_TIMEOUT = 1, // value in milliseconds, default: 1000
	OPT_READ_TIMEOUT, // value in milliseconds, default: 1000
	OPT_WRITE_TIMEOUT, // value in milliseconds, default: 1000
	OPT_POLICY_RETRY, // set to a Aerospike::POLICY_RETRY_* value
	OPT_POLICY_EXISTS, // set to a Aerospike::POLICY_EXISTS_* value
};


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

const char *get_aerospike_constant_name(int aerospike_constant_value)
{
	switch(aerospike_constant_value) {
		case OPT_CONNECT_TIMEOUT:
			return "OPT_CONNECT_TIMEOUT";
		case OPT_READ_TIMEOUT:
			return "OPT_READ_TIMEOUT";
		case OPT_WRITE_TIMEOUT:
			return "OPT_WRITE_TIMEOUT";
		case OPT_POLICY_RETRY:
			return "OPT_POLICY_RETRY";
		case OPT_POLICY_EXISTS:
			return "OPT_POLICY_EXISTS";
		
	}
}
