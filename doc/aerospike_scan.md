
# Aerospike::scan

Aerospike::scan - scans a set in the Aerospike database

## Description

```
public int Aerospike::scan ( string $ns, string $set, callback $record_cb [, array $select [, array $options ]] )
```

**Aerospike::scan()** will scan a *set* and invoke a callback function 
*record_cb* on each record in the result stream.
A selection of bins returned can be determined by passing an array in *select*,
otherwise all bins in the record are returned.
.
Non-existent bins will appear in the *record* with a NULL value.

## Parameters

**ns** the namespace

**set** the set to be scanned

**record_cb** a callback function invoked for each record streaming back from the server.

**bins** an array of bin names to be returned.

**percent** the percentage of data to scan.

**scan_priority** the priority of the scan.

**concurrent** whether to scan all nodes in parallel.

**no_bins** whether to return only metabins and exclude bins.

**options** including
- **Aerospike::OPT_POLICY_RETRY**
- **Aerospike::OPT_SCAN_PRIORITY**
- **Aerospike::OPT_SCAN_PERCENTAGE** of the records in the set to return
- **Aerospike::OPT_SCAN_CONCURRENTLY** whether to run the scan in parallel
- **Aerospike::OPT_SCAN_NOBINS** whether to not retrieve bins for the records

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$processed = 0;
$res = $db->scan("test", "users", function ($record) {
    if (!is_null($record['email'])) echo $record['email']."\n";
    if ($processed++ > 19) return false; // halt the stream by returning a false
}, array("email"));
if ($res == Aerospike::ERR_SCAN) {
    echo "An error occured while scanning[{$db->errorno()}] {$db->error()}\n";
} else if ($res == Aerospike::ERR_SCAN_ABORTED) {
    echo "I think a sample of $processed records is enough\n";
}

?>
```

We expect to see:

```
foo@example.com
:
bar@example.com
I think a sample of 20 records is enough
```

