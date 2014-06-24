
# Aerospike::get

Aerospike::get - gets a record from the Aerospike database

## Description

```
public int Aerospike::get ( string $key, array &$record [, array $filter [, int $policy ]] )
```

**Aerospike::get** will read a *record* with a given *key*, where the *record*
is filled with an associative array of bins and values.  The bins returned in
*record* can be filtered by passing an associative array of the bins needed.
Non-existent bins will appear in the *record* with a NULL value.

he behavior of **Aerospike::get** can be modified using the *policy* parameter.

## Parameters

**key** the key under which to store the record.

**record** filled by an associative array of bins and values.

**filter** an array of bin names

**policy** optionally **Aerospike::POLICY_RETRY_ONCE**

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Examples

### Example #1 Aerospike::get() default behavior example

```php
<?php

$config = array("hosts" => array(array("name" => "localhost", "port" => 3000));

try {
   $db = new Aerospike($config);
} catch (AerospikeException $e) {
   echo "Aerospike client creation failed: " . $e->getMessage() . "\n";
   exit(1);
}

$key = array("ns" => "test", "set" => "users", "key" => "1234");
$res = $db->get($key, $record);
if ($res == Aerospike::OK) {
    var_dump($record);
elseif ($res == Aerospike::KEY_NOT_FOUND_ERROR) {
    echo "A user with key ". $key['key']. " does not exist in the database\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
array(2) {
  ["email"]=>
  string(9) "hey@example.com"
  ["name"]=>
  string(9) "You There"
  ["age"]=>
  int(33)
}
```

### Example #2 get the record with filtered bins

```php
<?php

// assuming this follows Example #1

// Getting a filtered record
$filter = array("email", "manager");
unset($record);
$res = $db->get($key, $record, $filter);
if ($res == Aerospike::OK) {
    var_dump($record);
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
array(2) {
  ["email"]=>
  string(9) "hey@example.com"
  ["manager"]=>
  NULL
}
```

