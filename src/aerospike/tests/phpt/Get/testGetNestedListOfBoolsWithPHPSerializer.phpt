--TEST--
Get Nested List of bools with PHP serializer.  

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testGetNestedLisOfBoolstWithPHPSerializer");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetNestedListOfBoolsWithPHPSerializer");
--EXPECT--
OK
