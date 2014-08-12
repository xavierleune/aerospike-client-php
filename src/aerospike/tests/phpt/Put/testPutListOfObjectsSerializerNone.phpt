--TEST--
 PUT List of objects and serializer option is SERIALIZER_NONE.  

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testPutListOfObjectsSerializerNone");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutListOfObjectsSerializerNone");
--EXPECT--
ERR_PARAM
