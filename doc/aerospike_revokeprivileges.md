
# Aerospike::revokePrivileges

Aerospike::revokePrivileges - Revoke privileges from a role's list of privileges in a security-enabled Aerospike database

## Description

```
public int revokePrivileges ( string $role, array $privileges [, array $options ] )
```

**Aerospike::revokePrivileges()** will revoke privileges from a role's list of privileges.

## Parameters

**role** string rolename.

**privileges** an array of old privileges to be revoked from the role.

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

$res = $db->revokePrivileges("example_role", array(array("code":
                Aerospike::READ_WRITE_UDF)));
if ($res == Aerospike::OK) {
    echo "read-write_udf privileges successfully revoked from role";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
read-write_udf privileges successfully revoked from role
```

