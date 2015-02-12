--TEST--
Remove - Key not exist

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Remove", "testKeyRemoveKeyNotExist");
--EXPECT--
OK
