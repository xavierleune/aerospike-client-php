--TEST--
Revoke privileges - revoke privileges incorrect role type

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("RevokePrivileges", "testRevokePrivilegesIncorrectRoleType");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("RevokePrivileges", "testRevokePrivilegesIncorrectRoleType");
--EXPECT--
ERR_PARAM
