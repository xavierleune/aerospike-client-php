
# Aerospike::getNodeInfo \[to be implemented\]

Aerospike::getNodeInfo - get node info from the cluster

## Description

```
public int Aerospike::getNodeInfo ( array &$info [, array $config [, array $options ]] )
```

**Aerospike::getNodeInfo()** will get node related information from the database.
The info will be returned in the *info* variable, otherwise it will be an empty array.

## Parameters

**info** filled by an associative array of node info.

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

$res = $db->getNodeInfo($node_info, $config);
if ($res == Aerospike::OK) {
    var_dump($node_info);
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
    ["status"]=>
    string(2) "ok"
    ["build"]=>
    string(6) "3.3.17"
    ["services"]=>
    array(3)=> {
        [0]=>
        string(17) "192.168.1.11:3000"
        [1]=>
        string(17) "192.168.1.13:3000"
        [2]=>
        string(17) "192.168.1.14:3000"
    }
  }
}
```

