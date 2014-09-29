--TEST--
Scan Info - scan id negative

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ScanInfo", "testScanInfoScanIdNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanInfo", "testScanInfoScanIdNegative");
--EXPECT--
OK

