--TEST--
GetNodes - Positive test with policy

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("GetNodes", "testGetNodesPositiveWithPolicy");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("GetNodes", "testGetNodesPositiveWithPolicy");
--EXPECT--
OK
