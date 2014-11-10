--TEST--
QueryUser - query user with incorrect policy

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("QueryUser", "testQueryUserIncorrectPolicy");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("QueryUser", "testQueryUserIncorrectPolicy");
--EXPECT--
ERR_CLIENT
