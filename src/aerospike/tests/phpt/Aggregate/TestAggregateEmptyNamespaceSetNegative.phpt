--TEST--
Aggregate - empty namespace and set

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Aggregate", "testAggregateEmptyNamespaceSetNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateEmptyNamespaceSetNegative");
--EXPECT--
ERR_PARAM
