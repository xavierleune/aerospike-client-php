--TEST--
Admin connect - admin connect no username or password

--SKIPIF--
<?php
include(__FILE__).'/../../aerospike.inc';
if (!defined('AEROSPIKE_ENTERPRISE_EDITION') ||  constant('AEROSPIKE_ENTERPRISE_EDITION') !== true) {
    die("SKIP test config states server is not Enterprise Edition.");
}
--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("AdminConnect", "testConnectNoUsernamePassword");
--EXPECTREGEX--
(ERR_NOT_AUTHENTICATED|ERR_CLIENT)
