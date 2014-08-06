--TEST--
Put - Key Parameter missing in key array.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testCheckKeyParameterMissingInKeyArray");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testCheckKeyParameterMissingInKeyArray");
--EXPECT--
ERR_PARAM
