--TEST--
Admin connect - admin connect security not enabled

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("AdminConnect", "testConnectSecurityNotEnabled");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("AdminConnect", "testConnectSecurityNotEnabled");
--EXPECTREGEX--
(ERR_SECURITY_NOT_ENABLED|ERR_CLIENT)
