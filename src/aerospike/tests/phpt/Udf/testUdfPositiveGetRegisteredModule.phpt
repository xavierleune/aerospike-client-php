--TEST--
Get registered UDF's code.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Udf", "testUdfPositiveGetRegisteredModule");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Udf", "testUdfPositiveGetRegisteredModule");
--EXPECT--
OK
