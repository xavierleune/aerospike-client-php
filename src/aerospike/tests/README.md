# Aerospike PHP Client PHPT Tests

## Source Code

PHPT scripts are grouped by functionality inside `src/aerospike/tests/`.
Each *.inc* file contains the methods used by individual *.phpt* within
the respective `src/aerospike/tests/phpt/*/` subdirectory.

For example, the Put test cases are described by the list of
`src/aerospike/tests/phpt/Put/*.phpt` files, which make use of methods from `src/aerospike/tests/Put.inc`.

## Expected Values
The structure of PHPT tests is explained at the [PHP QA site](http://qa.php.net/write-test.php#writing-phpt).

The expected value for a test is the name of one of the Aerospike class
constants, such as **OK** or **ERR_RECORD_NOT_FOUND** as listed in
`src/aerospike/tests/astestframework/astest-phpt-loader.inc`.

## Build Instructions

Follow the build and installation steps described in this repository's main [README.md](../../../README.md) file.  Please use a standard build (without the **-l** flag), as a debug build will cause the tests to fail.

## Configuration:

Edit the file `src/aerospike/tests/aerospike.inc` with the IP address and port configuration of your Aerospike database server(s) before running the phpt
scripts.

## Running Tests:

Change directory to `src/aerospike/` and run:

    $ make test TESTS=tests/phpt

To run only the phpt test cases for Put:

    $ make test TESTS=tests/phpt/Put


## Cleanup

To clean up artifacts created by the tests you can run:

    $ test-cleanup

