--TEST--
Exists - Error if Key not exists

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Exist", "testKeyNotExist");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Exist", "testKeyNotExist");
--EXPECT--
ERR_RECORD_NOT_FOUND
