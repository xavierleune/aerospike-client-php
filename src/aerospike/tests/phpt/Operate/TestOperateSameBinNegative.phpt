--TEST--
Operate - Operate with same bin negative

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Operate", "testOperateSameBinNegative");
--XFAIL--
Servers prior to 3.6.0 could not handle multiple operations on the same bin.
--EXPECT--
OK
