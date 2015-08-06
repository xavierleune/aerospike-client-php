--TEST--
ChangePassword - change password non-existent user

--SKIPIF--
<?php
include(__FILE__).'/../../aerospike.inc';
if (!defined('AEROSPIKE_ENTERPRISE_EDITION') ||  constant('AEROSPIKE_ENTERPRISE_EDITION') !== true) {
    die("SKIP test config states server is not Enterprise Edition.");
}
--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ChangePassword", "testChangePasswordNonExistentUser");
--EXPECT--
ERR_INVALID_USER
