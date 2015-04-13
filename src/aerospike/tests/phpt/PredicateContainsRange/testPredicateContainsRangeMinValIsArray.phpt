--TEST--
PredicateBetween - predicate between has array parameter

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("PredicateContainsRange", "testPredicateContainsRangeMinValIsArray");
--EXPECT--
ERR_PARAM

