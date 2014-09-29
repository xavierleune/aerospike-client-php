--TEST--
Query - query call with one optional argument

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Query", "testQueryWithOneOptionalArgument");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Query", "testQueryWithOneOptionalArgument");
--EXPECT--
OK

