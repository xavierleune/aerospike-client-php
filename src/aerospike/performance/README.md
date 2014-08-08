# Aerospike PHP Performance Test

This is the Aerospike Performance Test.

## Documentation

Full documentation for the Aerospike PHP Performance test can be found in
`src/aeropsike/performance/`.Instructions are memtioned in `README.md` file
in the same directory.

## Source Code

The source code for the Aerospike PHP Client SDK is in the
`src/aerospike` directory.  Detailed instructions for regarding building
the Aerospike PHP Client SDK are in the file `src/README.md`.

## Build Instructions

Step 1:
    ## Build Aerospike PHP Client SDK:
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

Step:2

   ## RUN PERFORMANCE TEST FOR PUT:
   
   To run the performance test for put, run the following command:

   $ php benchmark_put.php <number_of_keys (greater than or equal to 500000)>
                           <number_of_bins_per_record>
                         
   For example:
   php benchmark_put.php 500000 5

   ## RUN PERFORMANCE TEST FOR GET:

   $ php benchmark_get.php <number_of_keys (greater than or equal to 500000)>

   For example:
   php benchmark_get.php 500000

## Cleanup

To clean up artifacts created by the build process you can do:

	$ build-cleanup

At this point if you want to use **make** you will need to rebuild.

