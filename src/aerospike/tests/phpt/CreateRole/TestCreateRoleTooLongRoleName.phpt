--TEST--
Create role - create role too long role name

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("CreateRole", "testCreateRoleTooLongRoleName");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("CreateRole", "testCreateRoleTooLongRoleName");
--EXPECT--
ERR_INVALID_ROLE
