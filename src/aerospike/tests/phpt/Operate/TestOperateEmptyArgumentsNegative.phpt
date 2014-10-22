--TEST--
Operate - Operate with no arguments

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Operate", "testOperateEmptyArgumentsNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Operate", "testOperateEmptyArgumentsNegative");
--EXPECT--
ERR_CLIENT
