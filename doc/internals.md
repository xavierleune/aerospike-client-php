---
title: PHP v3 Client Internals
description: Learn about the internal architecture of the PHP v3 API for the Aerospike database.
---

# PHP v3 Client Internal Architecture

The PHP v3 Client API provides access to the Aerospike server via a set
of PHP objects.  The objects are implented as a PHP extension
implemented in the C programming language using the Zend Engine
extension API.  The Aerospike PHP v3 extension is layered on top of the
Aerospike C client.

The PHP v3 API is structurally quit similar to the other Aerospike
client APIs, especially the Node.js client API.

## Arrays as Containers

PHP array objects are used to describe data structures, such as for
specifing a particlar object in the database. For example, to refer to
the key `key1` in namespace `test` and set `demo`, an array of the
following form must be created:

```
$my_key = array("ns"=>"test", "set"=>"demo", "key"=>"key1");
```

Internally, the PHP API method implementation C function receives an
array object as an input `zval` argument.  The C function first
validates each argument type (to the extent that it can) and the
contents of structured objects.  A number of the API methods are
polymorphic, meaning they have different behaviors depending upon
received argument types.
