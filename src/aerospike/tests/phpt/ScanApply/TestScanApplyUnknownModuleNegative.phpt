--TEST--
ScanApply - Unknown Module

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ScanApply", "testScanApplyUnknownModuleNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanApply", "testScanApplyUnknownModuleNegative");
--EXPECT--
OK

