--TEST--
Scaninfo - scan id is a string

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ScanInfo", "testScanInfoScanIdString");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanInfo", "testScanInfoScanIdString");
--EXPECT--
ERR_PARAM

