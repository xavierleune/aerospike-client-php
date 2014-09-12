
# AerospikeLSet::__construct

AerospikeLSet::__construct - constructor for creating AerospikeLSet objects.

## Description

```
public int AerospikeLSet::__construct ( Aerospike $db, array $key, string $bin )
```

**AerospikeLSet::__construct()** is the constructor of the AerospikeLSet class.

**AerospikeLSet::error()** and **AerospikeLSet::errorno()** methods can be used
to inspect whether an error occured at instantiation.

## Parameters

**db** the client instance created with Aerospike::__construct()

**key** the key under which the record is found. An array with keys 'ns','set','key'.

**bin** the name of the bin.


## See Also

- [AerospikeLDT::__construct()](aerospikeldt_construct.md)

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

$key = $db->initKey("rental", "vehicle", "4BPR005");
$llist = new AerospikeLSet($db, $key, "history");
if ($res != Aerospike::OK) {
    echo "LDT constructor error[{$db->errorno()}]: ".$db->error();
}
?>
```

