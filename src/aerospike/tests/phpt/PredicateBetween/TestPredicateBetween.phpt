--TEST--
PredicateBetween - predicate between with correct arguments

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("PredicateBetween", "testPredicateBetween");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("PredicateBetween", "testPredicateBetween");
--EXPECT--
OK

