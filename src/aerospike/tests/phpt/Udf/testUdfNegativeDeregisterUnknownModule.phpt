--TEST--
Deregisters UDF module at the Aerospike DB which is not present.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Udf", "testUdfNegativeDeregisterUnknownModule");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Udf", "testUdfNegativeDeregisterUnknownModule");
--EXPECT--
ERR_UDF
