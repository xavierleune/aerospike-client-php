--TEST--
Query - query call with incorrect ns and set

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Query", "testQueryWithIncorrectNsSet");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Query", "testQueryWithIncorrectNsSet");
--EXPECT--
ERR_REQUEST_INVALID

