
# Key-Value Methods

### [Aerospike::getKeyDigest](aerospike_getkeydigest.md)
```
public string Aerospike::getKeyDigest (string $ns, string $set, string|int $pk)
```

### [Aerospike::initKey](aerospike_initkey.md)
```
public array Aerospike::initKey ( string $ns, string $set, int|string $pk [, boolean $is_digest = false ] )
```

### [Aerospike::getKeyDigest](aerospike_getkeydigest.md)
```
public string Aerospike::getKeyDigest ( string $ns, string $set, int|string $pk )
```

### [Aerospike::put](aerospike_put.md)
```
public int Aerospike::put ( array $key, array $bins [, int $ttl = 0 [, array $options ]] )
```

### [Aerospike::get](aerospike_get.md)
```
public int Aerospike::get ( array $key, array &$record [, array $filter [, array $options ]] )
```

### [Aerospike::remove](aerospike_remove.md)
```
public int Aerospike::remove ( array $key [, array $options ] )
```

### [Aerospike::removeBin](aerospike_removebin.md)
```
public int Aerospike::removeBin ( array $key, array $bins [, array $options ] )
```

### [Aerospike::exists](aerospike_exists.md)
```
public int Aerospike::exists ( array $key, array &$metadata [, array $options ] )
public int Aerospike::getMetadata ( array $key, array &$metadata [, array $options ] )
```

### [Aerospike::touch](aerospike_touch.md)
```
public int Aerospike::touch ( array $key, int $ttl = 0 [, array $options ] )
```

### [Aerospike::increment](aerospike_increment.md)
```
public int Aerospike::increment ( array $key, string $bin, int $offset [, array $options ] )
```

### [Aerospike::append](aerospike_append.md)
```
public int Aerospike::append ( array $key, string $bin, string $value [, array $options ] )
```

### [Aerospike::prepend](aerospike_prepend.md)
```
public int Aerospike::prepend ( array $key, string $bin, string $value [, array $options ] )
```

### [Aerospike::operate](aerospike_operate.md)
```
public int Aerospike::operate ( array $key, array $operations [, array &$returned ] )
```

### [Aerospike::getMany](aerospike_getmany.md)
```
public int Aerospike::getMany ( array $keys, array &$records [, array $filter [, array $options]] )
```

### [Aerospike::existsMany](aerospike_existsmany.md)
```
public int Aerospike::existsMany ( array $keys, array &$metadata [, array $options ] )
```

### [Aerospike::setSerializer](aerospike_setserializer.md)
```
public static Aerospike::setSerializer ( callback $serialize_cb )
```

### [Aerospike::setDeserializer](aerospike_setdeserializer.md)
```
public static Aerospike::setDeserializer ( callback $unserialize_cb )
```


## Example

```php
<?php

$config = ["hosts" => [["addr"=>"localhost", "port"=>3000]], "shm"=>[]];
$client = new Aerospike($config, true);
if (!$client->isConnected()) {
   echo "Aerospike failed to connect[{$client->errorno()}]: {$client->error()}\n";
   exit(1);
}

$key = $client->initKey("test", "users", 1234);
$bins = ["email" => "hey@example.com", "name" => "Hey There"];
// attempt to 'CREATE' a new record at the specified key
$status = $client->put($key, $bins, 0, [Aerospike::OPT_POLICY_EXISTS => Aerospike::POLICY_EXISTS_CREATE]);
if ($status == Aerospike::OK) {
    echo "Record written.\n";
} elseif ($status == Aerospike::ERR_RECORD_EXISTS) {
    echo "The Aerospike server already has a record with the given key.\n";
} else {
    echo "[{$client->errorno()}] ".$client->error();
}

// check for the existance of the given key in the database, then fetch it
if ($client->exists($key, $foo) == Aerospike::OK) {
    $status = $client->get($key, $record);
    if ($status == Aerospike::OK) {
        var_dump($record);
    }
}

// filtering for specific keys
$status = $client->get($key, $record, ["email"], Aerospike::POLICY_RETRY_ONCE);
if ($status == Aerospike::OK) {
    echo "The email for this user is ". $record['bins']['email']. "\n";
    echo "The name bin should be filtered out: ".var_export(is_null($record['bins']['name']), true). "\n";
}
?>
```
