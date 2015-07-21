--TEST--
SetPassword - set password has no parameter

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("SetPassword", "testSetPasswordNoParameter");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("SetPassword", "testSetPasswordNoParameter");
--EXPECT--
ERR_PARAM
