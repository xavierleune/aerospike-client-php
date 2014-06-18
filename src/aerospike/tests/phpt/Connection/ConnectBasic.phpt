--TEST--
Connection - ConnectBasic

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Connection", "testConnectBasic");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testConnectBasic");
--EXPECT--
PHP_AEROSPIKE_OK
