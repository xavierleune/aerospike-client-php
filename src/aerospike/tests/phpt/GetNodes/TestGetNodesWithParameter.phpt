--TEST--
GetNodes - Negative test with arguments

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("GetNodes", "testGetNodesWithParameter");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("GetNodes", "testGetNodesWithParameter");
--EXPECT--
ERR_PARAM
