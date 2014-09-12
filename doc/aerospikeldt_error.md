
# AerospikeLDT::error

AerospikeLDT::error - display an error message associated with the last operation

## Description

```
public string AerospikeLDT::error ( void )
```

**AerospikeLDT::error()** will return an error message associated with the last
operation. This is a convenience method that actually wraps around
Aerospike::error().

## Return Values

Returns an error string, which may be empty on success.

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

