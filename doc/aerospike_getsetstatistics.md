
# Aerospike::getSetStatistics \[to be implemented\]

Aerospike::getSetStatistics - get set statistics from the cluster

## Description

```
public int Aerospike::getSetStatistics( array &$statistics [, array $config [, string $ns [, string $set [, array $options ]]]] )
```

**Aerospike::getSetStatistics()** will get set statistics from the database.
The statistics will be returned in the *statistics* variable, otherwise it will be an empty array.

## Parameters

**statistics** filled by an associative array of set statistics.

**config** an associative array holding the cluster connection information. One
node or more (for failover) may be defined. Once a connection is established to
a node of the Aerospike DB the client will retrieve the full list of nodes in the
cluster and manage its connections to them.

- *hosts* an array of host data
  - *addr* hostname or IP of the node
  - *port*
- *user*
- *pass*

**ns** the namespace for which statistics of sets is to be obtained. (default: all namespaces)

**set** the set for which statistics are to be obtained. (default: all sets)

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

$res = $db->getSetStatistics($set_stats, $config, "test", "demo");
if ($res == Aerospike::OK) {
    var_dump($set_stats);
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
    ["test"]=>
    array(1) {
        ["demo"]=>
        array(5) {
            ["n_objects"]=>
            int(281)
            ["set-stop-write-count"]=>
            int(0)
            ["set-evict-hwm-count"]=>
            int(0)
            ["set-enable-xdr"]=>
            string(11) "use-default"
            ["set-delete"]=>
            bool(false)
        }
  }
}
```

