
# Aerospike::createRole

Aerospike::createRole - creates a new user defined role in a security-enabled Aerospike database

## Description

```
public int Aerospike::createRole ( string $role, array $privileges [, array $options ] )
```

**Aerospike::createRole()** will create a user defined role with specified privileges.
Clear-text password will be hashed using bcrypt before sending to server.

## Parameters

**role** string rolename.

**privileges** an array of privileges to be assigned to the role.

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

$res = $db->createRole("example_role", array(array("code" : Aerospike::READ, "ns":
                "test", "set": "demo")));
if ($res == Aerospike::OK) {
    echo "Role successfully created";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
Role successfully created
```

