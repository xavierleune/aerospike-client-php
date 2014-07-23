--TEST--
PredicateEquals - predicateEquals with string value

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Predicate", "testPredicateEqualsValueString");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Predicate", "testPredicateEqualsValueString");
--EXPECT--
