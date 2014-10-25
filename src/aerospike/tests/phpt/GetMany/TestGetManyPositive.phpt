--TEST--
GetMany - correct arguments, without options and filter bins

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("GetMany","testGetManyPositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("GetMany", "testGetManyPositive");
--EXPECT--
OK

