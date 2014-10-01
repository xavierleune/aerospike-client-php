--TEST--
GET object which is serialized using SERIALIZER_PHP and deserialized 
with SERIALIZER_USER.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testGetObjectPHPSerializedAndUDFDeserialized");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetObjectPHPSerializedAndUDFDeserialized");
--EXPECT--
OK
