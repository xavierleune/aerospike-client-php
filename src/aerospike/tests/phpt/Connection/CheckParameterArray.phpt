--TEST--
Connection - Check Config parameter must be array

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Connection", "testCheckParameterArray");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testCheckParameterArray");
--EXPECT--
Fatal error: Uncaught exception 'ErrorException' with message 'Aerospike::__construct() expects parameter 1 to be array, string given' in /home/gslab/aerospike-client-php/src/aerospike/tests/Connection.inc:47
Stack trace:
#0 [internal function]: {closure}(2, 'Aerospike::__co...', '/home/gslab/aer...', 47, Array)
#1 /home/gslab/aerospike-client-php/src/aerospike/tests/Connection.inc(47): Aerospike->__construct('')
#2 [internal function]: Connection->testCheckParameterArray()
#3 /home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/ASTestFramework.inc(201): ReflectionMethod->invoke(Object(Connection))
#4 /home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/astest-phpt-loader.inc(19): ASTestFramework->runSingleTest('testCheckParame...')
#5 /home/gslab/aerospike-client-php/src/aerospike/tests/phpt/Connection/CheckParameterArray.php(3): aerospike_phpt_runtest('Connection', 'testCheckParame...')
#6 {main}
  thrown in /home/gslab/aerospike-client-php/src/aerospike/tests/Connection.inc on line 47