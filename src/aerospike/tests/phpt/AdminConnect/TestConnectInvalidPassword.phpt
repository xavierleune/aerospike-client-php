--TEST--
Admin connect - invalid password

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("AdminConnect", "testConnectInvalidPassword");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("AdminConnect", "testConnectInvalidPassword");
--EXPECTREGEX--
(ERR_INVALID_PASSWORD|ERR_CLIENT)
