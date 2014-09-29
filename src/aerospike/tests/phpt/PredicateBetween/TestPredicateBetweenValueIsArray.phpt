--TEST--
PredicateBetween - predicate between has array parameter

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("PredicateBetween", "testPredicateBetweenValueIsArray");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("PredicateBetween", "testPredicateBetweenValueIsArray");
--EXPECT--
Parameter_Exception

