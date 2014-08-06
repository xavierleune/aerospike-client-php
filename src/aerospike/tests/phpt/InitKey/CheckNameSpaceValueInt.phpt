--TEST--
InitKey - Namespace value integer

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("InitKey", "testNameSpaceValueInt");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("InitKey", "testNameSpaceValueInt");
--EXPECT--
OK
