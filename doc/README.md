
# Overview

The Aerospike <a href="http://www.aerospike.com/docs/architecture/clients.html"
target="_doc">client</a> for PHP enables your application to work with an
Aerospike <a href="http://www.aerospike.com/docs/architecture/distribution.html"
target="_doc">cluster</a> as its
<a href="http://www.aerospike.com/docs/guide/kvs.html" target="_doc">key-value store</a>.

The <a href="http://www.aerospike.com/docs/architecture/data-model.html" target="_doc">Data Model</a>
document gives further details on how data is organized in the cluster.

## Client API
The client API is described in the following sections:

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

## Configuration in a Web Server Context

Initializing the client to a cluster is a costly
operation, so ideally it should be reused for the multiple requests
that the same PHP process will handle (as is the case for mod\_php and fastCGI).

The developer can determine whether the constructor will
use persistent connections by way of an optional boolean argument.
After the first time Aerospike::\_\_construct() is called within the process, the
client will be stored for reuse by subsequent requests, if persistent connections
were indicated. With persistent connections, the methods _reconnect()_ and
_close()_ do not actually close the connection.

Each Aerospike client opens 2*N connections to the cluster, where N is the number
of nodes in that cluster, and a connection simply costs a file descriptor
on the server-side. Depending on the number of web servers and PHP processes on
each server, you may need to adjust the
[proto-fd-max](http://www.aerospike.com/docs/reference/configuration/#proto-fd-max)
server config parameter and the OS limits to account for the necessary number of
[file descriptors](http://www.aerospike.com/docs/operations/troubleshoot/startup/#not-enough-file-descriptors-error-in-log).
On the client side, the web server should be configured to reduce the frequency
in which new clients are created. Historically, the `max_requests` for mod\_php
and FPM was set low to combat memory leaks. PHP has been stable on memory for a
long while, so the correct configuration would be to have fewer processes, and
let each of them handle a high number of requests. This reduces the process
initialization overhead, and the overall number of connections on the web
server. Monitoring the memory consumption of the PHP processes allows the
`max_requests` to be raised safely to an efficient, stable value.

The client keeps track of changes at the server-side through a
[cluster tending](http://www.aerospike.com/docs/architecture/clustering.html)
thread. In a web server context, a single client can handle cluster tending and
share its state through a shared-memory segment. To enable shm cluster tending,
the developer can set the [`aerospike.shm.use`](aerospike_config.md) ini config
to `true`, or at the [constructor](aerospike_construct.md) through its config.

## Halting a Stream

Halting a _query()_ or _scan()_ result stream can be done by returning (an
explicit) boolean **false** from the callback.
The client will then close the sockets to the nodes of the cluster, halting the
stream of records.

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

**Warning:** Strings in PHP are a binary-safe structure that allows for the
null-byte (**\0**) to be stored inside the string, not just at its end.
Binary-strings with this characteristic are created by calling functions such
as serialize() and gzdeflate(). As a result, the Aerospike client may truncate
the resulting strings. On the Aerospike server, strings are a data type that can
be queried using a secondary index, while bytes are a data type that is only
used for storage. The developer should wrap binary-string with an object to
distinguish them. This allows the serializer to behave in the correct manner.

### Example:
```php
require('autoload.php');
$client = new Aerospike(['hosts'=>[['addr'=>'127.0.0.1', 'port'=>3000]]]);

$str = 'Glagnar\'s Human Rinds, "It\'s a bunch\'a munch\'a crunch\'a human!';
$deflated = new \Aerospike\Bytes(gzdeflate($str));
$wrapped = new \Aerospike\Bytes("trunc\0ated");

$key = $client->initKey('test', 'demo', 'wrapped-bytes');
$status = $client->put($key, ['unwrapped'=>"trunc\0ated", 'wrapped'=> $wrapped, 'deflated' => $deflated]);
if ($status !== Aerospike::OK) {
    die($client->error());
}
$client->get($key, $record);
$wrapped = \Aerospike\Bytes::unwrap($record['bins']['wrapped']);
$deflated = $record['bins']['deflated'];
$inflated = gzinflate($deflated->s);
echo "$inflated\n";
echo "wrapped binary-string: ";
var_dump($wrapped);
$unwrapped = $record['bins']['unwrapped'];
echo "The binary-string that was given to put() without a wrapper: $unwrapped\n";

$client->close();
```
Outputs:
```
Glagnar's Human Rinds, "It's a bunch'a munch'a crunch'a human!
wrapped binary-string: string(10) "truncated"
The binary-string that was given to put() without a wrapper: trunc
```

## Implementation Status
So far the *Runtime Configuration*, *Lifecycle and Connection Methods*, *Error*
*Handling and Logging Methods*, *Query and Scan Methods*, *User Defined Methods*
, *Admin Methods*, *Info Methods* and *Key-Value Methods* have been implemented.

The *Large Data Type Methods* are implemented as a PHP library.

Aerospike 3.7.x compatible list operations have been added to version 3.4.7 of the client.
Geospatial data types, indexing, and quries are in-progress.

PHP 5.3.3 - 5.6.x is supported by this client. A new PHP 7 compatible extension
is in progress.

## Further Reading

- [How does the Aerospike client find a node](https://discuss.aerospike.com/t/how-does-aerospike-client-find-a-node/706)
