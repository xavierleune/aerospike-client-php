---
title: Aerospike PHP Client API Reference
description: Complete reference for the Aerospike PHP Client API.
---

The Aerospike PHP client API may be described as follows:

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
    // By default writes will try to create and replace records and bins
    // behaving similar to an associative array in PHP.
    // The following policy flags allow for the default behavior to be
    // modified.  The flags can be combined with bitwise operators.

    const POLICY_RETRY_ONCE = 1; // allow for a single retry on an operation
    const POLICY_NO_KEY_COLLISION = 2; // fail on writes where the key exists
    const POLICY_NO_BIN_COLLISION = 4; // fail on writes where a bin exists

    //
    // Aerospike Status Codes:
    //
    // Each Aerospike API method invocation returns a status code
    //  depending upon the success or failure condition of the call.
    //
    // [Note:  Negative status values come from the client;
    //         positive status values come from the server.]
    //

    // Client status codes:

    const COMMAND_REJECTED        =  -8;
    const QUERY_TERMINATED        =  -7;
    const SCAN_TERMINATED         =  -6;
    const NO_HOSTS                =  -5;
    const INVALID_API_PARAM       =  -4;
    const FAIL_ASYNCQ_FULL        =  -3;
    const FAIL_TIMEOUT            =  -2;
    const FAIL_CLIENT             =  -1;

    // Server status codes:

    const OK                      =   0;
    const SERVER_ERROR            =   1;
    const KEY_NOT_FOUND_ERROR     =   2;
    const GENERATION_ERROR        =   3;
    const PARAMETER_ERROR         =   4;
    const KEY_FOUND_ERROR         =   5;
    const BIN_FOUND_ERROR         =   6;
    const CLUSTER_KEY_MISMATCH    =   7;
    const PARTITION_OUT_OF_SPACE  =   8;
    const SERVERSIDE_TIMEOUT      =   9;
    const NO_XDR                  =  10;
    const SERVER_UNAVAILABLE      =  11;
    const INCOMPATIBLE_TYPE       =  12;
    const RECORD_TOO_BIG          =  13;
    const KEY_BUSY                =  14;
    const SCAN_ABORT              =  15;
    const UNSUPPORTED_FEATURE     =  16;
    const BIN_NOT_FOUND           =  17;
    const DEVICE_OVERLOAD         =  18;
    const KEY_MISMATCH            =  19;

    // UDF status codes:

    const UDF_BAD_RESPONSE        = 100;

    // Secondary Index status codes:

    const INDEX_FOUND             = 200;
    const INDEX_NOTFOUND          = 201;
    const INDEX_OOM               = 202;
    const INDEX_NOTREADABLE       = 203;
    const INDEX_GENERIC           = 204;
    const INDEX_NAME_MAXLEN       = 205;
    const INDEX_MAXCOUNT          = 206;

    // Query statue codes:

    const QUERY_ABORTED           = 210;
    const QUERY_QUEUEFULL         = 211;
    const QUERY_TIMEOUT           = 212;
    const QUERY_GENERIC           = 213;

    //
    // Aerospike API Methods:
    //

    // Client Object APIs:

    public function __construct($config_z);
    public function __destruct();

    // Cluster Management APIs:

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
    public function delete($key_z [, $policy_z]);
    public function exists($key_z [, $policy_z]);
    public function getMany($key_z_a, &$value_z_a [, $policy_z]);
    public function getHeader($key_z [, $policy_z]);
    public function getHeaderMany($key_z_a [, $policy_z]);
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
