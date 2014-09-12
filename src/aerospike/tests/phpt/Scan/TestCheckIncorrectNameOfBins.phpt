--TEST--
Scan - Incorrect name of bins

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Scan", "testCheckIncorrectNameOfBins");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Scan", "testCheckIncorrectNameOfBins");
--EXPECT--
OK

