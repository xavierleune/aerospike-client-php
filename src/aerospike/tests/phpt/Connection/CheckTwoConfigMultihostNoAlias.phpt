--TEST--
Connection - Check Two Config Multihost no Alias

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Connection", "testTwoConfigMultihostNoAlias");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testTwoConfigMultihostNoAlias");
--EXPECT--
OK
