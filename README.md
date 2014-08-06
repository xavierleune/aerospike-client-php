# Aerospike PHP Client SDK

This is the Aerospike PHP Client SDK.

## Documentation

Full documentation for the Aerospike PHP Client SDK may be found in the
[doc directory](doc/README.md).  The API reference described there is the external
specification for the [Aerospike PHP Client API](doc/aerospike.md).  The internals of the
implementation are in [doc/internals.md](doc/internals.md).

## Source Code

The source code for the Aerospike PHP Client SDK is in the
`src/aerospike` directory.  Detailed instructions for regarding building
the Aerospike PHP Client SDK are in the file `src/README.md`.

## Build Instructions

To build the Aerospike PHP Client SDK, you must first set up the
environment, which will download the Aerospike C Client SDK if
necessary:

	$ . scripts/setup

This will set the `CLIENTREPO_3X` environment variable, add the `scripts` 
directory to the path, and change to the `src/aerospike` directory.
To grab the latest release of the C-client SDK modify the `AEROSPIKE_C_CLIENT`
variable to *latest*.

Next, to build the Aerospike PHP extension, do:

	$ build

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

