---
title: Introduction
description: The Aerospike PHP Client permits the creation of PHP applications using the full range of operations and data types with the Aerospike database.
---

The Aerospike PHP Client permits the creation of PHP applications using the full range of operations and data types with the Aerospike database.

```
// Simple example of using the Aerospike PHP Client API:

// Create a connection to the Aerospike cluster:
// [Note:  The default configuration is being used.]
$config = NULL;
$as = new aerospike($config);

// [Note:  Server is running on the local host on port 3000.]
$hosts = array("hosts"=>array(array("name"=>"localhost", "port"=>3000));
$rv = $as->connect($hosts);

// Store an object in the database:
$k1 = array("ns"=>"test", "set"=>"demo", "key"=>1);
$rv = $as->key_put($k1);

// Retrieve and print the object:
$rv = $as->key_get(&k2);
var_dump($k2);
```
