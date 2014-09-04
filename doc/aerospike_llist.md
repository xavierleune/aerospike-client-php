
# The Aerospike Large Ordered List class

The AerospikeLList class extends the abstract class AerospikeLDT from which it
inherits the common LDT functionality.

```php

AerospikeLList extends AerospikeLDT
{
    public __construct ( Aerospike $db, array $key, string $bin )
    public int add ( mixed $val )
    public int addMany ( array $vals )
    public int find ( mixed $val, array &$elements )
    public int remove ( mixed $val )
    public int scan ( array &$elements [, int|string $min = null [, int|string $max = null]] )

    // To be implemented:
    // public int filter ( string $module, string $function, mixed $args, array &$elements [, int|string $min = null [, int|string $max = null ]] )

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

