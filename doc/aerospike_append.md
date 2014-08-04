
# Aerospike::append

Aerospike::append - appends a string to the string value in a bin

## Description

```
public int Aerospike::append ( array $key, string $bin, string $value [, array $options ] )
```

**Aerospike::append()** will append a string to the string value in *bin*.

## Parameters

**key** the key under which the bin can be found. An associative array with keys 'ns','set','key'.

**bin** the name of the bin in which we have a numeric value.

**value** the string to append to the string value in the bin.

**options** including **Aerospike::OPT_WRITE_TIMEOUT**, **Aerospike::OPT_POLICY_RETRY**.

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config, 'prod-db');
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$key = $db->initKey("test", "users", 1234);
$res = $db->append($key, 'name', ' Ph.D.');
if ($res == Aerospike::OK) {
    echo "Added the Ph.D. suffix to the user.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
Added the Ph.D. suffix to the user.
```

