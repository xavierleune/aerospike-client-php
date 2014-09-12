
# AerospikeLList::add

AerospikeLList::add - adds an element to the LList

## Description

```
public int AerospikeLList::add ( mixed $value )
```

**AerospikeLList::add()** will add a *value* to the large ordered list type bin.
As the llist maintains sorted order, it needs to know how to compare between
elements.  For atomic type elements (integer, string) the value of the element
is used. For complex types (array) the llist expects to find a key named _key_,
which it will use for comparisons. Subsequently, llists require that all
elements in the bin should have the same type (determined by the first element
inserted).

## Parameters

**val** a string, integer, or array (with a key named _key_)

## Return Values

Returns an integer status code.  Compare to the AerospikeLDT status
constants.  When non-zero the **AerospikeLList::error()** and
**AerospikeLList::errorno()** methods can be used.

## Examples

### Example #1 AerospikeLList::add() atomic values

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$key = $db->initKey("test", "user", 1);
$llist = new AerospikeLList($db, $key, "meals");
$llist->add('2014-09-01 07:30:00 Toast');
$llist->add('2014-09-01 12:00:00 Pasta');

?>
```

### Example #1 AerospikeLList::add() complext values

```php
<?php

$key = $db->initKey("test", "user", 1);
$llist = new AerospikeLList($db, $key, "contacts");
$gary = array("key"=>"gary o'gary", "email"=>"him@garyo.org");
$llist->add($gary);
$zach = array("key"=>"zach von zach", "email"=>"zach@example.com");
$llist->add($zach);
$albert = array("key"=>"albert alberto", "email"=>"al@be.rt");
$llist->add($albert);

?>
```

