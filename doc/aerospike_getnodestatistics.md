
# Aerospike::getNodeStatistics \[to be implemented\]

Aerospike::getNodeStatistics - get node statistics from the cluster

## Description

```
public int Aerospike::getNodeStatistics( array &$statistics [, array $config [, array $options ]] )
```

**Aerospike::getNodeStatistics()** will get node statistics from the database.
The statistics will be returned in the *statistics* variable, otherwise it will be an empty array.

## Parameters

**statistics** filled by an associative array of node statistics.

**config** an associative array holding the cluster connection information. One
node or more (for failover) may be defined. Once a connection is established to
a node of the Aerospike DB the client will retrieve the full list of nodes in the
cluster and manage its connections to them.

- *hosts* an array of host data
  - *addr* hostname or IP of the node
  - *port*
- *user*
- *pass*

**[options](aerospike.md)** including
- **Aerospike::OPT_READ_TIMEOUT**

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Examples

```php
<?php


$config = array("hosts"=>array(array("addr"=>"192.168.1.10", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$res = $db->getNodeStatistics($stats, $config);
if ($res == Aerospike::OK) {
    var_dump($stats);
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
array(1) {
  [0]=>
  array(4) {
    ["addr"]=>
    string(12) "192.168.1.10"
    ["port"]=>
    string(4) "3000"
    ["cluster_size"]=>
    int(7)
    ["cluster_key"]=>
    string(16) "FEAC67EE16E02FBC"
    ["cluster_integrity"]=>
    bool(true)
    ["objects"]=>
    305
    ["total-bytes-disk"]=>
    int(0)
    ["used-bytes-disk"]=>
    int(0)
    ["free-pct-disk"]=>
    int(0)
    ["total-bytes-memory"]=>
    int(8589934592)
    ["used-bytes-memory"]=>
    int(130010)
    ["data-used-bytes-memory"]=>
    int(45425)
    ["index-used-bytes-memory"]=>
    int(19520)
    ["sindex-used-bytes-memory"]=>
    int(65065)
    ["free-pct-memory"]=>
    int(99)
    ["stat_read_reqs"]=>
    int(11388449)
    ["stat_read_reqs_xdr"]=>
    int(0)
    ["stat_read_success"]=>
    int(11388308)
    ["stat_read_errs_notfound"]=>
    int(141)
    ["stat_read_errs_other"]=>
    int(0)
    ["stat_write_reqs"]=>
    int(14680984)
    ["stat_write_reqs_xdr"]=>
    int(0)
    ["stat_write_success"]=>
    int(14664080)
    ["stat_write_errs"]=>
    int(813)
    ["stat_xdr_pipe_writes"]=>
    int(0)
    ["stat_xdr_pipe_miss"]=>
    int(0)
    ["stat_delete_success"]=>
    int(18133)
    ["stat_rw_timeout"]=>
    int(24)
    ["udf_read_reqs"]=>
    int(16053)
    ["udf_read_success"]=>
    int(112)
    ["udf_read_errs_other"]=>
    int(15941)
    ["udf_write_reqs"]=>
    int(267965)
    ["udf_write_success"]=>
    int(267965)
    ["udf_write_err_others"]=>
    int(0)
    ["udf_delete_reqs"]=>
    int(0)
    ["udf_delete_success"]=>
    int(0)
    ["udf_delete_err_others"]=>
    int(0)
    ["udf_lua_errs"]=>
    int(226333)
    ["udf_scan_rec_reqs"]=>
    int(509668)
    ["udf_query_rec_reqs"]=>
    int(509668)
    ["udf_replica_writes"]=>
    int(119215)
    ["stat_proxy_reqs"]=>
    int(1585)
    ["stat_proxy_reqs_xdr"]=>
    int(0)
    ["stat_proxy_success"]=>
    int(1467)
    ["stat_proxy_errs"]=>
    int(4)
    ["stat_cluster_key_trans_to_proxy_retry"]=>
    int(0)
    ["stat_cluster_key_transaction_reenqueue"]=>
    int(0)
    ....... 
        }
  }
}
```

