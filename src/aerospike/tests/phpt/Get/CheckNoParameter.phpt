--TEST--
Get - GET With No parameter.Expect atleast 2 get 0.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testGETNoParameter");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGETNoParameter");
--EXPECT--
Fatal error: Uncaught exception 'ErrorException' with message 'Aerospike::get() expects at least 2 parameters, 0 given' in /home/gslab/aerospike-client-php/src/aerospike/tests/Get.inc:30
Stack trace:
#0 [internal function]: {closure}(2, 'Aerospike::get(...', '/home/gslab/aer...', 30, Array)
#1 /home/gslab/aerospike-client-php/src/aerospike/tests/Get.inc(30): Aerospike->get()
#2 [internal function]: Get->testGETNoParameter()
#3 /home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/ASTestFramework.inc(203): ReflectionMethod->invoke(Object(Get))
#4 /home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/astest-phpt-loader.inc(19): ASTestFramework->runSingleTest('testGETNoParame...')
#5 /home/gslab/aerospike-client-php/src/aerospike/tests/phpt/Get/CheckNoParameter.php(3): aerospike_phpt_runtest('Get', 'testGETNoParame...')
#6 {main}
  thrown in /home/gslab/aerospike-client-php/src/aerospike/tests/Get.inc on line 30
