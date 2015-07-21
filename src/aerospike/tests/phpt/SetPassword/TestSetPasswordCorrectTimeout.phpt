--TEST--
SetPassword - set password with correct timeout

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("SetPassword", "testSetPasswordCorrectTimeout");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("SetPassword", "testSetPasswordCorrectTimeout");
--EXPECT--
OK
