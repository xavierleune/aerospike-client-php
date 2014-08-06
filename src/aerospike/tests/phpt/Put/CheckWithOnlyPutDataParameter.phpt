--TEST--
Put - Without key parameter.Only data parameter is available.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testPUTWithOnlyDataParameter");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPUTWithOnlyDataParameter");
--EXPECT--
Parameter_Exception
