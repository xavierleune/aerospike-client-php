--TEST--
Get Map containing List of objects with UDF Serializer.  

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testGetMapListObjectsWithUDFSerializer");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetMapListObjectsWithUDFSerializer");
--EXPECT--
OK
