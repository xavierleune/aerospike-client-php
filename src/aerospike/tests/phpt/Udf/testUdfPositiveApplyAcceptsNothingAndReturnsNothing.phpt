--TEST--
Apply UDF on a record which accepts nothing and returns nothing.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Udf",
"testUdfPositiveApplyAcceptsNothingAndReturnsNothing");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Udf",
"testUdfPositiveApplyAcceptsNothingAndReturnsNothing");
--EXPECT--
OK
