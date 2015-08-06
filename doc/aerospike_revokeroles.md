
# Aerospike::revokeRoles

Aerospike::revokeRoles - Remove roles from user's list of roles in a security-enabled Aerospike database

## Description

```
public int revokeRoles ( string $user, array $roles [, array $options ] )
```

**Aerospike::revokeRoles()** will remove roles from user's list of existing roles.

## Parameters

**user** string username.

**roles** an array of roles to be revoked from existing roles of the user.

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

$res = $db->revokeRoles("john", array("user-admin", "sys-admin"));
if ($res == Aerospike::OK) {
    echo "User john's user-admin and sys-admin roles have been successfully revoked";
} elseif ($res == Aerospike::ROLE_VIOLATION) {
    echo "User does not possess the required role to revoke roles";
} elseif ($res == Aerospike::INVALID_ROLE) {
    echo "Invalid Role being attempted to be revoked";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
User john's user-admin and sys-admin roles have been successfully revoked
```

