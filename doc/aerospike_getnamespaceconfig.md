
# Aerospike::getNamespaceConfig \[to be implemented\]

Aerospike::getNamespaceConfig - get namespace config from the cluster

## Description

```
public int Aerospike::getNamespaceConfig( array &$ns_config [, array $config [, string $ns [, array $options ]]])
```

**Aerospike::getNamespaceConfig()** will get namespace configuration from the database.
The configuration will be returned in the *ns_config* variable, otherwise it will be an empty array.

## Parameters

**ns_config** filled by an associative array of namespace configuration.

**config** an associative array holding the cluster connection information. One
node or more (for failover) may be defined. Once a connection is established to
a node of the Aerospike DB the client will retrieve the full list of nodes in the
cluster and manage its connections to them.

- *hosts* an array of host data
  - *addr* hostname or IP of the node
  - *port*
- *user*
- *pass*

**ns** namespace name whose configuration is to be obtained. (default: all
        namespaces)

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

$res = $db->getNamespaceConfig($ns_conf, $config, "test");
if ($res == Aerospike::OK) {
    var_dump($ns_conf);
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
        ["sets-enable-xdr"]=>
        bool(true)
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

