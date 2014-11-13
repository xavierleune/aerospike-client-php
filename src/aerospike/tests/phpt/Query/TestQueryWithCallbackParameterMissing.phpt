--TEST--
Query - query call with callback parameter missing

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Query", "testQueryWithCallbackParameterMissing");
--EXPECT--
Parameter_Exception

