--TEST--
Scaninfo - scan id array

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ScanInfo", "testScanInfoScanIdArray");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanInfo", "testScanInfoScanIdArray");
--EXPECT--
ERR_PARAM

