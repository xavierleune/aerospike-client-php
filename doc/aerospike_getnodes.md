
# Aerospike::getNodes \[to be implemented\]

Aerospike::getNodes - get node metadata from the cluster

## Description

```
public int Aerospike::getNodes ( array &$metadata [, array $options ] )
```

**Aerospike::getNodes()** will get node related metadata from the database.
The metadata will be returned in the *metadata* variable, otherwise it will be an empty array.

## Parameters

**metadata** filled by an array of metadata.

**options** including **Aerospike::OPT_READ_TIMEOUT** and **Aerospike::OPT_POLICY_RETRY**.

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

$res = $db->getNodes($node_metadata);
if ($res == Aerospike::OK) {
    var_dump($node_metadata);
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
array(3) {
  [0]=>
  array(4) {
    ["addr"]=>
    string(12) "192.168.1.10"
    ["port"]=>
    string(4) "3000"
    ["friends"]=>
    int(2)
    ["active"]=>
    bool(true)
  }
  [1]=>
  array(4) {
    ["addr"]=>
    string(12) "192.168.1.11"
    ["port"]=>
    string(4) "3000"
    ["friends"]=>
    int(2)
    ["active"]=>
    bool(true)
  }
  [2]=>
  array(4) {
    ["addr"]=>
    string(12) "192.168.1.12"
    ["port"]=>
    string(4) "3000"
    ["friends"]=>
    int(2)
    ["active"]=>
    bool(true)
  }
}
```

