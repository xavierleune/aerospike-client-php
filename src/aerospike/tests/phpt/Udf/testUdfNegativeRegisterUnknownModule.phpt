--TEST--
Registers UDF module at the Aerospike DB which is not present.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Udf", "testUdfNegativeRegisterUnknownModule");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Udf", "testUdfNegativeRegisterUnknownModule");
--EXPECT--
ERR_UDF_NOT_FOUND
