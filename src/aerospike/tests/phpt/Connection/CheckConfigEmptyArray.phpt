--TEST--
Connection - Check Config parameter for empty array

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Connection", "testEmptyArray");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testEmptyArray");
--EXPECT--
Fatal error: Invalid host port type: 6
 in /home/gslab/aerospike-client-php/src/aerospike/tests/Connection.inc on line 67