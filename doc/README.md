
# Overview

The Aerospike PHP client API may be described as follows:

### [Aerospike Class](aerospike.md)
### [Lifecycle and Connection Methods](apiref_connection.md)
### [Error Handling Methods](apiref_error.md)
### [Key-Value Methods](apiref_kv.md)
### [Query and Scan Methods](apiref_streams.md)
### [User Defined Methods](apiref_udf.md)
### [Admin Methods](apiref_admin.md)

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

## TBD
```

// Client interface to the Aerospike cluster.
class Aerospike
{
    // Query APIs:
    public function queryApply($set, $where, $callback, $apply, $bins, $options); // apply UDF on the query

    // Scan APIs:
    public function scanApply($set, $callback, $apply, $bins, $options); // apply UDF on the scan

    // helper method for combining predicates, such as predicate1 AND predicate2
    public array Aerospike::conjoin ( array $predicate, string $conjunction, array $next_predicate [, boolean $parenthesize = false] )

    // Large Data Type (LDT) APIs:
    public function getLargeList($key_z);
    public function getLargeMap($key_z);
    public function getLargeSet($key_z);
    public function getLargeStack($key_z);
}
```
