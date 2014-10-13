--TEST--
createIndex and dropIndex - Invalid bin

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Index", "testCreateAndDropIndexIntegerBinInvalidBinNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Index", "testCreateAndDropIndexIntegerBinInvalidBinNegative");
--EXPECT--
OK

