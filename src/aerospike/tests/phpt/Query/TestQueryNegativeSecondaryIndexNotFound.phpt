--TEST--
Query - Negative case with query on non-indexed bin in db

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Query", "testQueryNegativeSecondaryIndexNotFound");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Query", "testQueryNegativeSecondaryIndexNotFound");
--EXPECT--
ERR_INDEX_NOT_FOUND

