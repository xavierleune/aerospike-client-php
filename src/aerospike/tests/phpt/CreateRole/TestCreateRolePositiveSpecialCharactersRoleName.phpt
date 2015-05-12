--TEST--
Create role - create role special characters in role name

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("CreateRole", "testCreateRolePositiveSpecialCharactersRoleName");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("CreateRole", "testCreateRolePositiveSpecialCharactersRoleName");
--EXPECT--
OK
