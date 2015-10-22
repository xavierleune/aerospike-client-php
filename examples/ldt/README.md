# LDT Examples
You must make sure that the 'test' namespace has LDT enabled before running the
example scripts. In the aerospike.conf
[configuration file](http://www.aerospike.com/docs/guide/ldt.html):
```
namespace test {
  ldt-enabled true
}
```

### Large Ordered List Operations
`string_elements.php` gives an example of large ordered list (LList) operations
on an LList with elements of type string.

```bash
php string_elements.php --host=192.168.119.3 -a -c
```

`integer_elements.php` gives an example of large ordered list (LList) operations
on an LList with elements of type integer.

```bash
php integer_elements.php --host=192.168.119.3 -a -c
```

`map_elements.php` gives an example of large ordered list (LList) operations
on an LList with elements of type map.

```bash
php map_elements.php --host=192.168.119.3 -a -c
```

