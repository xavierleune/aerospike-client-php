--TEST--
Connection - Check without Alias and different Config 

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Connection", "testNoAliasDifferentConfigCase2");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testNoAliasDifferentConfigCase2");
--EXPECT--
OK
