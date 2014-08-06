--TEST--
Connection - Check Config parameter must contains Hosts key

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Connection", "testMissingHostsKeyFromConfigArray");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testMissingHostsKeyFromConfigArray");
--EXPECT--
ERR_PARAM
