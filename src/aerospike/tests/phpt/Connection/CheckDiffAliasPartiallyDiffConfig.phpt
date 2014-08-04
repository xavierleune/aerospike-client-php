--TEST--
Connection - Check Config parameter must contains port key

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Connection", "testDiffAliasPartiallyDiffConfig");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testDiffAliasPartiallyDiffConfig");
--EXPECT--
0
