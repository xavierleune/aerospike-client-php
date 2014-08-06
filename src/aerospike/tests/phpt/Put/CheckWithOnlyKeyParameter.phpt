--TEST--
Put - Only key parameter.Data parameter missing.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testPUTWithOnlyKeyParameter");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPUTWithOnlyKeyParameter");
--EXPECT--
Parameter_Exception
