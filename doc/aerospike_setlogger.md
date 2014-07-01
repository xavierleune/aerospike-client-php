
# Aerospike::setLogger

Aerospike::setLogger - sets the log file

## Description

```
public bool setLogger ( string $log_path [, int $log_level = Aerospike::LOG_LEVEL_INFO] )
```

**Aerospike::setLogger()** declares a log file for the Aerospike C client and
its logging threshold.

This method is implicitly called when the aerospike.log_path and
aerospike.log_level configurations are declared in php.ini .  If those
configuration parameters exist the **Aerospike::setLogger()** method can still
be used to override those values with new ones.

## Parameters

**log_path** the path to the log file

**log_level** one of *Aerospike::LOG_LEVEL_* values

## Return Values

Returns a **true** or **false** confirming the logger was configured.

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}
$db->setLogger('/path/to/logger/aerospike.log', Aerospike::LOG_LEVEL_DEBUG);

?>

