--TEST--
Scan - Check for correct arguments

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Scan", "testCheckForCorrectArguments");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Scan", "testCheckForCorrectArguments");
--EXPECT--
OK

