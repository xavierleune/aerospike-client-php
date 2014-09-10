--TEST--
PredicateEquals - value of predicate is of type array

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("PredicateEquals", "testPredicateEqualsValueIsArray");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("PredicateEquals", "testPredicateEqualsValueIsArray");
--EXPECT--
ERR_PARAM

