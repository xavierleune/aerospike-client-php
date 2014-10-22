--TEST--
Operate - Operate with return value as string

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Operate", "testOperateReturnIsStringPositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Operate", "testOperateReturnIsStringPositive");
--EXPECT--
OK
