--TEST--
Scan - Incorrect bin name in callback

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Scan", "testCheckIncorrectBinNameInCallback");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Scan", "testCheckIncorrectBinNameInCallback");
--EXPECT--
ERR_SCAN

