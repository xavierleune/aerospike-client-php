--TEST--
Grant privileges - grant privileges empty privilege list

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("GrantPrivileges", "testGrantPrivilegesEmptyPrivilegeList");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("GrantPrivileges", "testGrantPrivilegesEmptyPrivilegeList");
--EXPECT--
ERR_PARAM
