--TEST--
Connection - Check same config and same alias
--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
if (!check_for_socket(AEROSPIKE_CONFIG_NAME, AEROSPIKE_CONFIG_PORT)) {
    die("skip test, the Aerospike DB is not answering at ". AEROSPIKE_CONFIG_NAME. ':'. AEROSPIKE_CONFIG_PORT);
}
--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testSameConfigSameAlias");
--EXPECT--
OK
