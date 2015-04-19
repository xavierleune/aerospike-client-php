--TEST--
Query - query call with correct arguments and a NULL set

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Query", "testQueryWithCorrectArgumentsAndSetNull");
--XFAIL--
Fails due to a known bug with querying a secondary index that was built over the null set of the namespace.
--EXPECT--
OK

