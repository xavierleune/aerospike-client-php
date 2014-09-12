
# AerospikeLDT::errorno

AerospikeLDT::errorno - display an error code associated with the last operation

## Description

```
public int AerospikeLDT::errorno ( void )
```

**AerospikeLDT::errorno()** will return an error code associated with the last
operation. If the operation was successful the return value should be 0
(**AerospikeLDT::OK**).

This is a convenience method that actually wraps around Aerospike::errorno().

## Return Values

Returns an error code, which may be 0 (**AerospikeLDT::OK**) on success.

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);

$key = $db->initKey("test", "user", 1);
$llist = new AerospikeLList($db, $key, "honorificabilitudinitatibus");
$res = $llist->size($num);
if ($res == AerospikeLDT::ERR_BIN_NAME_TOO_LONG) {
    echo "Oops, the bin name you've chosen is too long [{$llist->errorno()}]\n";
}
?>
```

On error we expect to see:

```
Oops, the bin name you've chosen is too long [1413]
```

