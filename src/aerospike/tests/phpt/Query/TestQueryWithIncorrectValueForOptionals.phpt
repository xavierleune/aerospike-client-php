--TEST--
Query - query call has incorrect value for optionals

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Query", "testQueryWithIncorrectValueForOptionals");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Query", "testQueryWithIncorrectValueForOptionals");
--EXPECT--
ERR

