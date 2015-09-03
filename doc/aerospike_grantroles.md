
# Aerospike::grantRoles

Aerospike::grantRoles - Add roles to user's list of roles in a security-enabled Aerospike database

## Description

```
public int grantRoles ( string $user, array $roles [, array $options ] )
```

**Aerospike::grantRoles()** will add roles to a user's list of roles.

## Parameters

**user** string username.

**roles** an array of new roles to be assigned to the user.

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

$res = $db->grantRoles("john", array("read-write", "sys-admin"));
if ($res == Aerospike::OK) {
    echo "read-write and sys-admin roles successfully granted to user john";
} elseif ($res == Aerospike::ROLE_VIOLATION) {
    echo "User does not possess the required role to grant new roles";
} elseif ($res == Aerospike::INVALID_ROLE) {
    echo "Invalid Role being attempted to be assigned to user john";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
read-write and sys-admin roles successfully granted to user john
```

