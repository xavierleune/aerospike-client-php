--TEST--
Aggregate - extra parameter

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Aggregate", "testAggregateExtraParameter");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateExtraParameter");
--EXPECT--
OK
