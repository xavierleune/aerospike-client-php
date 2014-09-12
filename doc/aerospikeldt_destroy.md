
# AerospikeLDT::destroy

AerospikeLDT::destroy - Tests whether the bin is an LDT of the expected large data type

## Description

```
public boolean AerospikeLDT::destroy ( void )
```

**AerospikeLDT::destroy()** can be used to test that the bin is in fact a specific
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
$llist = new AerospikeLList($db, $key, "timeline2");
$res = $llist->destroy();
if ($res == AerospikeLDT::ERR_DELETE) {
    echo "Failed to destroy the LList timeline2 on test.user PK=1\n";
}
?>
```

