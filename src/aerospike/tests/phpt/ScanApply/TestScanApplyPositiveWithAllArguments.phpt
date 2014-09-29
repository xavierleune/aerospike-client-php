--TEST--
ScanApply - Correct arguments with optionals

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ScanApply", "testScanApplyPositiveWithAllArguments");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanApply", "testScanApplyPositiveWithAllArguments");
--EXPECT--
OK

