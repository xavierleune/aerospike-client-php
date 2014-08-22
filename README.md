# Aerospike PHP Client SDK

This is the Aerospike PHP Client SDK. It is intended for PHP developers who
wish to build applications that make use of the Aerospike DB.

## Documentation

Full documentation for the Aerospike PHP Client SDK may be found in the
[doc directory](doc/README.md).  The API reference described there is the external
specification for the [Aerospike PHP Client API](doc/aerospike.md).  Notes on the
internals of the implementation are in [doc/internals.md](doc/internals.md).
Example PHP code can be found in [src/aerospike/examples/basic_examples](src/aerospike/examples/basic_examples).

## Build Instructions

To build the PHP extension, you must first set up your environment using a
script which will download the Aerospike C-client SDK if necessary
(into `aerospike-client-c/`):

    $ . scripts/setup

This will set the `CLIENTREPO_3X` environment variable, add the `scripts/` 
directory to the path, and change the current directory to `src/aerospike/`.
To grab the latest release of the C-client SDK (rather than a specified
version) modify the `AEROSPIKE_C_CLIENT` variable of the setup script
to *latest*.  You may need to remove the `aerospike-client-c` directory before
running the setup script once more.

Next, build the Aerospike PHP extension.

    $ build

For a debug build specify the log level:

    $ build [-l|--loglevel <Desired log level: one among TRACE, DEBUG, INFO, WARN, ERROR, OFF>] [--help]

The PHP extension will be built as `modules/aerospike.so`

## Confirming The Build

To test the Aerospike PHP Client interactively after building, do:

    $ php -dextension=modules/aerospike.so -a

## Installing The PHP Extension

To install the PHP extension do:

    $ sudo make install

Then edit the aerospike.ini file which is usually in `/etc/php.d/`:

    extension=aerospike.so

The *aerospike* module should now be available to the PHP CLI:

    $ php -m | head -5
    [PHP Modules]
    aerospike
    bz2
    calendar
    Core

## Cleanup

To clean up artifacts created by the build process you can do:

    $ build-cleanup

At this point if you want to use **make** you will need to rebuild.

