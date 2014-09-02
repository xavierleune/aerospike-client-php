--TEST--
Registers UDF module at the Aerospike DB which is not present.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Udf", "testUdfRegisterUnknownModule");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Udf", "testUdfRegisterUnknownModule");
--EXPECT--
ERR_PARAM
