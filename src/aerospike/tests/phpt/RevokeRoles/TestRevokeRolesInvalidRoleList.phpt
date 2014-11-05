--TEST--
RevokeRoles - revoke roles invalid role list

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("RevokeRoles", "testRevokeRolesInvalidRoleList");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("RevokeRoles", "testRevokeRolesInvalidRoleList");
--EXPECT--
ERR_INVALID_ROLE
