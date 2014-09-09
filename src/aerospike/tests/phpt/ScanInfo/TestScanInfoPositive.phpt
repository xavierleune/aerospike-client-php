--TEST--
Scaninfo - correct arguments

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ScanInfo", "testScanInfoPositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanInfo", "testScanInfoPositive");
--EXPECT--
OK

