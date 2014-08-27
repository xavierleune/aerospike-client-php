# Aerospike PHP Client Basic Examples

The examples will connect to the Aerospike server and work against the *test*
namespace.

## Common Flags

 - **-h** or **--host=** set the host IP address.
 - **-p** or **--port=** set the host port.
 - **-a** or **--annotate** display the code used for each section of the
   examples.  If the [Pygments](http://pygments.org/docs/cmdline/) binary **pygmantize** is found, it will be used to
   provide syntax highlighting.
 - **-c** or **--clean** will remove the data inserted by the example script.

## Examples

    $ php rec-operations.php --host=192.168.119.3 -a -c
    $ php bin-operations.php --host=192.168.119.3 -a -c

