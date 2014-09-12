
# Aerospike::__construct

Aerospike::__construct - constructs a new Aerospike object

## Description

```
public int Aerospike::__construct ( array $config [, boolean $persistent_connection = true [, array $options]] )
```

**Aerospike::__construct()** will create an Aerospike object and connect to the
cluster defined in *config*.  The **Aerospike::isConnected()** method can be used
to test whether the connection succeeded. If a config or connection error
occured the **Aerospike::error()** and **Aerospike::errorno()** methods can be used
to inspect it.

The Aerospike class instance should use persistent connections.  This allows for
reduced overhead on initializing the cluster and keeping track of the state of
its nodes.  Subsequent instantiation calls will attempt to reuse the connection.

## Parameters

**config** an array holding the cluster connection information. One
node or more (for failover) may be defined. Once a connection is established to
a node of the Aerospike DB the client will retrieve the full list of nodes in the
cluster and manage its connections to them.

- *hosts* an array of host data
  - *addr* hostname or IP of the node
  - *port*
- *user*
- *pass*

**persistent_connection** whether the C-client will persist between requests.

**options**: set one or more of the following *OPT_\** constants
  **OPT_CONNECT_TIMEOUT**, **OPT_READ_TIMEOUT**, **OPT_WRITE_TIMEOUT** as
default values.

## See Also

- [Aerospike::isConnected()](aerospike_isconnected.md)

The following only apply to instances created with non-persistent connections:

- [Aerospike::close()](aerospike_close.md)
- [Aerospike::reconnect()](aerospike_reconnect.md)

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$opts = array(Aerospike::OPT_CONNECT_TIMEOUT => 1250, Aerospike::OPT_WRITE_TIMEOUT => 1500);
$db = new Aerospike($config, true, $opts);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

?>
```

On error we expect to see:

```
Aerospike failed to connect[300]: failed to initialize cluster
```

