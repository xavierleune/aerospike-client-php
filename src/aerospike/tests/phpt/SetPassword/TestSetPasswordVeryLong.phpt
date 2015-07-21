--TEST--
SetPassword - set password very long

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("SetPassword", "testSetPasswordVeryLong");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("SetPassword", "testSetPasswordVeryLong");
--EXPECT--
OK
