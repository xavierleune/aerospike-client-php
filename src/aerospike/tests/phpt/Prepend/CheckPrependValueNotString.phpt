--TEST--
Prepend - Basic prepend operation.Prepended value not string.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Prepend", "testPrependValueNotString");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Prepend", "testPrependValueNotString");
--EXPECT--
0
