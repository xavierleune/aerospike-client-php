--TEST--
Get - NameSpace Parameter missing in key array.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testCheckSetParameterMissingInKeyArray");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testCheckSetParameterMissingInKeyArray");
--EXPECT--
error(602) AEROSPIKE_ERR_RECORD_NOT_FOUND at [(null):0]
Assertion failed.. dumping backtrace..
#0  ASTestFramework->dieCommon(Expected TRUE) called at [/home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/ASTestFramework.inc:113]
#1  ASTestFramework->assertTrue(, Aerospike Get Response) called at [/home/gslab/aerospike-client-php/src/aerospike/tests/Get.inc:225]
#2  Get->testCheckSetParameterMissingInKeyArray()
#3  ReflectionMethod->invoke(Get Object ([] => astestframework_error_handler,[handle] => Aerospike Object (),[oo] => Aerospike Object ())) called at [/home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/ASTestFramework.inc:201]
#4  ASTestFramework->runSingleTest(testCheckSetParameterMissingInKeyArray) called at [/home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/astest-phpt-loader.inc:19]
#5  aerospike_phpt_runtest(Get, testCheckSetParameterMissingInKeyArray) called at [/home/gslab/aerospike-client-php/src/aerospike/tests/phpt/Get/CheckSetParameterMissingInKeyArray.php:3]
[ASTestFramework] Expected TRUE
