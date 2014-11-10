--TEST--
Create user - create user with already existing username

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("CreateUser", "testCreateUserExisting");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("CreateUser", "testCreateUserExisting");
--EXPECT--
ERR_USER_ALREADY_EXISTS
