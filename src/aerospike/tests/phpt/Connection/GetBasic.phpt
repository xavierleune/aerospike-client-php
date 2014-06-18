--TEST--
Connection - GETBasic

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Connection", "testGET");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testGET");
--EXPECT--
PHP_AEROSPIKE_OK
