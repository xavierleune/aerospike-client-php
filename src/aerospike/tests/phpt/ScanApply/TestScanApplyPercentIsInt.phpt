--TEST--
ScanApply - Percent is int

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanApply", "testScanApplyPercentIsInt");
--XFAIL--
A bug in C client as_scan_set_percent() only allows for 0 or 100 as values.
--EXPECT--
OK
