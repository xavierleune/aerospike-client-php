
# Overview

The Aerospike <a href="http://www.aerospike.com/docs/architecture/clients.html"
target="_doc">PHP client</a> enables your PHP application to work with an
<a href="http://www.aerospike.com/docs/architecture/distribution.html"
target="_doc">Aerospike cluster</a> as its
<a href="http://www.aerospike.com/docs/guide/kvs.html" target="_doc">key-value store</a>.

The <a href="http://www.aerospike.com/docs/architecture/data-model.html" target="_doc">Data Model</a>
document gives further details on how data is organized in the cluster.

## Client API
The Aerospike PHP client API is described in the following sections:

### [Runtime Configuration](aerospike_config.md)
### [Session Handler](aerospike_sessions.md)
### [Aerospike Class](aerospike.md)
### [Lifecycle and Connection Methods](apiref_connection.md)
### [Error Handling Methods](apiref_error.md)
### [Key-Value Methods](apiref_kv.md)
### [Query and Scan Methods](apiref_streams.md)
### [User Defined Methods](apiref_udf.md)
### [Admin Methods](apiref_admin.md)
### [Info Methods](apiref_info.md)
### [Large Data Type Methods](aerospike_ldt.md)

## Implementation Status
So far the *Runtime Configuration*, *Lifecycle and Connection Methods*, *Error*
*Handling and Logging Methods*, *Query and Scan Methods*, *User Defined Methods*
, *Admin Methods*, *Info Methods* and *Key-Value Methods* have been implemented.

The *Large Data Type Methods* are implemented as a PHP library.

We expect the specification of the PHP client to closely describe our next
release, including the unimplemented methods.  However, it is possible that
some changes to the client spec will occur.

## Persistent Connections

Initializing the C-client to connect to a specified cluster is a costly operation, so ideally the C-client should be reused for the multiple requests made against the same PHP process (as is the case for mod_php and fastCGI).

The PHP developer can determine whether the Aerospike class constructor will use persistent connections or not by way of an optional boolean argument.  After the first time Aerospike::__construct() is called within the process, the extension will attempt to reuse the persistent connection.

When persistent connections are used the methods _reconnect()_ and _close()_ do not actually close the connection.  Those methods only apply to instances of class Aerospike which use non-persistent connections.

## Halting a Stream

Halting a _query()_ or _scan()_ result stream can be done by returning (an explicit) boolean **false** from the callback.  The extension will capture the return value from the registered PHP callback, and pass it to the C-client.  The C-client will then close the sockets to the nodes involved in streaming results, effectively halting it.

## Handling Unsupported Types

See: [Data Types](http://www.aerospike.com/docs/guide/data-types.html)
See: [as_bytes.h](https://github.com/aerospike/aerospike-common/blob/master/src/include/aerospike/as_bytes.h)
* Allow the user to register their own serializer/deserializer method
 - OPT\_SERIALIZER : SERIALIZER\_PHP (default), SERIALIZER\_NONE, SERIALIZER\_USER
* when a write operation runs into types that do not map directly to Aerospike DB types it checks the OPT\_SERIALIZER setting:
 - if SERIALIZER\_NONE it returns an Aerospike::ERR\_PARAM error
 - if SERIALIZER\_PHP it calls the PHP serializer, sets the object's as\_bytes\_type to AS\_BYTES_PHP. This is the default behavior.
 - if SERIALIZER\_USER it calls the PHP function the user registered a callback with Aerospike::setSerializer(), and sets as\_bytes\_type to AS\_BYTES\_BLOB
* when a read operation extracts a value from an AS\_BYTES type bin:
 - if it’s a AS\_BYTES\_PHP use the PHP unserialize function
 - if it’s a AS\_BYTES\_BLOB and the user registered a callback with Aerospike::setDeserializer() call that function, otherwise place it in a PHP string

**There is a known problem** with PHP binary-strings and the default serializer
, because those may include
a null-byte (**\0**) anywhere in the string, not just at its end. One source for
such a crazy looking string is PHP's serialize() function. When you try to
serialize an object of a user-defined class the data gets delimited with a
null-byte. This is a terrifically short-sighted hack, but then ugly hacks are
seemingly a badge of courage in the PHP internals :grimacing:

So, if the binary-string does not include a null-byte at an odd position it will
get cast to an as\_string on writes and back to a PHP string on reads. If it
does contain a null-byte in an unfortunate position the string will get
truncated if you are using the (default) SERIALIZER\_PHP value for
OPT\_SERIALIZER. The workaround is to register your own serializer and
deserializer, which safely convert all data to and from an as\_bytes with type
AS\_BYTES\_BLOB.

### Example:

```php
ini_set('aerospike.serializer','user');
$client = new aerospike($config);
$client->setSerializer(function ($val) {
    return serialize($val);
});
$client->setDeserializer(function ($val) {
    return unserialize($val);
});
```

## Further Reading

- [How does the Aerospike client find a node](https://discuss.aerospike.com/t/how-does-aerospike-client-find-a-node/706)
- [How would hash collisions be handled](https://discuss.aerospike.com/t/what-will-aerospike-do-if-hash-collision-for-a-key/779)
