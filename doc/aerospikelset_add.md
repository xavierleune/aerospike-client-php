
# AerospikeLSet::add

AerospikeLSet::add - adds an element to the LSet

## Description

```
public int AerospikeLSet::add ( mixed $value )
```

**AerospikeLSet::add()** will add a *value* to the large set type bin.
The large set will contain only one copy of any element within it.

## Parameters

**value** a string, integer, or array

## Return Values

Returns an integer status code.  Compare to the AerospikeLDT status
constants.  When non-zero the **AerospikeLSet::error()** and
**AerospikeLSet::errorno()** methods can be used.

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$key = $db->initKey("test", "user", 1);
$lset = new AerospikeLSet($db, $key, "foods");
$lset->add('Toast');
$lset->add('Pasta');
$lset->add('Pita');
$lset->add('Pizza');

?>
```

