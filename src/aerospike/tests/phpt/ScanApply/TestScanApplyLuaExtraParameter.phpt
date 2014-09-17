--TEST--
ScanApply - Extra parameter in udf function definition

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ScanApply", "testScanApplyLuaExtraParameter");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanApply", "testScanApplyLuaExtraParameter");
--EXPECT--
OK
