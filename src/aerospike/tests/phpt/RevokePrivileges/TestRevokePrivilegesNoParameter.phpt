--TEST--
Revoke privileges - revoke privileges no parameter

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("RevokePrivileges", "testRevokePrivilegesNoParameter");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("RevokePrivileges", "testRevokePrivilegesNoParameter");
--EXPECT--
ERR_PARAM
