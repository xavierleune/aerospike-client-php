--TEST--
SetPassword - set password non-existent user

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("SetPassword", "testSetPasswordNonExistentUser");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("SetPassword", "testSetPasswordNonExistentUser");
--EXPECT--
ERR_INVALID_USER
