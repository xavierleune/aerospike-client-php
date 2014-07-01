
# Aerospike::put

Aerospike::put - writes a record to the Aerospike database

## Description

```
public int Aerospike::put ( array $key, array $record [, int $ttl = 0 [, array $options ]] )
```

**Aerospike::put()** will write a *record* with a given *key*, where the _record_
is an associative array of bins and values.  The *ttl* parameter can be used to
control the expiration of the record.

By default the **Aerospike::put()** method behaves in a set-and-replace mode similar to
associative array keys and values. This behavior can be modified using the
*options* parameter.

## Parameters

**key** the key under which to store the record.

**record** the associative array of bins and values to write.

**ttl** the time-to-live in seconds for the record.

**options** including **Aerospike::OPT_WRITE_TIMEOUT**, **Aerospike::OPT_POLICY_EXISTS** and **Aerospike::OPT_POLICY_RETRY**.

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Examples

### Example #1 Aerospike::put() default behavior example

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$key = array("ns" => "test", "set" => "users", "key" => 1234);
$put_vals = array("email" => "hey@example.com", "name" => "Hey There");
// will ensure a record exists at the given key with the specified bins
$res = $db->put($key, $put_vals);
if ($res == Aerospike::OK) {
    echo "Record written.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

// Updating the record
$put_vals = array("name" => "You There", "age" => 33);
// will update the name bin, and create a new 'age' bin
$res = $db->put($key, $put_vals);
if ($res == Aerospike::OK) {
    echo "Record updated.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
Record written.
Record updated.
```

### Example #2 Fail unless the put explicitly creates a new record

```php
<?php

// This time we expect an error due to the record already existing (assuming we
// already ran Example #1)
$res = $db->put($key, $put_val, 0, array(Aerospike::OPT_POLICY_EXISTS => Aerospike::POLICY_EXISTS_CREATE)));

if ($res == Aerospike::OK) {
    echo "Record written.\n";
} elseif ($res == Aerospike::ERR_RECORD_EXISTS) {
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

