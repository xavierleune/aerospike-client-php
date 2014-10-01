--TEST--
Get Nested List of floats with UDF serializer. 

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testGetNestedListOfFloatsWithUDFSerializer");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetNestedListOfFloatsWithUDFSerializer");
--EXPECT--
OK
