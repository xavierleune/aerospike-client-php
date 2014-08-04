--TEST--
Prepend - Basic prepend operation bin not exists.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Prepend", "testPrependBinNotExist");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Prepend", "testPrependBinNotExist");
--EXPECT--
0
