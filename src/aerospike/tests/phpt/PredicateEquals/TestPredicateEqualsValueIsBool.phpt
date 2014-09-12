--TEST--
PredicateEquals - value of predicate is of type boolean

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("PredicateEquals", "testPredicateEqualValueIsBool");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("PredicateEquals", "testPredicateEqualsValueIsBool");
--EXPECT--
OK

