# Aerospike PHPT TEST CASES

This is the Aerospike PHPT Test Cases.

## Documentation

Full documentation for the Aerospike PHPT Test Cases may be found in the
tests/README.md file.

## Source Code

The source code for the Aerospike PHPT Test Cases is in the
`aerospike-client-php/src/aerospike/tests/` directory.  Detailed instructions
for building the Aerospike PHPT Test Cases are in the file 
`aerospike/tests/README.md`.

Brief Description of the Configuration and Scripts details:

Configuration:
Provision is supported for giving IP addresses and ports for the test scripts to
run. You must update the file `tests/aerospike.inc` with your IP address and
ports before starting the phpt scripts.

PHPT Scripts for each functionality test are located in `src/aerospike/tests/`
folder. You can find `.inc` file with respect to each functionality in the above
folder. These `.inc` files contains the scripts for the test cases.
For example if you want to have a look on the Put function test cases you can
look into `tests/Put.inc` file for the reference.
`ASTestFramework` is the base class for the test run. Abstract class
`AerospikeTestCommon` extends the `ASTestFramework` class` for its functinality.

## PHPT instructions
With respect to each function in .inc file, there is a phpt file which takes and
run the test case.These phpt files can be found in the
directory `src/aerospike/tests/phpt`.Inside `phpt` directory you can found 
sub-directories named with a particular functionality. These sub-directories
then contians all the phpt files with respect to that operation.
For each function added in `.inc` file you should have a phpt file
refering to that function.
You must mention your expected output under the heading `--EXPECT--`
in each phpt file.

All the functions returned value are mapped with a PHP status in the array
$status_codes.This array is defined in the file
`tests/astestframework/astest-phpt-loader.inc`.

## Build Instructions

   Step 1:
   Build Aerospike PHP Client SDK
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

Step 2:

Before you started with the phpt you should first configure the
`tests/aerospike.inc` with the appropriate IP and Ports details.

Step 3:

   Before running the phpt test cases you must disable all the debugs and logs.

a) To run the phpt test case for any specific operation go to `src/aerospike/` and run
   the below command:-
       make test TESTS=tests/phpt/Operation_Name

   For example if you want to run the phpt test cases for Put operation, you must
   run the below command:

       make test TESTS=tests/phpt/Put

b) To run the phpt for all the operationi, run the below command:-

       make test TESTS=tests/phpt

## Cleanup

To clean up artifacts created by the build process you can do:

	$ build-cleanup

At this point if you want to use **make** you will need to rebuild.

