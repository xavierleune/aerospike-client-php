# Aerospike PHP Client Performance Test

This is a performance test for Aerospike PHP client.

## Documentation

Full documentation for the Aerospike PHP Performance test can be found in
`src/aeropsike/performance/`. Instructions are memtioned in `README.md` file
in the same directory.

## Instructions to run performance

To run the performance test for put, run the following command:

    $ php benchmark.php [-h<HOST IP ADDRESS>|--host=<HOST IP ADDRESS>                       (default = localhost)
                         -p<HOST PORT NUMBER>|--port=<HOST PORT NUMBER>                     (default = 3000)
                         -n<NAMESPACE NAME>|--namespace=<NAMESPACE NAME>                    (default = test)
                         -s<SET NAME>|--set=<SET NAME>                                      (default = demo)
                         -w<WORKLOAD TYPE=R,W or RW>|--workload=<WORKLOAD TYPE=R,W or RW>   (default = RW)
                         [for RW, you can optionally specify R:W ratio by
                         specifying the r value, for instance -wRW,50 or
                         --workload=RW,100                                                  (default r:w = 80/20])
                         -o|--once                                                          (default = Executes indefinitely until Ctrl+C is pressed)
                         -k<NO. OF KEYS>|--keys<NO. OF KEYS>                                (default = 1000000)
                         -r</path/for/report/file>|--report=</path/for/report/file>         (default = php://stdout)
                         -u|--usage]

# Example 1: 

    To perform a total of 100000 reads and writes randomly in the ratio of 80:20 
    on default namespace 'test' and default set 'demo',
    execute all the operations once and quit, generating the report in an arbitrary path,
    use the following:

        $ php benchmark.php -h"localhost" -p3000 -wRW -o -k100000 -r"/tmp/benchmark_report.log" OR
        $ php benchmark.php --host="localhost" --port=3000 --workload=RW --once --keys=100000 --report="/tmp/benchmark_report.log"

# Example 2: 

    To perform a total of 100000 reads, execute the operations infinitely until externally stopped using "Ctrl+C",
    on namespace 'my_namespace' and set 'my_set'
    generating the report on the console, use the following:

        $ php benchmark.php -wR -k100000 -n'my_namespace -s'my_set' OR
        $ php benchmark.php --workload=R --keys=100000 --namespace='my_namespace --set='my_set'

# Example 3:

    To perform a total of 100000 reads and writes randomly in the ratio of 50:50 
    on default namespace 'test' and default set 'demo',
    execute all the operations once and quit, generating the report on the console,
    use the following:

        $ php benchmark.php -h"localhost" -p3000 -wRW,50 -o -k100000 OR
        $ php benchmark.php --host="localhost" --port=3000 --workload=RW,50 --once --keys=100000

