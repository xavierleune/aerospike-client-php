
# Aerospike::query

Aerospike::query - queries a set in the Aerospike database

## Description

```
public int Aerospike::query ( mixed $set, array $where, callback $record_cb [, array $bins [, array $options ]] )
```

**Aerospike::query()** will query a *set* with a specified *where* predicate
then invoke a callback function *record_cb* on each record in the result stream.
The bins returned can be filtered by passing an associative array of the *bins*
needed, otherwise all bins in the record are returned (similar to a SELECT *).
Non-existent bins will appear in the *record* with a NULL value.

## Parameters

**set** either a string value *namespace.set* or an associative array with keys "ns", "set".

**where** the predicate conforming to one of the following:
```
Associative Array:
  bin => bin name
  op => one of Aerospike::OP_EQ, Aerospike::OP_BETWEEN
  val => scalar integer/string for OP_EQ or array($min, $max) for OP_BETWEEN
```
*or*
```
Tuple (Indexed Array):
  (bin, operator, value)
```
*examples:*
```
array("bin"=>"name", "op"=>Aerospike::OP_EQ, "val"=>"foo")
array("bin"=>"age", "op"=>Aerospike::OP_BETWEEN, "val"=>array(35,50))
array("name", Aerospike::OP_EQ, "foo")
array("age", Aerospike::OP_BETWEEN, array(35,50))
```

**record_cb** a callback function invoked for each record streaming back from the server.

**bins** an array of bin names to be returned.

**options** including **Aerospike::OPT_POLICY_RETRY**.

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$total = 0;
$in_thirties = 0;
$where = array("age", Aerospike::OP_BETWEEN, array(30, 39));
$res = $db->query('test.users', $where, function ($record) {
    echo "{$record['email']} age {$record['age']}\n";
    $total += (int) $record['age'];
    $in_thirties++;
}, array("email", "age"));
if ($res == Aerospike::ERROR_QUERY) {
    echo "An error occured while querying[{$db->errorno()}] ".$db->error();
} else {
    echo "The average age of employees in their thirties is ".round($total / $in_thirties)."\n";
}

?>
```

We expect to see:

```
foo age 32
bar age 35
The average age of employees in their thirties is 34
```

