
# Aerospike::jobInfo

Aerospike::jobInfo - gets the status of a background job triggered by
scanApply() or queryApply()

## Description

```
public int Aerospike::jobInfo ( integer $job_id, array &$info [, array $options ] )
```

**Aerospike::jobInfo()** will return *information* on a background job, identified
by *job_id*, which was triggered using **Aerospike::scanApply()** or
**Aerospike::queryApply()**.

## Parameters

**job_id** the job id

**info** the status of the background job returned as an array conforming to the following:
```
Associative Array:
  progress_pct => progress percentage for the job
  records_read => number of records read by the job
  status => one of Aerospike::STATUS_*
```

**[options](aerospike.md)** including
- **Aerospike::OPT_READ_TIMEOUT**

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## See Also

- [Aerospike::scanApply()](aerospike_scanapply.md)
- [Aerospike::queryApply()](aerospike_queryapply.md)

## Examples

```php
<?php

$config = ["hosts" => [["addr"=>"localhost", "port"=>3000]], "shm"=>[]];
$client = new Aerospike($config, true);
if (!$client->isConnected()) {
   echo "Aerospike failed to connect[{$client->errorno()}]: {$client->error()}\n";
   exit(1);
}

$poll = true;
while($poll) {
    $status = $client->jobInfo(1, $info);
    if ($status == Aerospike::OK) {
        var_dump($info);
        if ($info["status"] == Aerospike::STATUS_COMPLETED) {
            echo "Background job is complete!";
            $poll = false;
        }
    } else {
        echo "An error occured while retrieving info of job [{$client->errorno()}] {$client->error()}\n";
        $poll = false;
    }
}

?>
```

We expect to see:

```
array(3) {
  ["progress_pct"]=>
  int(70)
  ["records_read"]=>
  int(1000)
  ["status"]=>
  int(1)
}
```

