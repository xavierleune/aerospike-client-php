--TEST--
ScanApply - Unknown Namespace and Set

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ScanApply", "testScanApplyUnknownNamespaceAndSetNegattive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanApply", "testScanApplyUnknownNamespaceAndSetNegative");
--EXPECT--
ERR_SERVER

