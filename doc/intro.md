---
title: Introduction
description: The Aerospike PHP Client permits the creation of PHP applications using the full range of operations and data types with the Aerospike database.
---

The Aerospike PHP client permits the creation of PHP applications using the full range of operations and data types with the Aerospike database.

```
// Simple example of using the Aerospike PHP client API:

// Create a client connection to the Aerospike cluster:

// [Note:  The server is running on the local host on port 3000.]
$config = array("hosts"=>array(array("name"=>"localhost", "port"=>3000));

try {
   $as = new Aerospike($config);
} catch (AerospikeException $e) {
   echo "Aerospike client creation failed: " . $e->getMessage() . "\n";
   exit -1;
}

// Store an object in the database:
$key1 = array("ns"=>"test", "set"=>"demo", "key"=>1234);
$rec1 = array("bin1"=>"value1");
$rv = $as->put($key1, $rec1);

// Retrieve and print the object:
$rv = $as->get($key1, &$rec2);
if ($rv == Aerospike::OK) {
  var_dump($rec2);
} else {
  echo "Failed to retrieve key! rv = " . $rv;
}
```
