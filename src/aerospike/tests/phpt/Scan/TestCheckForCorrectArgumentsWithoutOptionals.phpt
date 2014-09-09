--TEST--
Scan - Correct arguments without optionals

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Scan", "testCheckForCorrectArgumentsWithoutOptionals");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Scan", "testCheckForCorrectArgumentsWithoutOptionals");
--EXPECT--
OK

