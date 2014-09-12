
# AerospikeLDT::getCapacity

AerospikeLDT::getCapacity - get the number of elements the LDT can hold

## Description

```
public int AerospikeLDT::getCapacity ( int &$num_elements )
```

**AerospikeLDT::getCapacity()** will fill *num_elements* with the estimated max
number of elements the LDT can contain.

## Parameters

**num_elements** filled by an integer value.

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
$res = $llist->getCapacity($num);
if ($res == AerospikeLDT::OK) {
    echo "The number of history elements this vehicle can have is $num\n";
}
?>
```

We expect to see:

```
The number of history elements this vehicle can have is 8000
```


