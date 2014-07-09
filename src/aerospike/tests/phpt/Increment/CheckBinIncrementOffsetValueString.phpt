--TEST--
Increment - bin when offset value string

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Increment", "testBinIncrementOffsetValueString");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Increment", "testBinIncrementOffsetValueString");
--EXPECT--
