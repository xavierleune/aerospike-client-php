--TEST--
GetMany - with empty string for key array.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("GetMany","testGetManyKeyEmptyStringNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("GetMany", "testGetManyKeyEmptyStringNegative");
--EXPECT--
ERR_CLIENT

