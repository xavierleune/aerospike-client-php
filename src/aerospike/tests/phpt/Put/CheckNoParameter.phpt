--TEST--
Put - No parameter

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testPUTNoParameter");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPUTNoParameter");
--EXPECT--
Fatal error: Uncaught exception 'ErrorException' with message 'Aerospike::put() expects exactly 2 parameters, 0 given' in /home/gslab/aerospike-client-php/src/aerospike/tests/Put.inc:76
Stack trace:
#0 [internal function]: {closure}(2, 'Aerospike::put(...', '/home/gslab/aer...', 76, Array)
#1 /home/gslab/aerospike-client-php/src/aerospike/tests/Put.inc(76): Aerospike->put()
#2 [internal function]: Put->testPUTNoParameter()
#3 /home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/ASTestFramework.inc(201): ReflectionMethod->invoke(Object(Put))
#4 /home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/astest-phpt-loader.inc(19): ASTestFramework->runSingleTest('testPUTNoParame...')
#5 /home/gslab/aerospike-client-php/src/aerospike/tests/phpt/Put/CheckNoParameter.php(3): aerospike_phpt_runtest('Put', 'testPUTNoParame...')
#6 {main}
  thrown in /home/gslab/aerospike-client-php/src/aerospike/tests/Put.inc on line 76
