--TEST--
createIndex and dropIndex- index with same name on same integer bin recreated.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Index", "testCreateAndDropIndexSameIntegerBinWithSameNamePositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Index", "testCreateAndDropIndexSameIntegerBinWithSameNamePositive");
--EXPECT--
OK

