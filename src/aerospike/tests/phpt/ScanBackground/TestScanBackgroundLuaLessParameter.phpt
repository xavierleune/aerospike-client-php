--TEST--
ScanBackground - Less arguments in udf function definition

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ScanBackground", "testScanBackgroundLuaLessParameter");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanBackground", "testScanBackgroundLuaLessParameter");
--EXPECT--
ERR_UDF
