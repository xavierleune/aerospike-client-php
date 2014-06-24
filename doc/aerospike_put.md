
# Aerospike::put

Aerospike::put - writes a record to the Aerospike database

## Description

```
public int Aerospike::put ( string $key, array $record [, int $ttl = 0 [, int $policy ]] )
```

**Aerospike::put** will write a *record* with a given *key*, where the _record_
is an associative array of bins and values.  The *ttl* parameter can be used to
control the expiration of the record.

By default the **put** method behaves in a set-and-replace mode similar to
associative array keys and values. This behavior can be modified using the
*policy* parameter.

## Parameters

**key** the key under which to store the record.

**record** the associative array of bins and values to write.

**ttl** the time-to-live in seconds for the record.

**policy** any bitwise combined value of **Aerospike::POLICY_RETRY_ONCE**, **Aerospike::POLICY_NO_KEY_COLLISION**, **Aerospike::POLICY_NO_BIN_COLLISION**

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Examples

### Example #1 Aerospike::put() default behavior example

```php
<?php

$config = array("hosts" => array(array("name" => "localhost", "port" => 3000));

try {
   $db = new Aerospike($config);
} catch (AerospikeException $e) {
   echo "Aerospike client creation failed: " . $e->getMessage() . "\n";
   exit(1);
}

$key = array("ns" => "test", "set" => "users", "key" => 1234);
$put_val = array("email" => "hey@example.com", "name" => "Hey There");
// will ensure a record exists at the given key with the specified bins
$res = $db->put($key, $put_val);
if ($res == Aerospike::OK) {
    echo "Record written.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
Record written.
```

### Example #2 Fail unless the put explicitly creates a new record

```php
<?php

// This time we expect an error due to the record already existing (assuming we
// already ran Example #1)
$res = $db->put($key, $put_val, 0, Aerospike::POLICY_RETRY_ONCE & Aerospike::POLICY_NO_KEY_COLLISION);

if ($res == Aerospike::OK) {
    echo "Record written.\n";
} elseif ($res == Aerospike::KEY_FOUND_ERROR) {
    echo "The Aerospike server already has a record with the given key.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}
?>
```

We expect to see:

```
The Aerospike server already has a record with the given key.
```

