
# Aerospike::remove

Aerospike::remove - removes a record from the Aerospike database

## Description

```
public int Aerospike::remove ( array $key [, array $options ] )
```

**Aerospike::remove()** will remove a record with a given *key* from the database.

## Parameters

**key** the key for the record. An array with keys ['ns','set','key'] or ['ns','set','digest'].

**[options](aerospike.md)** including
- **Aerospike::OPT_WRITE_TIMEOUT**
- **Aerospike::OPT_POLICY_KEY**
- **Aerospike::OPT_POLICY_RETRY**
- **Aerospike::OPT_POLICY_GEN**
- **Aerospike::OPT_POLICY_COMMIT_LEVEL**

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
$status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY => Aerospike::POLICY_RETRY_NONE));
if ($status == Aerospike::OK) {
    echo "Record removed.\n";
} elseif ($status == Aerospike::ERR_RECORD_NOT_FOUND) {
    echo "A user with key ". $key['key']. " does not exist in the database\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
Record removed.
```

