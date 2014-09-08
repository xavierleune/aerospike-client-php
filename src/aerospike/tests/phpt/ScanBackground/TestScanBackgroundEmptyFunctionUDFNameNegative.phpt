--TEST--
ScanBackground - Empty module and function

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ScanBackground", "testScanBackgroundEmptyFunctionUDFNameNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanBackground", "testScanBackgroundEmptyFunctionUDFNameNegative");
--EXPECT--
ERR_PARAM

