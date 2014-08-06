--TEST--
Connection - Test Connection

--SKIPIF--
<?php
/*include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Connection", "testNoAliasDiffrentConfig");*/

--FILE--
<?php
/*include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testNoAliasDiffrentConfig");*/
--EXPECT--
PHP_AEROSPIKE_OK
