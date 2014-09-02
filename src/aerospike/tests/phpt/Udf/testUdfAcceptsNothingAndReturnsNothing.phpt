--TEST--
Apply UDF on a record, UDF which accepts nothing and no return.  

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Udf", "testUdfAcceptsNothingAndReturnsNothing");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Udf", "testUdfAcceptsNothingAndReturnsNothing");
--EXPECT--
OK
