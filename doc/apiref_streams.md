
# Query and Scan Methods

### [Aerospike::query](aerospike_query.md)
```
public int Aerospike::query ( mixed $set, array $where, callback $record_cb [, array $bins [, array $options ]] )
```

### [Aerospike::scan](aerospike_scan.md)
```
public int Aerospike::scan ( mixed $set, callback $record_cb [, array $bins [, array $options ]] )
```

### [Aerospike::halt](aerospike_halt.md)
```
public void Aerospike::halt ( void )
```

## Example

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$total = 0;
$in_thirties = 0;
$where = array("age", Aerospike::OP_BETWEEN, array(30, 39));
$res = $db->query('test.users', $where, function ($record) {
    echo "{$record['email']} age {$record['age']}\n";
    $total += (int) $record['age'];
    $in_thirties++;
    if ($in_thirties >= 10) $db->halt(); // stop the stream at the tenth record
}, array("email", "age"));
if ($res == Aerospike::ERR_QUERY) {
    echo "An error occured while querying[{$db->errorno()}] ".$db->error();
else if ($res == Aerospike::ERR_QUERY_ABORTED) {
    echo "Stopped the result stream after {$in_thirties} results\n";
} else {
    echo "The average age of employees in their thirties is ".round($total / $in_thirties)."\n";
}

?>
```
