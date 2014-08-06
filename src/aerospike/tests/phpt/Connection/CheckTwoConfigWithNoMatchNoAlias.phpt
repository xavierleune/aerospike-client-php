--TEST--
Connection - Check Config

--SKIPIF--
<?php
/*include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Connection", "testTwoConfigWithNoMatchNoAlias");*/

--FILE--
<?php
/*include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testTwoConfigWithNoMatchNoAlias");*/
--EXPECT--
PHP_AEROSPIKE_OK
