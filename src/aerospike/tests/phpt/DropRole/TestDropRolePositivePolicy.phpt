--TEST--
Drop role - drop role positive with policy

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("DropRole", "testDropRolePositivePolicy");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("DropRole", "testDropRolePositivePolicy");
--EXPECT--
OK
