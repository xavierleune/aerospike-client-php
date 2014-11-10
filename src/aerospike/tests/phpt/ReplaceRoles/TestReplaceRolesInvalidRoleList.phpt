--TEST--
ReplaceRoles - replace roles invalid role list

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ReplaceRoles", "testReplaceRolesInvalidRoleList");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ReplaceRoles", "testReplaceRolesInvalidRoleList");
--EXPECT--
ERR_INVALID_ROLE
