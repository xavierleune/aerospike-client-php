
# The Aerospike class
The Aerospike PHP client API may be described as follows:

## Introduction

The main Aerospike class

```php

Aerospike
{
    //
    // Policy flags:
    // The policy constants map to the C client
    //  src/include/aerospike/as_policy.h
    //
    const POLICY_RETRY_NONE = 1; // do not retry an operation (default)
    const POLICY_RETRY_ONCE = 2; // allow for a single retry on an operation

    // By default writes will try to create or replace records and bins
    // behaving similar to an associative array in PHP. Setting
    // OPT_POLICY_EXISTS with one of these values will overwrite this.
    // POLICY_EXISTS_IGNORE (aka CREATE_OR_UPDATE) is the default value
    const POLICY_EXISTS_IGNORE = 1; // interleave bins of a record if it exists
    const POLICY_EXISTS_CREATE = 2; // create a record ONLY if it DOES NOT exist
    const POLICY_EXISTS_UPDATE = 3; // update a record ONLY if it exists
    const POLICY_EXISTS_REPLACE = 4; // replace a record ONLY if it exists
    const POLICY_EXISTS_CREATE_OR_REPLACE = 5; // overwrite the bins if record exists

    // Determines a handler for writing values of unsupported type into bins
    const SERIALIZER_NONE = 0;
    const SERIALIZER_PHP  = 1; // default handler
    const SERIALIZER_JSON = 2;
    const SERIALIZER_USER = 3;

    //
    // Options can be assigned values that modify default behavior
    //
    const OPT_CONNECT_TIMEOUT = 1; // value in milliseconds, default: 1000
    const OPT_READ_TIMEOUT = 2; // value in milliseconds, default: 1000
    const OPT_WRITE_TIMEOUT = 3; // value in milliseconds, default: 1000
    const OPT_POLICY_RETRY = 4; // set to a Aerospike::POLICY_RETRY_* value
    const OPT_POLICY_EXISTS = 5; // set to a Aerospike::POLICY_EXISTS_* value
    const OPT_SERIALIZER = 6; // set the unsupported type handler

    //
    // Aerospike Status Codes:
    //
    // Each Aerospike API method invocation returns a status code
    //  depending upon the success or failure condition of the call.
    //
    // The error status codes map to the C client
    //  src/include/aerospike/as_status.h

    //
    // Client status codes:
    //
    const OK                      =    0; // Generic success
    const ERR                     =  100; // Generic error
    const ERR_CLIENT              =  200; // Generic client error
    const ERR_PARAM               =  201; // Invalid client parameter
    const ERR_CLUSTER             =  300; // Cluster discovery and connection error
    const ERR_TIMEOUT             =  400; // Client-side timeout error
    const ERR_THROTTLED           =  401; // Client-side request throttling

    //
    // Server status codes:
    //
    const ERR_SERVER              =  500; // Generic server error
    const ERR_REQUEST_INVALID     =  501; // Invalid request protocol or protocol field
    const ERR_SERVER_FULL         =  503; // Node running out of memory/storage
    const ERR_CLUSTER_CHANGE      =  504; // Cluster state changed during the request
    const ERR_UNSUPPORTED_FEATURE =  505;
    const ERR_DEVICE_OVERLOAD     =  506; // Node storage lagging write load
    // Record specific:
    const ERR_RECORD              =  600; // Generic record error
    const ERR_RECORD_BUSY         =  601; // Hot key: too many concurrent requests for the record
    const ERR_RECORD_NOT_FOUND    =  602;
    const ERR_RECORD_EXISTS       =  603;
    const ERR_RECORD_GENERATION   =  604; // Write policy regarding generation violated
    const ERR_RECORD_TOO_BIG      =  605; // Record written cannot fit in storage write block
    const ERR_BIN_TYPE            =  606; // Bin modification failed due to value type
    const ERR_RECORD_KEY_MISMATCH =  607;
    // Scan operations:
    const ERR_SCAN                = 1000; // Generic scan error
    const ERR_SCAN_ABORTED        = 1001; // Scan aborted by the user
    // Query operations:
    const ERR_QUERY               = 1100; // Generic query error
    const ERR_QUERY_ABORTED       = 1101; // Query aborted by the user
    const ERR_QUERY_QUEUE_FULL    = 1102;
    // Index operations:
    const ERR_INDEX               = 1200; // Generic secondary index error
    const ERR_INDEX_OOM           = 1201; // Index out of memory
    const ERR_INDEX_NOT_FOUND     = 1202;
    const ERR_INDEX_FOUND         = 1203;
    const ERR_INDEX_NOT_READABLE  = 1204;
    const ERR_INDEX_NAME_MAXLEN   = 1205;
    const ERR_INDEX_MAXCOUNT      = 1206; // Max number of indexes reached
    // UDF operations:
    const ERR_UDF                 = 1300; // Generic UDF error
    const ERR_UDF_NOT_FOUND       = 1301; // UDF does not exist
    const ERR_UDF_FILE_NOT_FOUND  = 1301; // Source file for the module not found
    const ERR_LUA_FILE_NOT_FOUND  = 1301; // Source file for the module not found

    //
    // Logger
    //
    const LOG_LEVEL_OFF   = 6;
    const LOG_LEVEL_ERROR = 5;
    const LOG_LEVEL_WARN  = 4;
    const LOG_LEVEL_INFO  = 3;
    const LOG_LEVEL_DEBUG = 2;
    const LOG_LEVEL_TRACE = 1;

    //
    // Query Predicate Operators
    //
    const OP_EQ = '=';
    const OP_BETWEEN = 'BETWEEN';

    //
    // Multi-operation operators map to the C client
    //  src/include/aerospike/as_operations.h
    const OPERATOR_WRITE   = 0;
    const OPERATOR_READ    = 1;
    const OPERATOR_INCR    = 2;
    const OPERATOR_PREPEND = 4;
    const OPERATOR_APPEND  = 5;
    const OPERATOR_TOUCH   = 8;

    // UDF types
    const UDF_TYPE_LUA     = 1;

    // bin types
    const INDEX_TYPE_STRING  = 1;
    const INDEX_TYPE_INTEGER = 2;


    // lifecycle and connection methods
    public __construct ( array $config [,  boolean $persistent_connection = true [, array $options]] )
    public void __destruct ( void )
    public boolean isConnected ( void )
    public void close ( void )
    public void reconnect ( void )
    public int getNodes ( array &$metadata [, array $options ] )

    // error handling methods
    public string error ( void )
    public int errorno ( void )
    public void setLogLevel ( int $log_level )
    public void setLogHandler ( callback $log_handler )

    // key-value methods
    public array initKey ( string $ns, string $set, int|string $pk )
    public int put ( array $key, array $record [, int $ttl = 0 [, array $options ]] )
    public int get ( array $key, array &$record [, array $filter [, array $options ]] )
    public int exists ( array $key, array &$metadata [, array $options ] )
    public int touch ( array $key, int $ttl = 0 [, array $options ] )
    public int remove ( array $key [, array $options ] )
    public int removeBin ( array $key, array $bins [, array $options ] )
    public int increment ( array $key, string $bin, int $offset [, int $initial_value = 0 [, array $options ]] )
    public int append ( array $key, string $bin, string $value [, array $options ] )
    public int prepend ( array $key, string $bin, string $value [, array $options ] )
    public int operate ( array $key, array $operations [, array &$returned ] )

    // unsupported type handler methods
    public static void setSerializer ( callback $serialize_cb )
    public static void setDeserializer ( callback $unserialize_cb )

    // batch operation methods
    public int getMany ( array $keys, array &$records [, array $filter [, array $options]] )
    public int existsMany ( array $keys, array &$metadata [, array $options ] )

    // UDF methods
    public int register ( string $path, string $module [, int $language = Aerospike::UDF_TYPE_LUA] )
    public int deregister ( string $module )
    public int listRegistered ( array &$modules [, int $language ] )
    public int getRegistered ( string $module, string &$code )
    public int apply ( array $key, string $module, string $function[, array $args [, mixed &$returned ]] )
    public int aggregate ( string $module, string $function, array $args, string $ns, string $set, array $where, mixed &$value )

    // query and scan methods
    public int query ( string $ns, string $set, array $where, callback $record_cb [, array $bins [, array $options ]] )
    public int scan ( string $ns, string $set, callback $record_cb [, array $bins [, array $options ]] )
    public array predicateEquals ( string $bin, int|string $val )
    public array predicateBetween ( string $bin, int $min, int $max )

    // admin methods
    public int createIndex ( string $ns, string $set, string $bin, int $type, string $name )
    public int dropIndex ( string $ns, string $name )
}
```

### [Runtime Configuration](aerospike_config.md)
### [Lifecycle and Connection Methods](apiref_connection.md)
### [Error Handling and Logging Methods](apiref_error.md)
### [Key-Value Methods](apiref_kv.md)
### [Query and Scan Methods](apiref_streams.md)
### [User Defined Methods](apiref_udf.md) \[to be implemented\]
### [Admin Methods](apiref_admin.md) \[to be implemented\]
### [Aerospike LDT Classes](aerospike_ldt.md)

