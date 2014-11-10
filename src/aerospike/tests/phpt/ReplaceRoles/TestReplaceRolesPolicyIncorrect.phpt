--TEST--
ReplaceRoles - replace roles is positive with incorrect policy

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ReplaceRoles", "testReplaceRolesPolicyIncorrect");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ReplaceRoles", "testReplaceRolesPolicyIncorrect");
--EXPECT--
ERR_CLIENT
