
# Aerospike::scanInfo

Aerospike::scanInfo - returns the status of background scan triggered in the Aerospike database

## Description

```
public int Aerospike::scanInfo ( integer $scan_id, array &$info [, array $options ] )
```

**Aerospike::scanInfo()** will return the status of a background scan identified
by *scan_id* which was triggered using **Aerospike::scanBackground()** in
*info*.

## Parameters

**scan_id** the scan id

**info** the status of the background scan returned as an associative array conforming to the following:
```
Associative Array:
  progress_pct => percentage progress
  records_scanned => no. of records scanned
  status => one of Aerospike::SCAN_STATUS_UNDEF, Aerospike::SCAN_STATUS_INPROGRESS,
  Aerospike::SCAN_STATUS_ABORTED, Aerospike::SCAN_STATUS_COMPLETED
```

**options** including **Aerospike::OPT_WRITE_TIMEOUT** and **Aerospike::OPT_READ_TIMEOUT**.

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## See Also

- [Aerospike::scanBackground()](aerospike_scanbackground.md)

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$res = $db->scanInfo(1, $info);
if ($res == Aerospike::OK) {
    var_dump($info);
    if ($info["status"] == Aerospike::SCAN_STATUS_COMPLETED) {
        echo "Background scan is complete!";
    }
} else {
    echo "An error occured while retrieving info of scan [{$db->errorno()}] {$db->error()}\n";
}

?>
```

We expect to see:

```
array(3) {
  ["progress_pct"]=>
  int(70)
  ["records_scanned"]=>
  int(1000)
  ["status"]=>
  int(1)
}
```

