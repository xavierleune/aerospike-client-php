
# Aerospike::dropRole

Aerospike::dropRole - Drops an existing user defined role in a security-enabled Aerospike database

## Description

```
public int Aerospike::dropRole ( string $role [, array $options ] )
```

**Aerospike::dropRole()** will drop an existing user defined role from the cluster.

## Parameters

**role** string rolename.

**[options](aerospike.md)** including
- **Aerospike::OPT_WRITE_TIMEOUT**

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Example

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)),
        "user"=>"admin", "pass"=>"admin");
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$res = $db->dropRole("example_role");
if ($res == Aerospike::OK) {
    echo "Role successfully removed";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
Role successfully removed
```

