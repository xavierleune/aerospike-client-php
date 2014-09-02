--TEST--
Apply UDF on record, Where Module name is empty string.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Udf", "testUdfApplyModuleIsEmptyString");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Udf", "testUdfApplyModuleIsEmptyString");
--EXPECT--
ERR_PARAM
