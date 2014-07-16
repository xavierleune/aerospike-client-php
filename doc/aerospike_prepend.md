
# Aerospike::prepend

Aerospike::prepend - prepends a string to the string value in a bin

## Description

```
public int Aerospike::prepend ( array $key, string $bin, string $value [, array $options ] )
```

**Aerospike::prepend()** will prepend a string to the string value in *bin*.

## Parameters

**key** the key under which the bin can be found. An associative array with keys 'ns','set','key'.

**bin** the name of the bin in which we have a numeric value.

**value** the string to prepend to the string value in the bin.

**options** including **Aerospike::OPT_WRITE_TIMEOUT**, **Aerospike::OPT_POLICY_RETRY**, **Aerospike::OPT_SERIALIZER**

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

$key = $db->initKey("test", "users", 1234);
$res = $db->prepend($key, 'name', '*');
if ($res == Aerospike::OK) {
    echo "Starred the user.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
Starred the user.
```

