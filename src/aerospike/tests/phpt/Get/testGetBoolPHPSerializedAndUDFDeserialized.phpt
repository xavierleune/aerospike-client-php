--TEST--
 PUT Map containing List of floats with UDF serializer.  

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testGetBoolPHPSerializedAndUDFDeserialized");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetBoolPHPSerializedAndUDFDeserialized");
--EXPECT--
OK
