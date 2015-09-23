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
`llist.php` gives an example of large ordered list (LList) operations.

```bash
php llist.php --host=192.168.119.3 -a -c
```
