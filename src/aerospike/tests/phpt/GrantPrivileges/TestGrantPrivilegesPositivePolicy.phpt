--TEST--
Grant Privileges - grant privileges positive policy

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("GrantPrivileges", "testGrantPrivilegesPositivePolicy");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("GrantPrivileges", "testGrantPrivilegesPositivePolicy");
--EXPECT--
OK
