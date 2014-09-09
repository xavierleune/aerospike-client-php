
# Aerospike::scanBackground

Aerospike::scanBackground - Initiates a background read/write scan by applying a record UDF to each record being scanned.

## Description

```
public int Aerospike::scanBackground ( string $module, string $function, array $args, string $ns, string $set, int &$scan_id, [, int $percent = 100 [, int $scan_priority = Aerospike::SCAN_PRIORITY_AUTO [, boolean $concurrent = false [, boolean $no_bins = false [, array $options ]]]]] )
```

**Aerospike::scanBackground()** will initiate a background read/write scan by applying the record UDF *module*
*function* with *args* to each record being scanned on *ns*.*set*.
The *scan_id* is then filled with an integer handle for the initiated background scan.
Currently, scanning and applying a Record UDF on each record can only be performed in the background,
meaning the client sends the scan request to the database and does not wait for results.
The scan will be queued and run on the database, and no results will be returned to the client.
The client will have a job id for the scan it sent, so it can check the status.

Currently the only UDF language supported is Lua.  See the
[UDF Developer Guide](http://www.aerospike.com/docs/udf/udf_guide.html) on the Aerospike website.

## Parameters

**module** the name of the UDF module registered against the Aerospike DB.

**function** the name of the function to be applied to the records.

**args** an array of arguments for the UDF.

**ns** the namespace

**set** the set

**scan_id** filled by an integer handle for the initiated background scan

**percent** the percentage of data to scan.

**scan_priority** the priority of the scan.

**concurrent** whether to scan all nodes in parallel.

**no_bins** whether to return only metabins and exclude bins.

**options** including **Aerospike::OPT_WRITE_TIMEOUT** and **Aerospike::OPT_READ_TIMEOUT**.

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## See Also

- [Aerospike::scanInfo()](aerospike_scaninfo.md)

## Examples

### Example Record UDF

Registered module **my_udf.lua**
```lua
function mytransform(rec, offset)
    rec['a'] = rec['a'] + offset
    rec['b'] = rec['a'] * offset
    rec['c'] = rec['a'] + rec['b']
    aerospike:update(rec)
end
```

### Example of initiating a background read/write scan by applying a record UDF

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

// assuming test.users has integer bins 'a', 'b' and 'c'.
// Adds offset to the value in bin 'a', multiplies the value in bin 'b' by offset and
// Updates value in bin 'c' with the sum of updated values in bins 'a' and 'b'.

$res = $db->scanBackground("my_udf", "mytransform", array(20), "test", "users", $scan_id);
if ($res == Aerospike::OK) {
    var_dump($scan_id);
} else if ($res == Aerospike::ERR_SCAN) {
    echo "An error occured while initiating the BACKGROUND SCAN [{$db->errorno()}] ".$db->error();
} else {
    echo "An error occured while running the BACKGROUND SCAN [{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
int(1)
```

