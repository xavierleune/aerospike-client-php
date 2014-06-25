
# Aerospike::delete

Aerospike::delete - deletes a record from the Aerospike database

## Description

```
public int Aerospike::delete ( string $key [, int $policy ] )
```

**Aerospike::delete** will remove a *record* with a given *key* from the database.

The behavior of **Aerospike::delete** can be modified using the *policy* parameter.

## Parameters

**key** the key under which to store the record.

**policy** optionally **Aerospike::POLICY_RETRY_ONCE**

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Examples

### Example #1 Aerospike::delete()

```php
<?php

$config = array("hosts"=>array(array("name"=>"localhost", "port"=>3000));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$key = array("ns" => "test", "set" => "users", "key" => 1234);
$res = $db->delete($key);
if ($res == Aerospike::OK) {
    echo "Record deleted.\n";
elseif ($res == Aerospike::KEY_NOT_FOUND_ERROR) {
    echo "A user with key ". $key['key']. " does not exist in the database\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
Record deleted.
```

