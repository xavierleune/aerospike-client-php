# Aerospike PHP Performance Test

This is the Aerospike Performance Test.

## Documentation

Full documentation for the Aerospike PHP Performance test can be found in
`src/aeropsike/performance/`.Instructions are memtioned in `README.md` file
in the same directory.

## Instructions to run performance

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

