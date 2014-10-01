--TEST--
 Get Map containing List of floats with UDF serializer.  

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testGetMapListFloatsWithUDFSerializer");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetMapListFloatsWithUDFSerializer");
--EXPECT--
OK
