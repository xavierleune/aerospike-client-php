--TEST--
Get List containing Map of floats with PHP serializer.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testGetListMapFloatsWithPHPSerializer");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetListMapFloatsWithPHPSerializer");
--EXPECT--
OK
