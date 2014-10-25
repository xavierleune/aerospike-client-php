--TEST--
GetMany - with option as integer
--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("GetMany", "testGetManyOptionsIntegerNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("GetMany", "testGetManyOptionsIntegerNegative");
--EXPECT--
OK

