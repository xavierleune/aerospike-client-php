--TEST--
List registered UDF modules and PHP script passing variable.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Udf",
"testUdfPositiveListRegisteredModulesPassingVariable");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Udf",
"testUdfPositiveListRegisteredModulesPassingVariable");
--EXPECT--
OK
