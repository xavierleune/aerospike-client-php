--TEST--
Info - Positive for scan job with response initialized to string

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Info", "testInfoPositiveForJobsScan");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Info", "testInfoPositiveForJobsScan");
--EXPECT--
OK
