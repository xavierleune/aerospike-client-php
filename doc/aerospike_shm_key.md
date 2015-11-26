
# Aerospike::shm_key

Aerospike::shm_key - Method gives shm_key which is set

## Description

```
public Aerospike::shm_key ( void )
```

**Aerospike::shm_key** If shared memory is enabled then it will return shm_key
which is set by user and If shm_key is not passed then unique generated shm_key
will be returned.
If shared memory isn't configured then it will return NULL.

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)),
        "shm"=>array("shm_key"=>0xA5000001));
$opts = array(Aerospike::OPT_CONNECT_TIMEOUT => 8000, Aerospike::OPT_WRITE_TIMEOUT => 1500);
$db = new Aerospike($config, true, $opts);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}
$shared_memory_key = $db->shm_key();
var_dump($shared_memory_key);

?>
```

we expect to see:

```
2768240641
```

