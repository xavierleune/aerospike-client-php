--TEST--
PUT Map of objects with Serializer option is SERIALIZER_NONE.  

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", testPutMapOfObjectsSerializerNone");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutMapOfObjectsSerializerNone");
--EXPECT--
ERR_PARAM
