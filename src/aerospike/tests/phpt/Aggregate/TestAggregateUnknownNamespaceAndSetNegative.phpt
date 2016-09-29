--TEST--
Aggregate - unknown namespace and set

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateUnknownNamespaceAndSetNegative");
--EXPECTREGEX--
(ERR_REQUEST_INVALID|ERR_UDF|ERR_NAMESPACE_NOT_FOUND)

