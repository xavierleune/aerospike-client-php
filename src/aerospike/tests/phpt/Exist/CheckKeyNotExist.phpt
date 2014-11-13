--TEST--
Exists - Error if Key not exists

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Exist", "testKeyNotExist");
--EXPECT--
ERR_RECORD_NOT_FOUND
