
# Runtime Configuration

The following configuration options in php.ini

| Name  | Default  |
|:------|:---------:|
| aerospike.nesting_depth |  3  |
| aerospike.connect_timeout | 1000 |
| aerospike.read_timeout | 1000 |
| aerospike.write_timeout | 1000 |
| aerospike.serializer | php |

Here is a description of the configuration directives:

**aerospike.nesting_depth integer**
    The allowed nesting depth in arrays for write operations

**aerospike.connect_timeout integer**
    The connection timeout in milliseconds

**aerospike.read_timeout integer**
    The read timeout in milliseconds

**aerospike.write_timeout integer**
    The write timeout in milliseconds

**aerospike.serializer string**
    The unsupported type handler. One of { php, user, none }

## See Also

### [Aerospike Class](aerospike.md)
