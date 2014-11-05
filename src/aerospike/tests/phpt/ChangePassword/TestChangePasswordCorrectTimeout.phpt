--TEST--
ChangePassword - change password with correct timeout

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ChangePassword", "testChangePasswordCorrectTimeout");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ChangePassword", "testChangePasswordCorrectTimeout");
--EXPECT--
OK
