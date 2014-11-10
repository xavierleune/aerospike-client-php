--TEST--
ReplaceRoles - replace roles is positive with policy

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ReplaceRoles", "testReplaceRolesPositivePolicy");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ReplaceRoles", "testReplaceRolesPositivePolicy");
--EXPECT--
OK

