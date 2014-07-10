
# Query and Scan Methods

### [Aerospike::query](aerospike_query.md)
```
public int Aerospike::query ( string $ns, string $set, array $where, callback $record_cb [, array $bins [, array $options ]] )
```

### [Aerospike::scan](aerospike_scan.md)
```
public int Aerospike::scan ( string $ns, string $set, callback $record_cb [, array $bins [, array $options ]] )
```

### [Aerospike::predicateEquals](aerospike_predicateequals.md)
```
public array Aerospike::predicateEquals ( string $bin, int|string $val )
```

### [Aerospike::predicateBetween](aerospike_predicatebetween.md)
```
public array Aerospike::predicateBetween ( string $bin, int $min, int $max )
```

### Aerospike::conjoin [reserved]
```
public array Aerospike::conjoin ( array $predicate, string $conjunction, array $next_predicate [, boolean $parenthesize = false] ) 
```

## Example

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000));
$db = new Aerospike($config, 'prod-db');
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$total = 0;
$in_thirties = 0;
$where = Aerospike::predicateBetween("age", 30, 39);
$res = $db->query("test", "users", $where, function ($record) {
    echo "{$record['email']} age {$record['age']}\n";
    $total += (int) $record['age'];
    $in_thirties++;
    if ($in_thirties >= 10) return false; // stop the stream at the tenth record
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

