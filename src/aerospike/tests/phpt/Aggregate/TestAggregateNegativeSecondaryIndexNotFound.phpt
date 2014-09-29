--TEST--
Aggregate - Negative case with aggregation on non-indexed bin in db

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Aggregate", "testAggregateNegativeSecondaryIndexNotFound");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateNegativeSecondaryIndexNotFound");
--EXPECT--
ERR_INDEX

