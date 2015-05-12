--TEST--
Query roles - query all roles positive with policy

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("QueryRoles", "testQueryRolesPositivePolicy");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("QueryRoles", "testQueryRolesPositivePolicy");
--EXPECT--
OK
