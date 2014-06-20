--TEST--
Get - GET With Second Parameter not an array.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testCheckSecondParameterTypeArray");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testCheckSecondParameterTypeArray");
--EXPECT--
Fatal error: Uncaught exception 'ErrorException' with message 'Aerospike::get() expects parameter 2 to be array, string given' in /home/gslab/aerospike-client-php/src/aerospike/tests/Get.inc:126
Stack trace:
#0 [internal function]: {closure}(2, 'Aerospike::get(...', '/home/gslab/aer...', 126, Array)
#1 /home/gslab/aerospike-client-php/src/aerospike/tests/Get.inc(126): Aerospike->get(Array, '')
#2 [internal function]: Get->testCheckSecondParameterTypeArray()
#3 /home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/ASTestFramework.inc(203): ReflectionMethod->invoke(Object(Get))
#4 /home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/astest-phpt-loader.inc(19): ASTestFramework->runSingleTest('testCheckSecond...')
#5 /home/gslab/aerospike-client-php/src/aerospike/tests/phpt/Get/CheckSecondParameterTypeArray.php(3): aerospike_phpt_runtest('Get', 'testCheckSecond...')
#6 {main}
  thrown in /home/gslab/aerospike-client-php/src/aerospike/tests/Get.inc on line 126
