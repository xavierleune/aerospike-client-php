--TEST--
Connection - Check Config parameter must contains Name key

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Connection", "testMissingNameKeyFromConfigArray");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testMissingNameKeyFromConfigArray");
--EXPECT--
[aerospike.c:109][aerospike_connect] ERROR - no hosts provided
error(200) no hosts provided at [src/main/aerospike/aerospike.c:110]
PHP_AEROSPIKE_OK