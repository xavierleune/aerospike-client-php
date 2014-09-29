--TEST--
ScanApply - Unknown Function

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ScanApply", "testScanApplyUnknownFunctionNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanApply", "testScanApplyUnknownFunctionNegative");
--EXPECT--
OK

