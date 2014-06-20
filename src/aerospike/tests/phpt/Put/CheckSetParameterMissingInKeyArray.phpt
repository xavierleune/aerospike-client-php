--TEST--
Get - NameSpace Parameter missing in key array.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testCheckSetParameterMissingInKeyArray");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testCheckSetParameterMissingInKeyArray");
--EXPECT--