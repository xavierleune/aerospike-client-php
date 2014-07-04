
# Aerospike::get

Aerospike::get - gets a record from the Aerospike database

## Description

```
public int Aerospike::get ( array $key, array &$record [, array $filter [, array $options]] )
```

**Aerospike::get()** will read a *record* with a given *key*, where the *record*
is filled with an associative array of bins and values.  The bins returned in
*record* can be filtered by passing an associative array of the bins needed.
Non-existent bins will appear in the *record* with a NULL value.

## Parameters

**key** the key under which to store the record. An associative array with keys 'ns','set','key'.

**record** filled by an associative array of bins and values.

**filter** an array of bin names

**options** including **Aerospike::OPT_READ_TIMEOUT** and **Aerospike::OPT_POLICY_RETRY**.

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Examples

### Example #1 Aerospike::get() default behavior example

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$key = array("ns" => "test", "set" => "users", "key" => "1234");
$res = $db->get($key, $record);
if ($res == Aerospike::OK) {
    var_dump($record);
elseif ($res == Aerospike::ERR_RECORD_NOT_FOUND) {
    echo "A user with key ". $key['key']. " does not exist in the database\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
array(3) {
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

