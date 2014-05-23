---
title: PHP v3 Client API Reference
description: Complete reference for the PHP v3 Client API.
---

The PHP v3 Client API may be described as follows:

```
// AerospikeResult is returned on each Aerospike call.
// It specifies the success or failure condition of the call.
class AerospikeResult
{
    // Negative error codes are errors from the client side
    const AEROSPIKE_NO_HOSTS                =  -5;
    const AEROSPIKE_INVALID_API_PARAM       =  -4;
    const AEROSPIKE_FAIL_ASYNCQ_FULL        =  -3;
    const AEROSPIKE_FAIL_TIMEOUT            =  -2;
    const AEROSPIKE_FAIL_CLIENT             =  -1;
    const AEROSPIKE_OK                      =   0;
    const AEROSPIKE_UNKNOWN                 =   1;
    const AEROSPIKE_KEY_NOT_FOUND_ERROR     =   2;
    const AEROSPIKE_GENERATION_ERROR        =   3;
    const AEROSPIKE_PARAMETER_ERROR         =   4;
    const AEROSPIKE_KEY_FOUND_ERROR         =   5;
    const AEROSPIKE_BIN_FOUND_ERROR         =   6;
    const AEROSPIKE_CLUSTER_KEY_MISMATCH    =   7;
    const AEROSPIKE_PARTITION_OUT_OF_SPACE  =   8;
    const AEROSPIKE_SERVERSIDE_TIMEOUT      =   9;
    const AEROSPIKE_NO_XDS                  =  10;
    const AEROSPIKE_SERVER_UNAVAILABLE      =  11;
    const AEROSPIKE_INCOMPATIBLE_TYPE       =  12;
    const AEROSPIKE_RECORD_TOO_BIG          =  13;
    const AEROSPIKE_KEY_BUSY                =  14;
}

// Specifies the level of automatic retry to be performed on write.
class AerospikeWritePolicy
{
    const ONCE     =  1;
    const RETRY    =  2;
    const ASSURED  =  3;
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
   public function set_log_level($level_s);
   public function set_log_file($filename_s);
   public function set_log_callback($callback_z);

   public function init($config_z);

   public function connect($url_s, &$err_s);
   public function connect($urls_a, &$err_s);

   public function disconnect();

   public function info($host_s, $port_l, $command_s [,$timeoutms_l]);

   // KVS APIs:
   public function key_exists($key_z);
   public function key_get($key_z);
   public function key_select($key_z);
   public function key_operate($key_z);
   public function key_put($key_z, $value_z);
   public function key_batch_get($keys_z);
   public function key_remove($key_z);

   // Scan APIs:
   public function scan_create(&$scan_s, $options_s);
   public function scan_select($scan_s);
   public function scan_foreach($scan_s);
   public function scan_background($scan_s);
   public function scan_info($scan_s);
   public function scan_destroy($scan_s);

   // Secondary Index APIs:
   public function index_create(&$index_z, $type_s, $options_s);
   public function index_destroy($index_z);

   // Query APIs:
   public function query_create(&$query_s, $options_s);
   public function query_select($query_s);
   public function query_where($query_s);
   public function query_foreach($query_s);
   public function query_destroy($query_s);

   // UDF APIs:
   public function key_apply($key_z);

   // LDT APIs:
   // [TBD]

   // NB:  The Aerospike 3.0 C client does not yet provide
   //       the underlying necessary support for these SHM APIs,
   //       and so they are still subject to change.
   public function use_shm($num_nodes_l, $key_l);
   public function free_shm();
}
```
