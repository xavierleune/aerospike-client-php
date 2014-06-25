
# The Aerospike class
The Aerospike PHP client API may be described as follows:

## Introduction

The main Aerospike class

```

class Aerospike
{
    //
    // Policy flags:
    // By default writes will try to create and replace records and bins
    // behaving similar to an associative array in PHP.
    // The following policy flags allow for the default behavior to be
    // modified.  The flags can be combined with bitwise operators.
    //
    const POLICY_RETRY_ONCE = 1; // allow for a single retry on an operation
    const POLICY_NO_KEY_COLLISION = 2; // fail on writes where the key exists
    const POLICY_NO_BIN_COLLISION = 4; // fail on writes where a bin exists

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


    // error handling methods
    public string Aerospike::error ( void )
    public int Aerospike::errorno ( void )
    // key-value methods
    public int Aerospike::put ( string $key, array $record [, int $ttl = 0 [, int $policy ]] )
    public int Aerospike::get ( string $key, array &$record [, array $filter [, int $policy ]] )
    public int Aerospike::exists ( string $key, array &$metadata [, int $policy ] )
    public int Aerospike::delete ( string $key [, int $policy ] )
}
```

### [Error Handling Methods](apiref_error.md)
### [Key-Value Methods](apiref_kv.md)

