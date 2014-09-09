--TEST--
ScanBackground - Unknown Function

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ScanBackground", "testScanBackgroundUnknownFunctionNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanBackground", "testScanBackgroundUnknownFunctionNegative");
--EXPECT--
ERR_UDF

