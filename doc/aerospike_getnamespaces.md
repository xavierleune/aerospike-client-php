
# Aerospike::getNamespaces \[to be implemented\]

Aerospike::getNamespaces - get namespaces from the cluster

## Description

```
public int Aerospike::getNamespaces( array &$namespaces [, array $options ] )
```

**Aerospike::getNamespaces()** will get namespaces from the database.
The namespace names will be returned in the *namespaces* variable, otherwise it will be an empty array.

## Parameters

**namespaces** filled by an array of namespace names.

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

$res = $db->getNamespaces($namespaces);
if ($res == Aerospike::OK) {
    var_dump($namespaces);
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
array(2) {
  [0]=>
  "test"
  [1]=>
  "bar"
}
```

