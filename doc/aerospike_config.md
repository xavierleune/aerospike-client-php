
# Runtime Configuration

The following configuration options in php.ini

| Name  | Default  |
|:------|:---------:|
| aerospike.nesting_depth |  3  |
| aerospike.connect_timeout | 1000 |
| aerospike.read_timeout | 1000 |
| aerospike.write_timeout | 1000 |
| aerospike.key_policy | digest |
| aerospike.serializer | php |
| aerospike.lua_system_path | /opt/aerospike/client-php/sys-lua |
| aerospike.lua_user_path | /opt/aerospike/client-php/usr-lua |
| aerospike.use_shm | false |
| aerospike.shm_key | 0xA5000000 |
| aerospike.shm_max_nodes | 16 |
| aerospike.shm_max_namespaces | 8 |
| aerospike.shm_takeover_threshold_sec | 30 |

Here is a description of the configuration directives:

**aerospike.nesting_depth integer**
    The allowed nesting depth in associative arrays for write operations

**aerospike.connect_timeout integer**
    The connection timeout in milliseconds

**aerospike.read_timeout integer**
    The read timeout in milliseconds

**aerospike.write_timeout integer**
    The write timeout in milliseconds

**aerospike.key_policy string**
    Whether to send and store the record's (ns,set,key) data along with its (unique identifier) digest. One of { digest, send }

**aerospike.serializer string**
    The unsupported type handler. One of { php, user, none }

**aerospike.udf.lua_system_path string**
    Path to the system support files for Lua UDFs

**aerospike.udf.lua_user_path string**
    Path to the user-defined Lua function modules

**aerospike.use_shm boolean**
    Indicates if shared memory should be used for cluster tending. One of { true, false }

**aerospike.shm_key string**
    Shared memory identifier.

**aerospike.shm_max_nodes integer**
    Shared memory maximum number of server nodes allowed.

**aerospike.shm_max_namespaces integer**
    Shared memory maximum number of namespaces allowed.

**aerospike.shm_takeover_threshold_sec integer**
    Take over shared memory cluster tending if the cluster hasn't been tended by this threshold in seconds.

## See Also

### [Aerospike Class](aerospike.md)
