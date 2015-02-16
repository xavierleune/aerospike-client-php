
# Aerospike::getKeyDigest

Aerospike::getKeyDigest - Helper which computes the digest that for a given key

## Description

```
public string Aerospike::getKeyDigest ( string $ns, string $set, int|string $pk )
```

**Aerospike::getKeyDigest()** will return a string digest for the given key.

## Parameters

**ns** the namespace

**set** the name of the set within the namespace

**pk** the primary key or digest value that identifies the record

## Return Values

Returns a string digest or *NULL* on failure.

## Examples

### Initializing a digest
```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$digest = $db->getKeyDigest("test", "users", 1234);
var_dump($digest);

?>
```

We expect to see:

```
string(20) "M�v2Kp���
```

