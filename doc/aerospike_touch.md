
# Aerospike::touch

Aerospike::touch - touch a record in the Aerospike DB

## Description

```
public int Aerospike::touch ( array $key, int $ttl = 0 [, array $options ] )
```

**Aerospike::touch()** will touch the given record, resetting its time-to-live
and incrementing its generation.

## Parameters

**key** the key for the record. An associative array with keys 'ns','set','key'.

**ttl** the time-to-live in seconds for the record.

**options** including **Aerospike::OPT_WRITE_TIMEOUT** and **Aerospike::OPT_POLICY_RETRY**.

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$key = $db->initKey("test", "users", 1234);
$res = $db->touch($key, 120);
if ($res == Aerospike::OK) {
    echo "Added 120 seconds to the record's expiration.\n"
} elseif ($res == Aerospike::ERR_RECORD_NOT_FOUND) {
    echo "A user with key ". $key['key']. " does not exist in the database\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
Added 120 seconds to the record's expiration.
```
**or**
```
A user with key 1234 does not exist in the database.
```

