# Basic Examples

### Record Operations
`rec-operations.php` gives an example of record level operations such as get(),
remove(), touch(), exists(), and put() with various policy options set
(Aerospike::OPT\_POLICY\_GEN, Aerospike::OPT\_POLICY\_EXISTS). An example of
identifying a record using its digest (the server's hash of the key) is given.

```bash
php rec-operations.php --host=192.168.119.3 -a -c
```

### Bin Operations
`bin-operations.php` gives an example of modifying a record with bin operations
such as prepend(), append(), increment(), and removeBin().

```bash
php bin-operations.php --host=192.168.119.3 -a -c
```

### Applying a Record UDF
`udf-operations.php` shows the effect of applying two different record UDFs to a
record's bin.

```bash
php udf-operations.php --host=192.168.119.3 -a -c
```
