--TEST--
Aggregate - correct arguments

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Aggregate", "testAggregatePositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregatePositive");
--EXPECT--
OK

