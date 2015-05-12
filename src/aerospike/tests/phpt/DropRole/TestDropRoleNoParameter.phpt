--TEST--
Drop role - drop role no parameter

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("DropRole", "testDropRoleNoParameter");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("DropRole", "testDropRoleNoParameter");
--EXPECT--
ERR_PARAM
