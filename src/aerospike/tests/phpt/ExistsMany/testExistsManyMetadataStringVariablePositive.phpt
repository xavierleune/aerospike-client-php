--TEST--
 Basic existsMany, Metadata is of type string.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ExistsMany",
"testExistsManyMetadataStringVariablePositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ExistsMany",
"testExistsManyMetadataStringVariablePositive");
--EXPECT--
OK
