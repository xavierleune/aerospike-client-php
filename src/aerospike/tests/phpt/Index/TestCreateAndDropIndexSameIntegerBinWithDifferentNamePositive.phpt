--TEST--
createIndex and dropIndex- index with different name on same integer bin.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Index", "testCreateAndDropIndexSameIntegerBinWithDifferentNamePositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Index", "testCreateAndDropIndexSameIntegerBinWithDifferentNamePositive");
--EXPECT--
OK
