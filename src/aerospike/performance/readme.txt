TO RUN PERFORMANCE TEST FOR PUT:

$ php benchmark_put.php <number_of_keys (greater than or equal to 500000)> <number_of_bins_per_record>

For example:
php benchmark_put.php 500000 5


TO RUN PERFORMANCE TEST FOR GET:

$ php benchmark_get.php <number_of_keys (greater than or equal to 500000)>

For example:
php benchmark_get.php 500000

