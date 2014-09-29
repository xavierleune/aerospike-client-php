--TEST--
Aggregate - extra parameter in lua

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Aggregate", "testAggregateLuaExtraParameter");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateLuaExtraParameter");
--EXPECT--
ERR_UDF
