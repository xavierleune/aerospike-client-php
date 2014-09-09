--TEST--
Scan - Check for empty callback funtion

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Scan", "testCheckForEmptyCallbackFunction");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Scan", "testCheckForEmptyCallbackFunction");
--EXPECT--
OK

