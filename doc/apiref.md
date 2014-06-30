---
title: Aerospike PHP Client API Reference
description: Complete reference for the Aerospike PHP Client API.
---

The Aerospike PHP client API may be described as follows:

### [Aerospike Class](aerospike.md)
### [Error Handling Methods](apiref_error.md)
### [Key-Value Methods](apiref_kv.md)

```

// Signature for custom log callback functions:
// function log_callback($level_s, $function_s, $filename_s, $line_s, $format_s, ...);

//  Signature for scan callback functions:
// function scan_foreach_callback($val_z, $udata_z);

// Signature for query callback functions:
// function query_foreach_callback($val_z, $udata_z);

// Client interface to the Aerospike cluster.
class Aerospike
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
    // OPT_POLICY_EXISTS with one of these values will overwrite this
    const POLICY_EXISTS_IGNORE = 1; // write record regardless of existence
    const POLICY_EXISTS_CREATE = 2; // create a record ONLY if it DOES NOT exist
    const POLICY_EXISTS_UPDATE = 3; // update a record ONLY if it exists
    const POLICY_EXISTS_REPLACE = 4; // replace a record ONLY if it exists
    const POLICY_EXISTS_CREATE_OR_REPLACE = 5; // default behavior

    //
    // Options can be assigned values that modify default behavior
    //
    const OPT_CONNECT_TIMEOUT = 1; // value in milliseconds, default: 1000
    const OPT_READ_TIMEOUT = 2; // value in milliseconds, default: 1000
    const OPT_WRITE_TIMEOUT = 3; // value in milliseconds, default: 1000
    const OPT_POLICY_RETRY = 4; // set to a Aerospike::POLICY_RETRY_* value
    const OPT_POLICY_EXISTS = 5; // set to a Aerospike::POLICY_EXISTS_* value

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


    //
    // Aerospike API Methods:
    //

    // Client Object APIs:

    public function __construct($config_z);
    public function __destruct();

    // Cluster Management APIs:

    public function reconnect();
    public function isConnected();
    public function close();
    public function getNodes();
    public function info($host_s, $port_l, $command_s [,$timeoutms_l]);

    // Errors
    // When an error occurs on any operation store the error code and string
    // in the object where the following methods can access it.
    // If no error occured both methods should return a '' and 0 respectively
    public function error(); // the string error message or an empty string
    public function errorno(); // the integer error code or 0

    // Key Value Store (KVS) APIs:

    public function put($key_z, $value_z [, $policy_z]);
    public function get($key_z, &$value_z, [$value_z [, $policy_z]]);

    public function add($key_z, $value_z [, $policy_z]);
    public function append($key_z, $value_z [, $policy_z]);
    public function remove($key_z [, $policy_z]);
    public function exists($key_z [, $policy_z]);
    public function getMany($key_z_a, &$value_z_a [, $policy_z]);
    public function operate($key_z [, $policy_z]);
    public function prepend($key_z, $value_z [, $policy_z]);
    public function touch($key_z [, $policy_z]);

    // Scan APIs:

    public function scan_create(&$scan_s, $options_s);
    public function scanAll($scan_s);
    public function scanNode($scan_s);
    public function scan_background($scan_s);
    public function scan_info($scan_s);
    public function scan_destroy($scan_s);

    // Secondary Index APIs:

    public function createIndex(&$index_z, $type_s, $options_s);
    public function dropIndex($index_z);

    // Query APIs:

    public function query_create(&$query_s, $options_s);
    public function query($query_s);
    public function queryAggregate($query_s);
    public function query_foreach($query_s);
    public function query_destroy($query_s);

    // User Defined Function (UDF) APIs:

    public function execute($key_z);
    public function register($client_path_s, $server_path_s, $language);

    // Large Data Type (LDT) APIs:

    public function getLargeList($key_z);
    public function getLargeMap($key_z);
    public function getLargeSet($key_z);
    public function getLargeStack($key_z);

    // Logging APIs:

    public function setLogLevel($level_s);
    public function setLogFile($filename_s);
    public function setLogCallback($callback_z);

    // Shared Memory APIs:

    // NB:  The Aerospike 3.0 C client does not yet provide
    //       the underlying necessary support for these SHM APIs,
    //       and so they are still subject to change.
    public function useShm($num_nodes_l, $key_l);
    public function freeShm();
}
```
