
# Aerospike::createIndex (deprecated)

Aerospike::createIndex - creates a secondary index on a bin

## Description

```
public int Aerospike::createIndex ( string $ns, string $set, string $bin, int $type, string $name [, array $options ] )
```

**Aerospike::createIndex()** will create a secondary index of a given *type* on
a namespace, *set* and *bin* with a specified *name*.

**Deprecated** in favor of [addIndex()](aerospike_addindex.md). Will be removed
with client release 3.4.0.

## Parameters

**ns** the namespace

**set** the set

**bin** the bin on which the secondary index is to be created

**type** one of *Aerospike::INDEX_TYPE_\**

**name** the name of the index

**options** including
- **Aerospike::OPT_WRITE_TIMEOUT**

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$status = $db->createIndex("test", "user", "email", Aerospike::INDEX_TYPE_STRING, "user_email_idx");
if ($status == Aerospike::OK) {
    echo "Index user_email_idx created on test.user.email\n";
else if ($status == Aerospike::ERR_INDEX_FOUND) {
    echo "This index has already been created.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
Index user_email_id created on test.user.email
```

## See Also

### [addIndex()](aerospike_addindex.md)

