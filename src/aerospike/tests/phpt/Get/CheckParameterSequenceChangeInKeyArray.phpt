--TEST--
Get - Parameter equence change in Key array

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testCheckParameterSequenceChangeInKeyArray

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testCheckParameterSequenceChangeInKeyArray");
--EXPECT--
501
