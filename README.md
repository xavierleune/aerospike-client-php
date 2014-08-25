# Aerospike PHP Client

## Documentation

Documentation of the Aerospike PHP Client may be found in the
[doc directory](doc/README.md).  The API described there is the
[specification](doc/aerospike.md) for the PHP Client.  Notes on the
internals of the implementation are in [doc/internals.md](doc/internals.md).

Example PHP code can be found in [examples/basic_examples/](examples/basic_examples).

Full documentation of the Aerospike database is available at http://www.aerospike.com/docs/

## Build Instructions

To build the PHP extension, you must first invoke the setup script, which will
download the Aerospike C client SDK if necessary (into `aerospike-client-c/`):

    $ . scripts/setup

This will set the `CLIENTREPO_3X` environment variable, add the `scripts/` 
directory to your path, and change the working directory to `src/aerospike/`.
To grab the latest release of the C client SDK (rather than a specified
version) modify the `AEROSPIKE_C_CLIENT` variable of the setup script
to *latest*. You may need to remove the `aerospike-client-c` directory before
running the setup script once more.

Next, build the Aerospike PHP extension.

    $ build

For a debug build specify the log level (default being OFF):

    $ build [-l|--loglevel <Desired log level: one among TRACE, DEBUG, INFO, WARN, ERROR, OFF>] [--help]

The PHP extension will be built as `modules/aerospike.so`

## Confirming The Build

To test the Aerospike PHP Client interactively after building, do:

    $ php -dextension=modules/aerospike.so -a

## Installing The PHP Extension

To install the PHP extension do:

    $ sudo make install

Then edit the aerospike.ini file, which is usually in `/etc/php.d/`:

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

## License

The Aerospike PHP Client is made availabled under the terms of
the Apache License, Version 2, as stated in the file `LICENSE`.

Individual files may be made available under their own specific license,
all compatible with Apache License, Version 2. Please see individual files for
details.
