--TEST--
Get - NameSpace Parameter missing in key array.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testCheckNameSpaceParameterMissingInKeyArray");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testCheckNameSpaceParameterMissingInKeyArray");
--EXPECT--
ERR_PARAM
