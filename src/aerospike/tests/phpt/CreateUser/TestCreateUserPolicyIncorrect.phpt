--TEST--
Create user - create user policy incorrect

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("CreateUser", "testCreateUserPolicyIncorrect");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("CreateUser", "testCreateUserPolicyIncorrect");
--EXPECT--
ERR_CLIENT
