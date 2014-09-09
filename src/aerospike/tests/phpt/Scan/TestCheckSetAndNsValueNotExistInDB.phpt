--TEST--
Scan - Set and namespance not exist in DB

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Scan", "testCheckSetAndNsValueNotExistInDB");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Scan", "testCheckSetAndNsValueNotExistInDB");
--EXPECT--
ERR_REQUEST_INVALID

