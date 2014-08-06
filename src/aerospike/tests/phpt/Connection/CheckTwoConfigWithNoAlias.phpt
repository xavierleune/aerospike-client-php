--TEST--
Connection - Check Configs with no Alias

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Connection", "testTwoConfigWithNoAlias");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testTwoConfigWithNOAlias");
--EXPECT--
OK
