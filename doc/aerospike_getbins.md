
# Aerospike::getBins \[to be implemented\]

Aerospike::getBins - get bins from the cluster

## Description

```
public int Aerospike::getBins( array &$metadata [, array $config [, string $ns [, array $options ]]] )
```

**Aerospike::getBins()** will get bins related metadata from the database.
The metadata will be returned in the *metadata* variable, otherwise it will be an empty array.

## Parameters

**metadata** filled by an associative array of metadata of bins.

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

$res = $db->getBins($bins_metadata);
if ($res == Aerospike::OK) {
    var_dump($bins_metadata, $config);
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
    array(3) {
        ["num-bin-names"]=>
        int(3)
        ["bin-names-quota"]=>
        int(32768)
        ["bin-names"]=>
        array(3) {
            [0]=>
            "age"
            [1]=>
            "email"
            [2]=>
            "name"
        }
    }
    ["bar"]=>
    array(3) {
        ["num-bin-names"]=>
        int(3)
        ["bin-names-quota"]=>
        int(32768)
        ["bin-names"]=>
        array(3) {
            [0]=>
            "age"
            [1]=>
            "email"
            [2]=>
            "name"
        }
    }
  }
}
```

