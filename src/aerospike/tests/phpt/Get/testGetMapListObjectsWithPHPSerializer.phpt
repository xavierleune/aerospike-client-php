--TEST--
Get Map containing List of objects with PHP Serializer.  

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testGetMapListObjectsWithPHPSerializer");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetMapListObjectsWithPHPSerializer");
--EXPECT--
OK
