--TEST--
PredicateContains - Predicate without any parameter.

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("PredicateContainsRange", "testPredicateContainsRangeNoParameter");
--EXPECT--
ERR_PARAM

