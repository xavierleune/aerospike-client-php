
# Aerospike::getNodeConfig \[to be implemented\]

Aerospike::getNodeConfig - get node config from the cluster

## Description

```
public int Aerospike::getNodeConfig( array &$node_config [, array $config [, array $options ]] )
```

**Aerospike::getNodeConfig()** will get node config from the database.
The configuration will be returned in the *node_config* variable, otherwise it will be an empty array.

## Parameters

**node_config** filled by an associative array of node configuration.

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

$res = $db->getNodeConfig($node_conf, $config);
if ($res == Aerospike::OK) {
    var_dump($node_conf);
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
    ["transaction-queues"]=>
    int(4)
    ["transaction-threads-per-queue"]=>
    int(4)
    ["transaction-duplicate-threads"]=>
    int(0)
    ["transaction-pending-limit"]=>
    int(20)
    ["migrate-threads"]=>
    int(1)
    ["migrate-priority"]=>
    int(40)
    ["migrate-xmit-priority"]=>
    int(40)
    ["migrate-xmit-sleep"]=>
    int(500)
    ["migrate-read-priority"]=>
    int(10)
    ....... 
        }
  }
}
```

