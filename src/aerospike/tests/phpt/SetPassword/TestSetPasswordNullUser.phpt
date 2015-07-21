--TEST--
SetPassword - set password null user

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("SetPassword", "testSetPasswordNullUser");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("SetPassword", "testSetPasswordNullUser");
--EXPECT--
ERR_PARAM
