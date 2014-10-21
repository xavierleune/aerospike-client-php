
# Info Methods

### [Aerospike::getNodes](aerospike_getnodes.md)
```
public array Aerospike::getNodes ( void )
```

### [Aerospike::info](aerospike_info.md)
```
public int Aerospike::info ( string $request, string &$response [, array $host ] )
```

### [Aerospike::infoMany](aerospike_infomany.md)
```
public array Aerospike::infoMany ( string $request [, array $config ] )
```

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$opts = array(Aerospike::OPT_CONNECT_TIMEOUT => 1250, Aerospike::OPT_WRITE_TIMEOUT => 1500);
$db = new Aerospike($config, true, $opts);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$status = $db->info('bins/test', $response);
if ($status == Aerospike::OK) {
    var_dump($response);
}

// Get the nodes in the cluster
$nodes = $db->getNodes();
var_dump($nodes);

?>

