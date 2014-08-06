--TEST--
Increment - bin value by offset key not exist

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Increment", "testBinIncrementKeyNotExist");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Increment", "testBinIncrementKeyNotExist");
--EXPECT--
ERR_RECORD_NOT_FOUND
