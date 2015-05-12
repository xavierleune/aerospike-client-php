--TEST--
Create role - create role invalid privileges type

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("CreateRole", "testCreateRoleInvalidPrivilegesType");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("CreateRole", "testCreateRoleInvalidPrivilegesType");
--EXPECT--
ERR_PARAM
