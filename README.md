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

This will set the `CLIENTREPO_3X` environment variable, add the
`scripts` directory to the path, and change to the `src/aerospike`
directory.  Next, to build the Aerospike PHP Zend extension, do:

	$ build

The Zend extension will be built as: `modules/aerospike.so`

## Running Instructions

To run the Aerospike PHP Client interactively after building, do:

	$ php -dextension=modules/aerospike.so -a
