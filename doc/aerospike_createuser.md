
# Aerospike::createUser

Aerospike::createUser - creates a new user in a security-enabled Aerospike database

## Description

```
public int Aerospike::createUser ( string $user, string $password, array $roles [, array $options ] )
```

**Aerospike::createUser()** will create a user with password and roles.
Clear-text password will be hashed using bcrypt before sending to server.

## Parameters

**user** string username.

**password** string password.

**roles** an array of roles to be assigned to the user.

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

$res = $db->createUser("john", "mypass@123", array("read"));
if ($res == Aerospike::OK) {
    echo "User john successfully created";
} elseif ($res == Aerospike::ROLE_VIOLATION) {
    echo "User does not possess the required role to create a new user";
} elseif ($res == Aerospike::USER_ALREADY_EXISTS) {
    echo "User john already exists";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
User john successfully created
```

