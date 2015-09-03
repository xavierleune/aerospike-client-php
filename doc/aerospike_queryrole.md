
# Aerospike::queryRole

Aerospike::queryRole - Query a given user defined role in a security-enabled Aerospike database

## Description

```
public int queryRole ( string $role, array &$privileges [, array $options ] )
```

**Aerospike::queryRole()** will query  a user defined role.

## Parameters

**role** string rolename.

**privileges** an array of privileges possessed by the role.

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
   echo "Aerospike failed to connect[{$db->errorno()} {$db->error()}";
   exit(1);
}

$res = $db->grantPrivileges("example_role", array(array("code": Aerospike::READ)));
if ($res == Aerospike::OK) {
    echo "read privileges successfully granted to role";
    $res = $db->queryRole("example_role", $privileges);
    if ($res == Aerospike::OK) {
        var_dump($privileges);
    } else {
        echo "[{$db->errorno()}] ".$db->error();
    }
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
read privileges successfully granted to role
array(2) {
  [0]=>
  array(3) {
    ["ns"]=>
    string(0) ""
    ["set"]=>
    string(0) ""
    ["code"]=>
    int(10)
  }
  [1]=>
  array(3) {
    ["ns"]=>
    string(4) "test"
    ["set"]=>
    string(4) "demo"
    ["code"]=>
    int(11)
  }
}
```

