--TEST--
Get - NameSpace Parameter missing in key array.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testCheckNameSpaceParameterMissingInKeyArray");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testCheckNameSpaceParameterMissingInKeyArray");
--EXPECT--
error(501) AEROSPIKE_ERR_REQUEST_INVALID at [(null):0]
Assertion failed.. dumping backtrace..
#0  ASTestFramework->dieCommon(Expected TRUE) called at [/home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/ASTestFramework.inc:113]
#1  ASTestFramework->assertTrue(, Aerospike Get Response) called at [/home/gslab/aerospike-client-php/src/aerospike/tests/Get.inc:201]
#2  Get->testCheckNameSpaceParameterMissingInKeyArray()
#3  ReflectionMethod->invoke(Get Object ([] => astestframework_error_handler,[handle] => Aerospike Object (),[oo] => Aerospike Object ())) called at [/home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/ASTestFramework.inc:203]
#4  ASTestFramework->runSingleTest(testCheckNameSpaceParameterMissingInKeyArray) called at [/home/gslab/aerospike-client-php/src/aerospike/tests/astestframework/astest-phpt-loader.inc:19]
#5  aerospike_phpt_runtest(Get, testCheckNameSpaceParameterMissingInKeyArray) called at [/home/gslab/aerospike-client-php/src/aerospike/tests/phpt/Get/CheckNameSpaceParameterMissingInKeyArray.php:3]
[ASTestFramework] Expected TRUE
