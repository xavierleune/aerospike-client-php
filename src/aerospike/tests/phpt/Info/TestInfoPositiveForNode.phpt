--TEST--
Info - Positive for info of node

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Info", "testInfoPositiveForNode");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Info", "testInfoPositiveForNode");
--EXPECT--
OK
