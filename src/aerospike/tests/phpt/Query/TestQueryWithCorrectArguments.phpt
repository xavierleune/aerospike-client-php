--TEST--
Query - query call with correct arguments

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Query", "testQueryWithCorrectArguments");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Query", "testQueryWithCorrectArguments");
--EXPECT--
OK

