--TEST--
Query - query call has incorrect name of bins

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Query", "testQueryWithIncorrectNameOfBins");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Query", "testQueryWithIncorrectNameOfBins");
--EXPECT--
ERR_QUERY

