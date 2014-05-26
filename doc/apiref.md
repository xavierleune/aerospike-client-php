---
title: Aerospike PHP Client API Reference
description: Complete reference for the Aerospike PHP Client API.
---

The Aerospike PHP Client API may be described as follows:

```
// AerospikeResult is returned on each Aerospike call.
// It specifies the success or failure condition of the call.
// [Note:  Negative status values come from the client;
//         positive status values come from the server.]
class AerospikeResult
{
    // Client status codes:

    const AEROSPIKE_COMMAND_REJECTED        =  -8;
    const AEROSPIKE_QUERY_TERMINATED        =  -7;
    const AEROSPIKE_SCAN_TERMINATED         =  -6;
    const AEROSPIKE_NO_HOSTS                =  -5;
    const AEROSPIKE_INVALID_API_PARAM       =  -4;
    const AEROSPIKE_FAIL_ASYNCQ_FULL        =  -3;
    const AEROSPIKE_FAIL_TIMEOUT            =  -2;
    const AEROSPIKE_FAIL_CLIENT             =  -1;

    // Server status codes:

    const AEROSPIKE_OK                      =   0;
    const AEROSPIKE_SEVRVER_ERROR           =   1;
    const AEROSPIKE_KEY_NOT_FOUND_ERROR     =   2;
    const AEROSPIKE_GENERATION_ERROR        =   3;
    const AEROSPIKE_PARAMETER_ERROR         =   4;
    const AEROSPIKE_KEY_FOUND_ERROR         =   5;
    const AEROSPIKE_BIN_FOUND_ERROR         =   6;
    const AEROSPIKE_CLUSTER_KEY_MISMATCH    =   7;
    const AEROSPIKE_PARTITION_OUT_OF_SPACE  =   8;
    const AEROSPIKE_SERVERSIDE_TIMEOUT      =   9;
    const AEROSPIKE_NO_XDR                  =  10;
    const AEROSPIKE_SERVER_UNAVAILABLE      =  11;
    const AEROSPIKE_INCOMPATIBLE_TYPE       =  12;
    const AEROSPIKE_RECORD_TOO_BIG          =  13;
    const AEROSPIKE_KEY_BUSY                =  14;
    const AEROSPIKE_SCAN_ABORT              =  15;
    const AEROSPIKE_UNSUPPORTED_FEATURE     =  16;
    const AEROSPIKE_BIN_NOT_FOUND           =  17;
    const AEROSPIKE_DEVICE_OVERLOAD         =  18;
    const AEROSPIKE_KEY_MISMATCH            =  19;

    // UDF status codes:

    const AEROSPIKE_UDF_BAD_RESPONSE        = 100;

    // Secondary Index status codes:

    const AEROSPIKE_INDEX_FOUND             = 200;
    const AEROSPIKE_INDEX_NOTFOUND          = 201;
    const AEROSPIKE_INDEX_OOM               = 202;
    const AEROSPIKE_INDEX_NOTREADABLE       = 203;
    const AEROSPIKE_INDEX_GENERIC           = 204;
    const AEROSPIKE_INDEX_NAME_MAXLEN       = 205;
    const AEROSPIKE_INDEX_MAXCOUNT          = 206;

    // Query statue codes:

    const AEROSPIKE_QUERY_ABORTED           = 210;
    const AEROSPIKE_QUERY_QUEUEFULL         = 211;
    const AEROSPIKE_QUERY_TIMEOUT           = 212;
    const AEROSPIKE_QUERY_GENERIC           = 213;
}

// Specifies the level of automatic retry to be performed on write.
class AerospikeWritePolicy
{
    const ONCE     =  1;
    const RETRY    =  2;
}

// Specifies the uniqueness constraint to be applied on write.
class AerospikeWriteUniqueFlag
{
    const NONE                       =  0;
    const WRITE_ONLY_KEY_NOT_EXIST   =  1;
    const WRITE_ONLY_BINS_NOT_EXIST  =  2;
}

// Signature for custom log callback functions:
// function log_callback($level_s, $function_s, $filename_s, $line_s, $format_s, ...);

//  Signature for scan callback functions:
// function scan_foreach_callback($val_z, $udata_z);

// Signature for query callback functions:
// function query_foreach_callback($val_z, $udata_z);

// Client interface to the Aerospke cluster.
class AerospikeClient
{
   // Client Object APIs:

   public function __construct($config_z);
   public function __destruct($config_z);

   // Cluster Management APIs:

   public function connect($hosts_a, &$err_s);
   public function isConnected();
   public function close();
   public function getNodes();
   public function info($host_s, $port_l, $command_s [,$timeoutms_l]);

   // Key Value Store (KVS) APIs:

   public function add($key_z [, $policy_z]);
   public function append($key_z [, $policy_z]);
   public function delete($key_z [, $policy_z]);
   public function exists($key_z [, $policy_z]);
   public function get($key_z [, $policy_z]);
   public function getHeader($key_z [, $policy_z]);
   public function operate($key_z [, $policy_z]);
   public function prepend($key_z [, $policy_z]);
   public function put($key_z, $value_z [, $policy_z]);
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
   public function register($client_path_s, $server_path_s, $langage);

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
