--TEST--
Create user - create user with roles as empty

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("CreateUser", "testCreateUserRolesEmpty");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("CreateUser", "testCreateUserRolesEmpty");
--EXPECT--
ERR_PARAM
