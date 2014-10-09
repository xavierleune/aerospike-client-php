
# Aerospike::getThroughput \[to be implemented\]

Aerospike::getThroughput - get throughput of the cluster

## Description

```
public int Aerospike::getThroughput( array &$throughput [, array $config [, string $histogram=<all> [, int $back=<minimum to get last slice> [, int $duration=0 [, int $slice=0 [, array $options ]]]]]] )
```

**Aerospike::getThroughput()** will get throughput of the database.
The throughput will be returned in the *throughput* variable, otherwise it will be an empty array.

## Parameters

**throughput** filled by an associative array of throughput

**config** an associative array holding the cluster connection information. One
node or more (for failover) may be defined. Once a connection is established to
a node of the Aerospike DB the client will retrieve the full list of nodes in the
cluster and manage its connections to them.

- *hosts* an array of host data
  - *addr* hostname or IP of the node
  - *port*
- *user*
- *pass*

**histogram** Histogram name, one of "reads", "writes_master", "writes_reply", "udf", "query". (default: all histograms)

**back** Start search this many seconds before now. (default: Minimum to get last slice)

**duration** seconds (forward) from start to search. (default: 0, everything to present)

**slice** Intervals (in seconds) to analyze. (default:  0, everything as one slice)

**[options](aerospike.md)** including
- **Aerospike::OPT_READ_TIMEOUT**

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Examples

```php
<?php


$config = array("hosts"=>array(array("addr"=>"192.168.1.10", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$res = $db->getThroughput($throughput, $config);
if ($res == Aerospike::OK) {
    var_dump($throughput);
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
array(1) {
  [0]=>
  array(8) {
    ["addr"]=>
    string(12) "192.168.1.10"
    ["port"]=>
    string(4) "3000"
    ["reads"]=>
    array(2) {
        ["ops/sec"]=>
            float(0.0)
        ["timestamp"]=>
            string(8) "09:54:46"
    }
    ["writes_master"]=>
    array(2) {
        ["ops/sec"]=>
            float(0.0)
        ["timestamp"]=>
            string(8) "09:54:46"
    }
    ["proxy"]=>
    array(2) {
        ["ops/sec"]=>
            float(0.0)
        ["timestamp"]=>
            string(8) "09:54:46"
    }
    ["writes_reply"]=>
    array(2) {
        ["ops/sec"]=>
            float(0.0)
        ["timestamp"]=>
            string(8) "09:54:46"
    }
    ["udf"]=>
    array(2) {
        ["ops/sec"]=>
            float(0.0)
        ["timestamp"]=>
            string(8) "09:54:46"
    }
    ["query"]=>
    array(2) {
        ["ops/sec"]=>
            float(0.0)
        ["timestamp"]=>
            string(8) "09:54:46"
    }
  }
}
```

