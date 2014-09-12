
# AerospikeLDT::setCapacity

AerospikeLDT::setCapacity - set the number of elements the LDT can hold

## Description

```
public int AerospikeLDT::setCapacity ( int $num_elements )
```

**AerospikeLDT::setCapacity()** will set *num_elements* to be the max number
of elements the LDT can contain.

## Parameters

**num_elements** integer for the max number of elements the LDT should hold.

## Return Values

Returns an integer status code.  Compare to the AerospikeLDT status code class
constants.  When non-zero the **AerospikeLDT::error()** and
**AerospikeLDT::errorno()** methods can be used.

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);

$key = $db->initKey("rental", "vehicle", "4BPR005");
$llist = new AerospikeLList($db, $key, "history");
$res = $llist->setCapacity(400);
if ($res == AerospikeLDT::OK) {
    echo "Set the capacity of history elements for vehicle 4BPR005 to 400";
}
?>
```

We expect to see:

```
Set the capacity of history elements for vehicle 4BPR005 to 400
```


