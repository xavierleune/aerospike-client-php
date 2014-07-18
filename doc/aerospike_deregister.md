
# Aerospike::deregister

Aerospike::deregister - removes a UDF module from the Aerospike DB

## Description

```
public int Aerospike::deregister ( string $module )
```

**Aerospike::deregister()** will remove a UDF module named *module* from the
Aerospike DB.

## Parameters

**module** the name of the UDF module to remove from the Aerospike DB.

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000));
$db = new Aerospike($config, 'prod-db');
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$res = $db->deregister('my_udf');
if ($res == Aerospike::OK) {
    echo "UDF module my_udf was removed from the Aerospike DB.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
UDF module my_udf was removed from the Aerospike DB.
```

