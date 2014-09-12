
# AerospikeLDT::__construct

AerospikeLDT::__construct - parent constructor for all AerospikeLDT subclasses.

## Description

```
public int AerospikeLDT::__construct ( Aerospike $db, array $key, string $bin, int $type )
```

**AerospikeLDT::__construct()** is the constructor of the AerospikeLDT abstract
class and used by the concrete subclasses.

**AerospikeLDT::error()** and **AerospikeLDT::errorno()** methods can be used
to inspect whether an error occured at instantiation.

## Parameters

**db** the client instance created with Aerospike::__construct()

**key** the key under which the record is found. An array with keys 'ns','set','key'.

**bin** the name of the bin.

**type**: one of the AerospikeLDT LDT type constants.

