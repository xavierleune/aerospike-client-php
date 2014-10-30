--TEST--
 Basic existsMany operation with no arguments.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ExistsMany", "testExistsManyNoArgumentsNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ExistsMany", "testExistsManyNoArgumentsNegative");
--EXPECT--
Parameter_Exception
