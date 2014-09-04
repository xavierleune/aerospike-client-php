
# The Aerospike Large Map class

The AerospikeLMap class extends the abstract class AerospikeLDT from which it
inherits the common LDT functionality.

```php

AerospikeLMap extends AerospikeLDT
{
    public __construct ( Aerospike $db, array $key, string $bin )
    public int put ( int|string $key, mixed $val )
    public int putMany ( array $key_vals )
    public int get ( int|string $key, mixed &$element )
    public int remove ( int|string $key )
    public int scan ( array &$elements )

    // To be implemented:
    // public int filter ( string $module, string $function, mixed $args, array &$elements )

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

