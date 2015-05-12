--TEST--
Query roles - query all roles positive

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("QueryRoles", "testQueryRolesPositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("QueryRoles", "testQueryRolesPositive");
--EXPECT--
OK
