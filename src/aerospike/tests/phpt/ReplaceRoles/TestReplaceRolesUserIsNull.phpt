--TEST--
ReplaceRoles - replace roles with user null

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ReplaceRoles", "testReplaceRolesUserIsNull");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ReplaceRoles", "testReplaceRolesUserIsNull");
--EXPECT--
ERR_PARAM
