--TEST--
GetMany - correct arguments, with options but without filter bins

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("GetMany","testGetManyWithOptionsPositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("GetMany", "testGetManyWithOptionsPositive");
--EXPECT--
OK

