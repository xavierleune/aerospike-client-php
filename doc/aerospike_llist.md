
# The Aerospike Large Ordered List class

Large Ordered List (llist) is optimized for searching and updating sorted
lists. It can access data at any point in the collection, while still being
capable of growing the collection to virtually any size.

```php

\Aerospike\LDT\LList extends \Aerospike\LDT
{
    public __construct ( Aerospike $db, array $key, string $bin )
    public int add ( int|string|array $value )
    public int addMany ( array $values )
    public int update ( int|string|array $value )
    public int updateMany ( array $values )
    public int find ( int|string $value, array &$elements )
    public int remove ( int|string $value )
    public int removeRange ( int|string $min, int|string $max )
    public int removeMany ( array $values )
    public int scan ( array &$elements [, int|string $min=null [, int|string $max=null [, string $module=null [, string $function=null [, array $args=array()]]]]] )

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

