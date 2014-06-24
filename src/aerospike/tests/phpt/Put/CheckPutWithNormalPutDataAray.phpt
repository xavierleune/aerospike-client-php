--TEST--
Put - data as normal array ex. array("hi","hello");

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testPUTWithNormalpPutDataArrayParameter");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPUTWithNormalpPutDataArrayParameter");
--EXPECT--
PHP_AEROSPIKE_OK
