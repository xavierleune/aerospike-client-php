
# The Aerospike Large Set class

The AerospikeLSet class extends the abstract class AerospikeLDT from which it
inherits the common LDT functionality.

```php

AerospikeLSet extends AerospikeLDT
{
    public __construct ( Aerospike $db, array $key, string $bin )
    public int add ( mixed $val )
    public int exists ( mixed $val, boolean &$found )
    public int remove ( mixed $val )
    public int scan ( array &$elements )

    // To be implemented:
    // public int filter ( string $module, string $function, mixed $args, array &$elements )
    // public int get ( mixed $val, mixed &$element )

    /* Inherited Methods */
    public boolean isLDT ( void )
    public string error ( void )
    public int errorno ( void )
    public int size ( int &$num_elements )
    public int destroy ( void )
    public int getCapacity ( int &$num_elements )
    public int setCapacity ( int $num_elements )
}
```

### [Aerospike Class](aerospike.md)
### [Aerospike LDT Abstract Class](aerospike_ldt.md)

