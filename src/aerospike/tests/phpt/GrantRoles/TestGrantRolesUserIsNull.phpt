--TEST--
GrantRoles - grant roles with user null

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("GrantRoles", "testGrantRolesUserIsNull");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("GrantRoles", "testGrantRolesUserIsNull");
--EXPECT--
ERR_PARAM
