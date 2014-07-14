
# Admin Methods

### [Aerospike::createIndex](aerospike_createindex.md)
```
public int Aerospike::createIndex ( string $ns, string $set, string $bin, int $type, string $name )
```

### [Aerospike::dropIndex](aerospike_dropindex.md)
```
public int Aerospike::dropIndex ( string $ns, string $name )
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

$res = $db->createIndex("test", "user", "email", Aerospike::INDEX_TYPE_STRING, "user_email_idx");
if ($res == Aerospike::OK) {
    echo "Index user_email_idx created on test.user.email\n";
else if ($res == Aerospike::ERR_INDEX_FOUND) {
    echo "This index has already been created.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

