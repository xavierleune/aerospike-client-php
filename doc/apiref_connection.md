
# Lifecycle and Connection Methods

### [Aerospike::__construct](aerospike_construct.md)
```
public int Aerospike::__construct ( array $config [, string $alias = Aerospike::CLUSTER_ALIAS [, array $options]] )
```

### [Aerospike::__destruct](aerospike_destruct.md)
```
public void Aerospike::__destruct ( void )
```

### [Aerospike::isConnected](aerospike_isconnected.md)
```
public boolean Aerospike::isConnected ( void )
```

### [Aerospike::close](aerospike_close.md)
```
public void Aerospike::close ( void )
```

### [Aerospike::reconnect](aerospike_reconnect.md)
```
public void Aerospike::reconnect ( void )
```

### [Aerospike::getNodes](aerospike_getnodes.md)
```
public int Aerospike::getNodes ( array &$metadata [, array $options ] )
```

## Example

```php
<?php

$config = array("hosts"=>array(array("addr"=>"192.168.1.10", "port"=>3000));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$res = $db->getNodes($node_metadata);
if ($res == Aerospike::OK) {
    var_dump($node_metadata);
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

$db->close();
$db->reconnect();
$db->__destruct();

?>
```
