
# Runtime Configuration

The following configuration options in php.ini

| Name  | Default  |
|:------|:---------:|
| aerospike.nesting_depth |  3  |
| aerospike.connect_timeout | 1000 |
| aerospike.read_timeout | 1000 |
| aerospike.write_timeout | 1000 |
| aerospike.serializer | php |
| aerospike.lua_system_path | /opt/aerospike/client-php/sys-lua |
| aerospike.lua_user_path | /opt/aerospike/client-php/usr-lua |

Here is a description of the configuration directives:

**aerospike.nesting_depth integer**
    The allowed nesting depth in associative arrays for write operations

**aerospike.connect_timeout integer**
    The connection timeout in milliseconds

**aerospike.read_timeout integer**
    The read timeout in milliseconds

**aerospike.write_timeout integer**
    The write timeout in milliseconds

**aerospike.serializer string**
    The unsupported type handler. One of { php, user, none }

**aerospike.udf.lua_system_path string**
    Path to the system support files for Lua UDFs

**aerospike.udf.lua_user_path string**
    Path to the user-defined Lua function modules

## See Also

### [Aerospike Class](aerospike.md)
