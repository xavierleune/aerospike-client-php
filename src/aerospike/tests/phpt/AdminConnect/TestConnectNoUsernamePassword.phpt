--TEST--
Admin connect - admin connect no username or password

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("AdminConnect", "testConnectNoUsernamePassword");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("AdminConnect", "testConnectNoUsernamePassword");
--EXPECT--
ERR_NOT_AUTHENTICATED
