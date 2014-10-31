--TEST--
GetMany - with empty key array.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("GetMany","testGetManyEmptyKeyArrayNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("GetMany", "testGetManyEmptyKeyArrayNegative");
--EXPECT--
OK

