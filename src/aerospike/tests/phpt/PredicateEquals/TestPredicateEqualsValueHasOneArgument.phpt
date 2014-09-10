--TEST--
Predicate Equals - Only one argument present

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("PredicateEquals", "testPredicateEqualsValueHasOneArgument");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("PredicateEquals", "testPredicateEqualsValueHasOneArgument");
--EXPECT--
ERR_PARAM

