--TEST--
Query - Query call with where parameter missing

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Query", "testQueryWithWhereParameterMissing");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Query", "testQueryWithWhereParameterMissing");
--EXPECT--
Parameter_Exception

