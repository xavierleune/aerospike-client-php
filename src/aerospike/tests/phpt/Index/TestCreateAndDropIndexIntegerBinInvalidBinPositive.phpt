--TEST--
createIndex and dropIndex - Invalid bin

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Index", "testCreateAndDropIndexIntegerBinInvalidBinPositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Index", "testCreateAndDropIndexIntegerBinInvalidBinPositive");
--EXPECT--
OK

