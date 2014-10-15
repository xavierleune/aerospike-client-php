
# Aerospike::put

Aerospike::put - writes a record to the Aerospike database

## Description

```
public int Aerospike::put ( array $key, array $bins [, int $ttl = 0 [, array $options ]] )
```

**Aerospike::put()** will write a record with a given *key* with *bins*
described as an array of bin-name => value pairs.
The *ttl* parameter can be used to control the expiration of the record.

By default the **Aerospike::put()** method behaves in a set-and-replace mode similar to
array keys and values. This behavior can be modified using the
*options* parameter.

## Parameters

**key** the key under which to store the record. An array with keys ['ns','set','key'] or ['ns','set','digest'].

**bins** the array of bin names and values to write.

**ttl** the [time-to-live](http://www.aerospike.com/docs/client/c/usage/kvs/write.html#change-record-time-to-live-ttl) in seconds for the record.

**options** including
- **Aerospike::OPT_POLICY_KEY**
- **Aerospike::OPT_WRITE_TIMEOUT**
- **Aerospike::OPT_POLICY_EXISTS**
- **Aerospike::OPT_POLICY_RETRY**
- **Aerospike::OPT_SERIALIZER**.
- **Aerospike::OPT_POLICY_GEN**

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Examples

### Example #1 Aerospike::put() default behavior example

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$key = $db->initKey("test", "users", 1234);
$bins = array("email" => "hey@example.com", "name" => "Hey There");
// will ensure a record exists at the given key with the specified bins
$res = $db->put($key, $bins);
if ($res == Aerospike::OK) {
    echo "Record written.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

// Updating the record
$bins = array("name" => "You There", "age" => 33);
// will update the name bin, and create a new 'age' bin
$res = $db->put($key, $bins);
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
$res = $db->put($key, $bins, 0, array(Aerospike::OPT_POLICY_EXISTS => Aerospike::POLICY_EXISTS_CREATE)));

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


### Example #3 Fail if the record has been written recently

```php
<?php

// Get the record metadata and note its generation
$db->exists($key, $metadata);
$gen = $metadata['generation'];
$gen_policy = array(Aerospike::POLICY_GEN_EQ, $gen);
$res = $db->put($key, $bins, 0, array(Aerospike::OPT_POLICY_GEN => $gen_policy));

if ($res == Aerospike::OK) {
    echo "Record written.\n";
} elseif ($res == Aerospike::ERR_RECORD_GENERATION) {
    echo "The record has been written since we last read it.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}
?>
```

We expect to see:

```
The Aerospike server already has a record with the given key.
```

