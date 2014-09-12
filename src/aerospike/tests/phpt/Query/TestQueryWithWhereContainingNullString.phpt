--TEST--
Query - where parameter contains null string

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Query", "testQueryWithWhereContainingNullString");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Query", "testQueryWithWhereContainingNullString");
--EXPECT--
ERR_PARAM

