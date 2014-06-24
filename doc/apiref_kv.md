
# Key-Value Methods

### [Aerospike::put](aerospike_put.md)
```
public int Aerospike::put ( string $key, array $record [, int $ttl = 0 [, int $policy ]] )
```

### [Aerospike::get](aerospike_get.md)
```
public int Aerospike::get ( string $key, array &$record [, array $filter [, int $policy ]] )
```

### [Aerospike::delete](aerospike_delete.md)
```
public int Aerospike::delete ( string $key [, int $policy ] )
```

### [Aerospike::exists](aerospike_exists.md)
```
public int Aerospike::exists ( string $key, array &$metadata [, int $policy ] )
public int Aerospike::getMetadata ( string $key, array &$metadata [, int $policy ] )
```

## Example

```php
<?php

$config = array("hosts"=>array(array("name"=>"localhost", "port"=>3000));

try {
   $db = new Aerospike($config);
} catch (AerospikeException $e) {
   echo "Aerospike client creation failed: " . $e->getMessage() . "\n";
   exit -1;
}

$key = array("ns" => "test", "set" => "users", "key" => 1234);
$put_val = array("email" => "hey@example.com", "name" => "Hey There");
// attempt to 'CREATE' a new record at the specified key
$res = $db->put($key, $put_val, 0, Aerospike::POLICY_RETRY_ONCE & Aerospike::POLICY_NO_KEY_COLLISION);

if ($res == Aerospike::OK) {
    echo "Record written.\n";
} elseif ($res == Aerospike::KEY_FOUND_ERROR) {
    echo "The Aerospike server already has a record with the given key.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

if ($db->exists($key, $foo) == Aerospike::OK) {
    $res = $db->get($key, $record);
    if ($res == Aerospike::OK) {
        var_dump($record);
    }
}

// filtering for specific keys
$res = $db->get($key, $record, array("email"), Aerospike::POLICY_RETRY_ONCE);
if ($res == Aerospike::OK) {
    echo "The email for this user is ". $record['email']. "\n";
    echo "The name bin should be filtered out: ".var_export(is_null($record['name']), true). "\n";
}
?>
```
