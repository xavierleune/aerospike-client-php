--TEST--
ReplaceRoles - replace roles is empty

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ReplaceRoles", "testReplaceRolesEmptyRoles");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ReplaceRoles", "testReplaceRolesEmptyRoles");
--EXPECT--
ERR_PARAM
