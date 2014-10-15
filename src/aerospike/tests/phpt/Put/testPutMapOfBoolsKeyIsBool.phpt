--TEST--
Put - Boolean value asa a key

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testPutMapOfBoolsKeyIsBool");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutMapOfBoolsKeyIsBool");
--EXPECT--
OK
