--TEST--
Get - GET With Third Parameter not an array.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testCheckThirdParameterTypeArray");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testCheckThirdParameterTypeArray");
--EXPECT--
Fatal error: Uncaught exception 'ErrorException' with message 'Aerospike::get() expects parameter 3 to be array, string given' in /home/gslab/aerospike-client-php/src/aerospike/tests/Get.inc:151
Stack trace:
#0 [internal function]: {closure}(2, 'Aerospike::get(...', '/home/gslab/aer...', 151, Array)
#1 /home/gslab/aerospike-client-php/src/aerospike/tests/Get.inc(151): Aerospike->get(Array, Array, '')
#2 [internal function]: Get->testCheckThirdParameterTypeArray()
#3 /home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/ASTestFramework.inc(203): ReflectionMethod->invoke(Object(Get))
#4 /home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/astest-phpt-loader.inc(19): ASTestFramework->runSingleTest('testCheckThirdP...')
#5 /home/gslab/aerospike-client-php/src/aerospike/tests/phpt/Get/CheckThirdParameterTypeArray.php(3): aerospike_phpt_runtest('Get', 'testCheckThirdP...')
#6 {main}
  thrown in /home/gslab/aerospike-client-php/src/aerospike/tests/Get.inc on line 151
