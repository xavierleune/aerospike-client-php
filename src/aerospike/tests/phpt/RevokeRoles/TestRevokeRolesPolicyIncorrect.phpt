--TEST--
RevokeRoles - revoke roles is positive with incorrect policy

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("RevokeRoles", "testRevokeRolesPolicyIncorrect");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("RevokeRoles", "testRevokeRolesPolicyIncorrect");
--EXPECT--
ERR_CLIENT
