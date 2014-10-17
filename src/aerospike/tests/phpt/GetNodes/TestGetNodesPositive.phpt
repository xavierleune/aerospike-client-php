--TEST--
GetNodes - Positive test

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("GetNodes", "testGetNodesPositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("GetNodes", "testGetNodesPositive");
--EXPECT--
OK
