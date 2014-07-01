
# Runtime Configuration

The following configuration options in php.ini

| Name  | Default  |
|:------|:---------:|
| aerospike.nesting_depth |  3  |
| aerospike.connect_timeout | 1000 |
| aerospike.read_timeout | 1000 |
| aerospike.write_timeout | 1000 |
| aerospike.log_path | **NULL** |
| aerospike.log_level | **NULL** |

Here is a description of the configuration directives:

aerospike.nesting_depth integer
 The allowed nesting depth in associative arrays for write operations

aerospike.connect_timeout integer
 The connection timeout in milliseconds

aerospike.read_timeout integer
 The read timeout in milliseconds

aerospike.write_timeout integer
 The write timeout in milliseconds

aerospike.log_path string
 If defined acts as Aerospike::setLogger()

aerospike.log_level string
 One of LOG_LEVEL_OFF through LOG_LEVEL_TRACE as described in the Aerospike
 class. Example: aerospike.log_level = LOG_LEVEL_WARN

## See Also

### [Aerospike Class](aerospike.md)
