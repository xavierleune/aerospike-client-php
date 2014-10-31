--TEST--
 Basic existsMany operation with empty string passed 
 in place of key array .

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ExistsMany", "testExistsManyKeyEmptyStringNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ExistsMany", "testExistsManyKeyEmptyStringNegative");
--EXPECT--
Parameter_Exception
