--TEST--
createIndex and dropIndex - index with same name on different integer bin recreated.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Index", "testCreateAndDropIndexDifferentIntegerBinWithSameNamePositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Index", "testCreateAndDropIndexDifferentIntegerBinWithSameNamePositive");
--EXPECT--
OK

