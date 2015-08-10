
# Security Methods

### [Aerospike::createUser](aerospike_createuser.md)
```
public int Aerospike::createUser ( string $user, string $password, array $roles [, array $options ] )
```

### [Aerospike::dropUser](aerospike_dropuser.md)
```
public int Aerospike::dropUser ( string $user [, array $options ] )
```

### [Aerospike::changePassword](aerospike_changepassword.md)
```
public int changePassword ( string $user, string $password [, array $options ] )
```

### [Aerospike::grantRoles](aerospike_grantroles.md)
```
public int grantRoles ( string $user, array $roles [, array $options ] )
```

### [Aerospike::revokeRoles](aerospike_revokeroles.md)
```
public int revokeRoles ( string $user, array $roles [, array $options ] )
```

### [Aerospike::replaceRoles](aerospike_replaceroles.md)
```
public int replaceRoles ( string $user, array $roles [, array $options ] )
```

### [Aerospike::queryUser](aerospike_queryuser.md)
```
public int queryUser ( string $user, array &$roles [, array $options ] )
```

### [Aerospike::queryUsers](aerospike_queryusers.md)
```
public int queryUsers ( array &$roles [, array $options ] )
```

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

$res = $db->createUser("john", "john@123", array("read-write"));
$res = $db->createUser("steve", "steve@123", array("user-admin", "sys-admin"));
if ($res == Aerospike::OK) {
    $res = $db->queryUsers($roles);
    if ($res == Aerospike::OK) {
        var_dump($roles);
    } else {
        echo "[{$db->errorno()}] ".$db->error();
    }
} elseif ($res == Aerospike::ROLE_VIOLATION) {
    echo "User does not possess the required role to create user";
} elseif ($res == Aerospike::INVALID_ROLE) {
    echo "Invalid Role being attempted to be assigned to user";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
array(2) {
  ["john"]=>
    array(1) {
      [0]=>
        string(10) "read-write"
    }
  ["steve"]=>
    array(2) {
      [0]=>
        string(10) "user-admin"
      [1]=>
        string(9) "sys-admin"
    }
}
```

