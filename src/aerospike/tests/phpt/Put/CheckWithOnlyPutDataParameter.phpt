--TEST--
Put - Without key parameter.Only data parameter is available.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testPUTWithOnlyDataParameter");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPUTWithOnlyDataParameter");
--EXPECT--
Fatal error: Uncaught exception 'ErrorException' with message 'Aerospike::put() expects exactly 2 parameters, 1 given' in /home/gslab/aerospike-client-php/src/aerospike/tests/Put.inc:122
Stack trace:
#0 [internal function]: {closure}(2, 'Aerospike::put(...', '/home/gslab/aer...', 122, Array)
#1 /home/gslab/aerospike-client-php/src/aerospike/tests/Put.inc(122): Aerospike->put(Array)
#2 [internal function]: Put->testPUTWithOnlyDataParameter()
#3 /home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/ASTestFramework.inc(201): ReflectionMethod->invoke(Object(Put))
#4 /home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/astest-phpt-loader.inc(19): ASTestFramework->runSingleTest('testPUTWithOnly...')
#5 /home/gslab/aerospike-client-php/src/aerospike/tests/phpt/Put/CheckWithOnlyPutDataParameter.php(3): aerospike_phpt_runtest('Put', 'testPUTWithOnly...')
#6 {main}
  thrown in /home/gslab/aerospike-client-php/src/aerospike/tests/Put.inc on line 122