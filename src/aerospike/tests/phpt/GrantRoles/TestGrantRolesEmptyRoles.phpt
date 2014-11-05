--TEST--
GrantRoles - grant roles is empty

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("GrantRoles", "testGrantRolesEmptyRoles");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("GrantRoles", "testGrantRolesEmptyRoles");
--EXPECT--
ERR_PARAM
