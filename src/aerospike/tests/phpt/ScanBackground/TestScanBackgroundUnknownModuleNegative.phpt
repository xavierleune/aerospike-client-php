--TEST--
ScanBackground - Unknown Module

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ScanBackground", "testScanBackgroundUnknownModuleNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanBackground", "testScanBackgroundUnknownModuleNegative");
--EXPECT--
ERR_UDF_NOT_FOUND

