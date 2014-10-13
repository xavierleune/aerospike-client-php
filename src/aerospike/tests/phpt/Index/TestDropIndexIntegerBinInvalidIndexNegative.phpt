--TEST--
dropIndex - Non-existing index

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Index", "testDropIndexIntegerBinInvalidIndexNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Index", "testDropIndexIntegerBinInvalidIndexNegative");
--EXPECT--
OK

