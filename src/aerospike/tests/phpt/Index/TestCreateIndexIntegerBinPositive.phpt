--TEST--
createIndex - correct arguments for integer index

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Index", "testCreateIndexIntegerBinPositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Index", "testCreateIndexIntegerBinPositive");
--EXPECT--
OK

