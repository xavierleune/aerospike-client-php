
# Aerospike::setLogLevel

Aerospike::setLogLevel - set the logging threshold of the Aerospike object

## Description

```
public static void setLogLevel ( int $log_level )
```

**Aerospike::setLogLevel()** declares a logging threshold for the Aerospike C client.
This is a static method and the log handler is global across all instances of
the Aerospike class.

## Parameters

**log_level** one of *Aerospike::LOG_LEVEL_\** values

## Examples

```php
<?php

Aerospike::setLogLevel(Aerospike::LOG_LEVEL_DEBUG);

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000));
$db = new Aerospike($config, 'prod-db');
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

?>
```
