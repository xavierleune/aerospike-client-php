
# Overview

The Aerospike PHP client API may be described as follows:

### [Aerospike Class](aerospike.md)
### [Lifecycle and Connection Methods](apiref_connection.md)
### [Error Handling Methods](apiref_error.md)
### [Key-Value Methods](apiref_kv.md)
### [Query and Scan Methods](apiref_streams.md)
### [User Defined Methods](apiref_udf.md) \[to be implemented\]
### [Admin Methods](apiref_admin.md) \[to be implemented\]

## Implementation Status
So far the *Runtime Configuration*, *Lifecycle and Connection Methods*, *Error*
*Handling and Logging Methods*, and parts of *Key-Value Methods* and *Query and*
*Scan Methods* have been implemented.

The *User Defined Methods* and *Admin Methods* are yet to be implemented.

The *Large Data Type Methods* are to be determined (no spec available, yet).

We expect the specification of the PHP client to closely describe our next
release, including the unimplemented methods.  However, it is possible that
some changes to the client spec will occur.

# Client instance caching

Cluster instance caching

The initialization of the C-client to talk to a specified cluster is a heavy operation, so ideally the C-client should be reused for multiple requests made against the same PHP process (as is the case for mod_php and fastCGI).

The (user land) PHP developer will pass an alias for the cluster that should be used to persistently identify the C-client instance associated with it.  After the first time Aerospike::__construct() is called in the process for a cluster specified by the alias, it should be placed in memory that was allocated using the [persistent memory API](www.php.net/manual/en/internals2.memory.persistence.php).

When the process is terminated the module shutdown callbacks are called. A cleanup of the resources used by the Aerospike object should be registered with [PHP_MSHUTDOWN](http://www.php.net/manual/en/internals2.structure.modstruct.php) and include explicitly disconnecting from the cluster (aerospike_close, aerospike_destroy).

# Halting a Stream

Halting a query or scan stream can be done by returning (an explicit) boolean
**false** from the callback.  The extension should capture the return value from
the userland callback and if it is **=== false** it should return a false to the
C-client.  The C-client will then close the sockets to the nodes involved in
streaming results, effectively halting it.

# Handling Unsupported Types

See: [as_bytes.h](https://github.com/aerospike/aerospike-common/blob/master/src/include/aerospike/as_bytes.h)
* Allow the user to configure their serializer through an option.
 - OPT\_SERIALIZER : SERIALIZER\_PHP (default), SERIALIZER\_NONE, SERIALIZER\_USER, *(SERIALIZER\_JSON)*
* when a write operation runs into types that do not map directly to Aerospike DB types it checks the OPT\_SERIALIZER setting:
 - if SERIALIZER\_NONE it returns an Aerospike::ERR\_PARAM error
 - if SERIALIZER\_PHP it calls the PHP serializer, sets the object's as\_bytes\_type to AS\_BYTES_PHP
 - *(if SERIALIZER\_JSON it calls json\_encode, sets the object's as\_bytes\_type AS\_BYTES_JSON)*
 - if SERIALIZER\_USER it calls the PHP function the user registered a callback with Aerospike::setSerializer(), and sets as\_bytes\_type to AS\_BYTES\_BLOB
* when a read operation extracts a value from an AS\_BYTES type bin:
 - if it’s a AS\_BYTES\_PHP use the PHP unserialize function
 - *(if it’s a AS\_BYTES\_JSON call json_decode)*
 - if it’s a AS\_BYTES\_BLOB and the user registered a callback with Aerospike::setSerializer() call that function, otherwise place it in a PHP string

## TBD

```php
<?php

// Client interface to the Aerospike cluster.
class Aerospike
{
    // helper method for combining predicates, such as predicate1 AND predicate2
    public array Aerospike::conjoin ( array $predicate, string $conjunction, array $next_predicate [, boolean $parenthesize = false] )

    // Large Data Type (LDT) APIs:
    public function getLargeList($key_z);
    public function getLargeMap($key_z);
    public function getLargeSet($key_z);
    public function getLargeStack($key_z);
}
?>
```

