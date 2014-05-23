---
title: Introduction
description: Aerospike's PHP v3 client permits the creation of PHP applications using the full range of operations and data types with the Aerospike database.
---

Aerospike's PHP v3 client permits the creation of PHP applications using the full range of operations and data types with the Aerospike database.

```
// Example of using the PHP v3 API:

// Create a connection to the Aerospike cluster:
$config = array("hosts"=>array(array("name"=>"localhost","port"=>3000));
$as = new aerospike($config);
$rv = $as->connect();

// Store an object in the database:
$k1 = array("ns"=>"test", "set"=>"demo", "key"=>1);
$rv = $as->key_put($k1);

// Retrieve and print the object:
$rv = $as->key_get(&k2);
var_dump($k2);
```
