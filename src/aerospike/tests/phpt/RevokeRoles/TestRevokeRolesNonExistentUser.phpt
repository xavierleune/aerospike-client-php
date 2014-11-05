--TEST--
RevokeRoles - revoke roles on non-existent user

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("RevokeRoles", "testRevokeRolesNonExistentUser");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("RevokeRoles", "testRevokeRolesNonExistentUser");
--EXPECT--
ERR_INVALID_USER
