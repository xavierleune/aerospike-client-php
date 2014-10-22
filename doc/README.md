
# Overview

The Aerospike PHP client API is described in the following sections:

### [Runtime Configuration](aerospike_config.md)
### [Aerospike Class](aerospike.md)
### [Lifecycle and Connection Methods](apiref_connection.md)
### [Error Handling Methods](apiref_error.md)
### [Key-Value Methods](apiref_kv.md)
### [Query and Scan Methods](apiref_streams.md)
### [User Defined Methods](apiref_udf.md)
### [Admin Methods](apiref_admin.md)
### [Large Data Type Methods](aerospike_ldt.md)

## Implementation Status
So far the *Runtime Configuration*, *Lifecycle and Connection Methods*, *Error*
*Handling and Logging Methods*, *Query and Scan Methods*, *User Defined Methods*
, parts of *Key-Value Methods* and *Admin Methods* have been implemented.

The *Large Data Type Methods* are implemented as a PHP library.

We expect the specification of the PHP client to closely describe our next
release, including the unimplemented methods.  However, it is possible that
some changes to the client spec will occur.

# Persistent Connections

Initializing the C-client to connect to a specified cluster is a costly operation, so ideally the C-client should be reused for the multiple requests made against the same PHP process (as is the case for mod_php and fastCGI).

The PHP developer can determine whether the Aerospike class constructor will use persistent connections or not by way of an optional boolean argument.  After the first time Aerospike::__construct() is called within the process, the extension will attempt to reuse the persistent connection.

When persistent connections are used the methods _reconnect()_ and _close()_ do not actually close the connection.  Those methods only apply to instances of class Aerospike which use non-persistent connections.

# Halting a Stream

Halting a _query()_ or _scan()_ result stream can be done by returning (an explicit) boolean **false** from the callback.  The extension will capture the return value from the registered PHP callback, and pass it to the C-client.  The C-client will then close the sockets to the nodes involved in streaming results, effectively halting it.

# Handling Unsupported Types

See: [Data Types](http://www.aerospike.com/docs/guide/data-types.html)
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

