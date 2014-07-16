
# Aerospike::setSerializer

Aerospike::setSerializer - sets a serialization handler for unsupported types

## Description

```
public static void setSerializer ( callback $serialize_cb )
```

**Aerospike::setSerializer()** registers a callback method that will be triggered
whenever a write method handles a value whose type is unsupported.
This is a static method and the *serialize_cb* handler is global across all 
instances of the Aerospike class.

The callback method must follow the signature
```
public function string aerodb_serialize ( mixed $value )
```

## Parameters

**serialize_cb** is a callback function invoked for each value of an unsupported type.

## Examples

```php
<?php

Aerospike::setSerializer(function ($val) {
    if (is_bool ($val)) {
        return "b||". serialize($val);
    }
    if (is_object ($val)) {
        return "o||". serialize($val);
    }
    // otherwise, mark it as raw
    return "r||". $val;
});

?>
```
