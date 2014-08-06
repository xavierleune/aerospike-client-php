--TEST--
Touch - Key not exists

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Touch", "testTouchOperationKeyNotExist");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Touch", "testTouchOperationKeyNotExist");
--EXPECT--
ERR_RECORD_NOT_FOUND
