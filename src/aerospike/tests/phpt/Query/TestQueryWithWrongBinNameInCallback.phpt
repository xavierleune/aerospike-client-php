--TEST--
Query - query call has wrong bin name in callback

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Query", "testQueryWithWrongBinNameInCallback");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Query", "testQueryWithWrongBinNameInCallback");
--EXPECT--
ERR_QUERY

