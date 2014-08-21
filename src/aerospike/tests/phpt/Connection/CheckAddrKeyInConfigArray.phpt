--TEST--
Connection - Check instantiation with a malformed config array (missing "addr")
--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testMissingNameKeyFromConfigArray");
--EXPECT--
ERR_PARAM
