--TEST--
Apply UDF on a record and UDF returns a Map(Array) but we are passing variable.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Udf", "testUdfApplyReturnsMapNeg");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Udf", "testUdfApplyReturnsMapNeg");
--EXPECT--
OK
