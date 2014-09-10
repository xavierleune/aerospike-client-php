T--
PredicateBetween - predicate between has one argument

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("PredicateBetween", "testPredicateBetweenValueHasOneArgument");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("PredicateBetween", "testPredicateBetweenValueHasOneArgument");
--EXPECT--
ERR_PARAM

