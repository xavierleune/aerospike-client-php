--TEST--
Get - GET With Wrong number of parameter.Expect atleast 2 get 1.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testGETWrongParameter");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGETWrongParameter");
--EXPECT--
Fatal error: Uncaught exception 'ErrorException' with message 'Aerospike::get() expects at least 2 parameters, 1 given' in /home/gslab/aerospike-client-php/src/aerospike/tests/Get.inc:102
Stack trace:
#0 [internal function]: {closure}(2, 'Aerospike::get(...', '/home/gslab/aer...', 102, Array)
#1 /home/gslab/aerospike-client-php/src/aerospike/tests/Get.inc(102): Aerospike->get(Array)
#2 [internal function]: Get->testGETWrongParameter()
#3 /home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/ASTestFramework.inc(201): ReflectionMethod->invoke(Object(Get))
#4 /home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/astest-phpt-loader.inc(19): ASTestFramework->runSingleTest('testGETWrongPar...')
#5 /home/gslab/aerospike-client-php/src/aerospike/tests/phpt/Get/GetWrongParameter.php(3): aerospike_phpt_runtest('Get', 'testGETWrongPar...')
#6 {main}
  thrown in /home/gslab/aerospike-client-php/src/aerospike/tests/Get.inc on line 102