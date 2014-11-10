--TEST--
RevokeRoles - revoke read role

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("RevokeRoles", "testRevokeRolesReadRole");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("RevokeRoles", "testRevokeRolesReadRole");
--EXPECT--
ERR_ROLE_VIOLATION
