
# AerospikeLDT::isLDT

AerospikeLDT::isLDT - Tests whether the bin is an LDT of the expected large data type

## Description

```
public boolean AerospikeLDT::isLDT ( void )
```

**AerospikeLDT::isLDT()** can be used to test that the bin is in fact a specific
LDT.

## Parameters

This method has no parameters.

## Return Values

Returns a **true** or **false**.

## See Also

- [AerospikeLDT::__construct()](aerospikeldt_construct.md)

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$opts = array(Aerospike::OPT_CONNECT_TIMEOUT => 750);
$db = new Aerospike($config, true, $opts);

$key = $db->initKey("test", "user", 1);
$llist = new AerospikeLList($db, $key, "timeline");
var_dump($llist->isLDT());
?>
```

We expect to see:

```
bool(true)
```

