--TEST--
PUT Map of floats and serializer option is SERIALIZER_NONE.  

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testPutMapOfFloatsSerializerNone");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutMapOfFloatsSerializerNone");
--EXPECT--
ERR_PARAM
