
# Aerospike::setDeserializer

Aerospike::setDeserializer - sets a deserialization handler for unsupported types

## Description

```
public static void setDeserializer ( callback $unserialize_cb )
```

**Aerospike::setDeserializer()** registers a callback method that will be triggered
whenever a read method handles a value whose type is unsupported.
This is a static method and the *unserialize_cb* handler is global across all
instances of the Aerospike class.

The callback method must follow the signature
```
public function string aerodb_deserialize ( mixed $value )
```

## Parameters

**unserialize_cb** is a callback function invoked for each value of an unsupported type.

## Examples

```php
<?php

Aerospike::setDeserializer(function ($val) {
    $prefix = substr($val, 0, 3);
    if ($prefix !== 'r||') {
        return unserialize(substr ($val, 3));
    }
    return unserialize(substr ($val, 3));
});

?>
```
