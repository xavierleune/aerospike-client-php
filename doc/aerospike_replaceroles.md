
# Aerospike::replaceRoles

Aerospike::replaceRoles - Replace user's list of roles with a new list of roles in a security-enabled Aerospike database

## Description

```
public int replaceRoles ( string $user, array $roles [, array $options ] )
```

**Aerospike::replaceRoles()** will replace user's list of existing roles with a new list of roles.

## Parameters

**user** string username.

**roles** an array of new roles to be assigned to the user by replacing existing roles.

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

$res = $db->replaceRoles("john", array("user-admin", "sys-admin"));
if ($res == Aerospike::OK) {
    echo "User john's roles have been successfully replaced with user-admin and sys-admin";
} elseif ($res == Aerospike::ROLE_VIOLATION) {
    echo "User does not possess the required role to replace roles";
} elseif ($res == Aerospike::INVALID_ROLE) {
    echo "Invalid Role being attempted to be assigned to user john";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
User john's roles have been successfully replaced with user-admin and sys-admin
```

