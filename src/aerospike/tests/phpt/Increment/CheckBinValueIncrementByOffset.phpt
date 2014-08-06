--TEST--
Increment - bin value by offset

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Increment", "testBinIncrementByOffsetValue");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Increment", "testBinIncrementByOffsetValue");
--EXPECT--
ERR_BIN_INCOMPATIBLE_TYPE
