
# AerospikeLDT::size

AerospikeLDT::size - get the number of elements in the LDT

## Description

```
public int AerospikeLDT::size ( int &$num_elements )
```

**AerospikeLDT::size()** will fill *num_elements* with the number of elements
in the LDT.

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
$res = $llist->size($num);
if ($res == AerospikeLDT::OK) {
    echo "There are $num history elements for the vehicle\n";
}
?>
```

We expect to see:

```
There are 64 history elements for the vehicle
```


