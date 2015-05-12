--TEST--
Grant privileges - grant privileges incorrect role type

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("GrantPrivileges", "testGrantPrivilegesIncorrectRoleType");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("GrantPrivileges", "testGrantPrivilegesIncorrectRoleType");
--EXPECT--
ERR_PARAM
