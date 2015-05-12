--TEST--
Admin connect - admin connect security not supported

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("AdminConnect", "testConnectSecurityNotSupported");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("AdminConnect", "testConnectSecurityNotSupported");
--EXPECTREGEX--
(ERR_SECURITY_NOT_SUPPORTED|ERR_CLIENT)
