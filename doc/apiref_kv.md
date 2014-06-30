
# Key-Value Methods

### [Aerospike::put](aerospike_put.md)
```
public int Aerospike::put ( string $key, array $record [, int $ttl = 0 [, array $options ]] )
```

### [Aerospike::get](aerospike_get.md)
```
public int Aerospike::get ( string $key, array &$record [, array $filter [, array $options ]] )
```

### [Aerospike::remove](aerospike_remove.md)
```
public int Aerospike::remove ( string $key [, array $options ] )
```

### [Aerospike::exists](aerospike_exists.md)
```
public int Aerospike::exists ( string $key, array &$metadata [, array $options ] )
public int Aerospike::getMetadata ( string $key, array &$metadata [, array $options ] )
```

### [Aerospike::touch](aerospike_touch.md)
```
public int Aerospike::touch ( string $key, int $ttl = 0 [, array $options ] )
```

### [Aerospike::increment](aerospike_increment.md)
```
public int Aerospike::increment ( string $key, string $bin, int $offset [, int $initial_value = 0 [, array $options ]] )
```

### [Aerospike::append](aerospike_append.md)
```
public int Aerospike::append ( string $key, string $bin, string $value [, array $options ] )
```

### [Aerospike::prepend](aerospike_prepend.md)
```
public int Aerospike::prepend ( string $key, string $bin, string $value [, array $options ] )
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

$key = array("ns" => "test", "set" => "users", "key" => 1234);
$put_val = array("email" => "hey@example.com", "name" => "Hey There");
// attempt to 'CREATE' a new record at the specified key
$res = $db->put($key, $put_val, 0, array(Aerospike::OPT_POLICY_EXISTS => Aerospike::POLICY_EXISTS_CREATE));
if ($res == Aerospike::OK) {
    echo "Record written.\n";
} elseif ($res == Aerospike::KEY_FOUND_ERROR) {
    echo "The Aerospike server already has a record with the given key.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

// check for the existance of the given key in the database, then fetch it
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
