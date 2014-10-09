
# Aerospike::getNamespaceStatistics \[to be implemented\]

Aerospike::getNamespaceStatistics - get namespace statistics from the cluster

## Description

```
public int Aerospike::getNamespaceStatistics( array &$statistics [, array $config [, string $ns [, array $options ]]] )
```

**Aerospike::getNamespaceStatistics()** will get namespace statistics from the database.
The statistics will be returned in the *statistics* variable, otherwise it will be an empty array.

## Parameters

**statistics** filled by an associative array of namespace statistics.

**config** an associative array holding the cluster connection information. One
node or more (for failover) may be defined. Once a connection is established to
a node of the Aerospike DB the client will retrieve the full list of nodes in the
cluster and manage its connections to them.

- *hosts* an array of host data
  - *addr* hostname or IP of the node
  - *port*
- *user*
- *pass*

**ns** the namespace whose statistics is to be obtained. (default: all namespaces)

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

$res = $db->getNamespaceStatistics($ns_statistics, $config, "test");
if ($res == Aerospike::OK) {
    var_dump($ns_statistics);
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
array(1) {
  [0]=>
  array(3) {
    ["addr"]=>
    string(12) "192.168.1.10"
    ["port"]=>
    string(4) "3000"
    ["test"]=>
    array(43) {
        ["type"]=>
        string(6) "memory"
        ["objects"]=>
        int(356)
        ["master-objects"]=>
        int(183)
        ["prole-objects"]=>
        int(173)
        ["expired-objects"]=>
        int(2)
        ["evicted-objects"]=>
        int(0)
        ["set-deleted-objects"]=>
        int(0)
        ["set-evicted-objects"]=>
        int(0)
        ["used-bytes-memory"]=>
        int(136255)
        ["data-used-bytes-memory"]=>
        int(47286)
        ["index-used-bytes-memory"]=>
        int(22784)
        ["sindex-used-bytes-memory"]=>
        int(66185)
        ["free-pct-memory"]=>
        int(99)
        ["max-void-time"]=>
        int(153117081)
        ["non-expirable-objects"]=>
        int(178)
        ["current-time"]=>
        int(150535364)
        ["stop-writes"]=>
        bool(false)
        ["hwm-breached"]=>
        bool(false)
        ["available-bin-names"]=>
        int(32751)
        ["ldt_reads"]=>
        int(0)
        ["ldt_read_success"]=>
        int(0)
        ["ldt_deletes"]=>
        int(0)
        ["ldt_delete_success"]=>
        int(0)
        ["ldt_writes"]=>
        int(0)
        ["ldt_write_success"]=>
        int(0)
        ["ldt_updates"]=>
        int(0)
        ["ldt_errors"]=>
        int(0)
        ["sets-enable-xdr"]=>
        bool(false)
        ["memory-size"]=>
        int(4294967296)
        ["high-water-disk-pct"]=>
        int(50)
        ["high-water-memory-pct"]=>
        int(60)
        ["evict-tenths-pct"]=>
        int(5)
        ["stop-writes-pct"]=>
        int(90)
        ["cold-start-evict-ttl"]=>
        int(4294967295)
        ["repl-factor"]=>
        int(2)
        ["default-ttl"]=>
        int(2592000)
        ["max-ttl"]=>
        int(0)
        ["conflict-resolution-policy"]=>
        string(10) "generation"
        ["allow_versions"]=>
        bool(false)
        ["single-bin"]=>
        bool(false)
        ["enable-xdr"]=>
        bool(false)
        ["disallow-null-setname"]=>
        bool(false)
        ["total-bytes-memory"]=>
        int(4294967296)
    }
  }
}
```

