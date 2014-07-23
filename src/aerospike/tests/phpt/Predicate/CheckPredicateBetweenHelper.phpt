--TEST--
PredicateBetween - Basic Operation

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Predicate", "testPredicateBetweenHelper");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Predicate", "testPredicateBetweenHelper");
--EXPECT--
