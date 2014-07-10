
# Runtime Configuration

The following configuration options in php.ini

| Name  | Default  |
|:------|:---------:|
| aerospike.nesting_depth |  3  |
| aerospike.connect_timeout | 1000 |
| aerospike.read_timeout | 1000 |
| aerospike.write_timeout | 1000 |

Here is a description of the configuration directives:

**aerospike.nesting_depth integer**
    The allowed nesting depth in associative arrays for write operations

**aerospike.connect_timeout integer**
    The connection timeout in milliseconds

**aerospike.read_timeout integer**
    The read timeout in milliseconds

**aerospike.write_timeout integer**
    The write timeout in milliseconds

## See Also

### [Aerospike Class](aerospike.md)
