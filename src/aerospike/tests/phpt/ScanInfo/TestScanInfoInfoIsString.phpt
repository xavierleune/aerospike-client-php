--TEST--
Scaninfo - info parameter is string

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ScanInfo", "testScanInfoInfoIsString");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanInfo", "testScanInfoInfoIsString");
--EXPECT--
ERR_PARAM

