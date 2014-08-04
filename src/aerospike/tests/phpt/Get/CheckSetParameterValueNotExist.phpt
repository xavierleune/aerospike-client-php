--TEST--
Get - Check Set Value not exist in Database

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testCheckSetValueNotExistInDB");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testCheckSetValueNotExistInDB");
--EXPECT--
602
