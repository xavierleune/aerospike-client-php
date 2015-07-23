--TEST--
Drop role - drop role incorrect policy

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("DropRole", "testDropRoleIncorrectPolicy");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("DropRole", "testDropRoleIncorrectPolicy");
--EXPECT--
ERR_PARAM
