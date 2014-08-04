--TEST--
PredicateEquals - string args to become an array of (min, max) integer

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Predicate", "testPredicateBetweenArrayValueWithStrings");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Predicate", "testPredicateBetweenArrayValueWithStrings");
--EXPECT--
