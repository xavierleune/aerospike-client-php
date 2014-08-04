--TEST--
Put - Parameter equence change in Key array

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testCheckParameterSequenceChangeInKeyArray

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testCheckParameterSequenceChangeInKeyArray");
--EXPECT--
201
