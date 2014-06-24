
# Aerospike::exists / Aerospike::getMetadata

Aerospike::exists - check if a record exists in the Aerospike database

## Description

```
public int Aerospike::exists ( string $key, array &$metadata [, int $policy ] )

is an alias for

public int Aerospike::getMetadata ( string $key, array &$metadata [, int $policy ] )
```

**Aerospike::exists** will check if a record with a given *key* exists in the database.
If such a key exists its metadata will be returned in the *metadata* variable,
otherwise it will be an empty array.

The behavior of **Aerospike::exists** can be modified using the *policy* parameter.

## Parameters

**key** the key for the record.

**metadata** filled by an associative array of metadata.

**policy** optionally **Aerospike::POLICY_RETRY_ONCE**

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Examples

### Example #1 Aerospike::exists()

```php
<?php

$config = array("hosts" => array(array("name" => "localhost", "port" => 3000));

try {
   $db = new Aerospike($config);
} catch (AerospikeException $e) {
   echo "Aerospike client creation failed: " . $e->getMessage() . "\n";
   exit(1);
}

$key = array("ns" => "test", "set" => "users", "key" => "1234");
$res = $db->exists($key, $metadata);
if ($res == Aerospike::OK) {
    var_dump($metadata);
elseif ($res == Aerospike::KEY_NOT_FOUND_ERROR) {
    echo "A user with key ". $key['key']. " does not exist in the database\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
array(2) {
  ["generation"]=>
  int(4)
  ["ttl"]=>
  int(1337)
}
```
**or**
```
A user with key 1234 does not exist in the database.
```

