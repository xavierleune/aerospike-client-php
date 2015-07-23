--TEST--
Query roles - query all roles incorrect policy

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("QueryRoles", "testQueryRolesIncorrectPolicy");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("QueryRoles", "testQueryRolesIncorrectPolicy");
--EXPECT--
ERR_PARAM
