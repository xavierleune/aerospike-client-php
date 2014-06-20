--TEST--
Connection - Check Config port parameter type must be integer

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Connection", "testPortValueIsString");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testPortValueIsString");
--EXPECT--
Fatal error: Invalid host port type: 6
 in /home/gslab/aerospike-client-php/src/aerospike/tests/Connection.inc on line 167