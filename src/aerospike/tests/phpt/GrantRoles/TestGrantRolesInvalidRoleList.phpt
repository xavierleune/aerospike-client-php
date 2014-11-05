--TEST--
GrantRoles - grant roles invalid role list

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("GrantRoles", "testGrantRolesInvalidRoleList");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("GrantRoles", "testGrantRolesInvalidRoleList");
--EXPECT--
INVALID_ROLE
