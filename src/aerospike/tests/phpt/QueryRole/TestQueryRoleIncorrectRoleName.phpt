--TEST--
Query role - query role incorrect role name

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("QueryRole", "testQueryRoleIncorrectRoleName");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("QueryRole", "testQueryRoleIncorrectRoleName");
--EXPECT--
ERR_INVALID_ROLE
