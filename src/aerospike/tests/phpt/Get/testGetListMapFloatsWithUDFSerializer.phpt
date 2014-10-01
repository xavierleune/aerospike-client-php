--TEST--
Get List containing Map of floats with UDF serializer.  

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testGetListMapFloatsWithUDFSerializer");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetListMapFloatsWithUDFSerializer");
--EXPECT--
OK
