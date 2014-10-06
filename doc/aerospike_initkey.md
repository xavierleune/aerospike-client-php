
# Aerospike::initKey

Aerospike::initKey - helper method for building the key array

## Description

```
public array Aerospike::initKey ( string $ns, string $set, int|string $pk, [
        boolean $digest = false ] )
```

**Aerospike::initKey()** will return an array that can be passed as the
*$key* arguement in [key-value methods](apiref_kv.md).

## Parameters

**ns** the namespace

**set** the name of the set within the namespace

**pk** the primary key value or digest value (depending on parameter 4 $digest) that identifies the record

## Return Values

Returns an array with the following structure:
```
Associative Array:
  ns => string namespace
  set => string set name
  key => the record's primary key value
```
or
```
Associative Array:
  ns => string namespace
  set => string set name
  digest => a RIPEMD-160 hash of the key
```
or *NULL* on failure.

## Examples

### Example #1 Aerospike::initKey() default behavior example returning primary
key

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$key = $db->initKey("test", "users", 1234);
var_dump($key);

?>
```

We expect to see:

```
array(3) {
  ["ns"]=>
  string(4) "test"
  ["set"]=>
  string(5) "users"
  ["key"]=>
  int(1234)
}
```
### Example #2 Aerospike::initKey() example returning digest

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$key = $db->initKey("test", "users", "abc1234", true);
var_dump($key);

?>
```

We expect to see:

```
array(3) {
  ["ns"]=>
  string(4) "test"
  ["set"]=>
  string(5) "users"
  ["digest"]=>
  string(7) "abc1234"
}
```
