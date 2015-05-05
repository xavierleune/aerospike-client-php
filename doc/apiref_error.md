
# Error Handling and Logging Methods

### [Aerospike::error](aerospike_error.md)
```
public string Aerospike::error ( void )
```

### [Aerospike::errorno](aerospike_errorno.md)
```
public int Aerospike::errorno ( void )
```

The error codes returned are constants of the **Aerospike** class, and map to
the client and server error codes defined in the C client (in as_status.h).

### [Aerospike::setLogLevel](aerospike_setloglevel.md)
```
public static Aerospike::setLogLevel ( int $log_level )
```

### [Aerospike::setLogHandler](aerospike_setloghandler.md)
```
public static Aerospike::setLogHandler ( callback $log_handler )
```

## Example

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}
$db->setLogLevel(Aerospike::LOG_LEVEL_DEBUG);
$db->setLogHandler(function ($level, $file, $function, $line) {
    switch ($level) {
        case Aerospike::LOG_LEVEL_ERROR:
            $lvl_str = 'ERROR';
            break;
        case Aerospike::LOG_LEVEL_WARN:
            $lvl_str = 'WARN';
            break;
        case Aerospike::LOG_LEVEL_INFO:
            $lvl_str = 'INFO';
            break;
        case Aerospike::LOG_LEVEL_DEBUG:
            $lvl_str = 'DEBUG';
            break;
        case Aerospike::LOG_LEVEL_TRACE:
            $lvl_str = 'TRACE';
            break;
        default:
            $lvl_str = '???';
    }
    error_log("[$lvl_str] in $function at $file:$line");
});

$key = array("ns" => "test", "set" => "users", "key" => 1234);
$put_bins = array("email" => "hey@example.com", "name" => "Hey There");
// attempt to 'CREATE' a new record at the specified key
$status = $db->put($key, $put_bins, 0, array(Aerospike::OPT_POLICY_EXISTS => Aerospike:POLICY_EXISTS_CREATE));
if ($status == Aerospike::OK) {
    echo "Record written.\n";
} elseif ($status == Aerospike::ERR_RECORD_EXISTS) {
    echo "The Aerospike server already has a record with the given key.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

// check for the existance of the given key in the database, then fetch it
if ($db->exists($key, $foo) == Aerospike::OK) {
    $status = $db->get($key, $record);
    if ($status == Aerospike::OK) {
        var_dump($record);
    }
}

// filtering for specific keys
$status = $db->get($key, $record, array("email"), Aerospike::POLICY_RETRY_ONCE);
if ($status == Aerospike::OK) {
    echo "The email for this user is ". $record['email']. "\n";
    echo "The name bin should be filtered out: ".var_export(is_null($record['name']), true). "\n";
}
?>
```
