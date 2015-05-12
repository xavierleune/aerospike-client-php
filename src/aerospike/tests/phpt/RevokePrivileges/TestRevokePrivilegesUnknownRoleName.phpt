--TEST--
Revoke privileges - revoke privileges unknown role name

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("RevokePrivileges", "testRevokePrivilegesUnknownRoleName");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("RevokePrivileges", "testRevokePrivilegesUnknownRoleName");
--EXPECT--
ERR_INVALID_ROLE
