
# Aerospike::queryUser

Aerospike::queryUser - Retrieve roles for a given user in a security-enabled Aerospike database

## Description

```
public int queryUser ( string $user, array &$roles [, array $options ] )
```

**Aerospike::queryUser()** will retrieve roles for a given user.

## Parameters

**user** string username.

**roles** an array of roles possessed by the user.

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
    $res = $db->queryUser("john", $roles);
    if ($res == Aerospike::OK) {
        var_dump($roles);
    } elseif ($res == Aerospike::INVALID_USER) {
        echo "Invalid user being queried";
    } else {
        echo "[{$db->errorno()}] ".$db->error();
    }
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
array(3) {
  [0]=>
    string(4) "read"
  [1]=>
    string(10) "read-write"
  [2]=>
    string(9) "sys-admin"
}
```

