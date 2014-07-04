
# Aerospike::halt

Aerospike::halt - Gets the client to drop a stream of records from the server.

## Description

```
public void Aerospike::halt ( void )
```

**Aerospike::halt()** will halt results streaming back from a query or scan, and
can be called inside the record callback context of either.

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$processed = 0;
$res = $db->scan('test.users', function ($record) {
    var_dump($record);
    if ($processed++ > 5) {
        $db->halt();
    }
});
if ($res == Aerospike::ERR_SCAN) {
    echo "An error occured while scanning[{$db->errorno()}] {$db->error()}\n";
} else if ($res == Aerospike::ERR_SCAN_ABORTED) {
    echo "The results streaming from the Aerospike DB were stopped after $processed records\n";
}

?>
```

We expect to see:

```
foo@example.com
:
bar@example.com
I think a sample of 20 records is enough
```

