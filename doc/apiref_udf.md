
# UDF Methods

### [Aerospike::register](aerospike_register.md)
```
public int Aerospike::register ( string $path, string $module [, int $language = Aerospike::UDF_TYPE_LUA] )
```

### [Aerospike::deregister](aerospike_deregister.md)
```
public int Aerospike::deregister ( string $module )
```

### [Aerospike::apply](aerospike_apply.md)
```
public int Aerospike::apply ( array $key, string $module, string $function[, array $args [, mixed &$returned ]] )
```

### [Aerospike::aggregate](aerospike_aggregate.md)
```
public int Aerospike::aggregate ( string $module, string $function, array $args, string $ns, string $set, array $where, mixed &$value )
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

$key = array("ns" => "test", "set" => "users", "key" => 1234);
$put_val = array("email" => "hey@example.com", "name" => "Hey There");
// attempt to 'CREATE' a new record at the specified key
$res = $db->put($key, $put_val, 0, array(Aerospike::OPT_POLICY_EXISTS => Aerospike::POLICY_EXISTS_CREATE));
if ($res == Aerospike::OK) {
    echo "Record written.\n";
} elseif ($res == Aerospike::ERR_RECORD_EXISTS) {
    echo "The Aerospike server already has a record with the given key.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

// apply a UDF to a record
$res = $db->apply($key, 'my_udf', 'startswith', array('email', 'hey@'), $returned);
if ($res == Aerospike::OK) {
    if ($returned) {
        echo "The email of the user with key {$key['key']} starts with 'hey@'.\n";
    } else {
        echo "The email of the user with key {$key['key']} does not start with 'hey@'.\n";
    }
} elseif ($res == Aerospike::ERR_UDF_NOT_FOUND) {
    echo "The UDF module my_udf.lua was not registered with the Aerospike DB.\n";
}

// filtering for specific keys
$res = $db->get($key, $record, array("email"), Aerospike::POLICY_RETRY_ONCE);
if ($res == Aerospike::OK) {
    echo "The email for this user is ". $record['email']. "\n";
    echo "The name bin should be filtered out: ".var_export(is_null($record['name']), true). "\n";
}
?>
```
