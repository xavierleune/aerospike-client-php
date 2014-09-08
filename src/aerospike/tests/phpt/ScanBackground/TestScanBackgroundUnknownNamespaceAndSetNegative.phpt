T--
ScanBackground - Unknown Namespace and Set

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ScanBackground", "testScanBackgroundUnknownNamespaceAndSetNegattive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanBackground", "testScanBackgroundUnknownNamespaceAndSetNegative");
--EXPECT--
ERR_REQUEST_INVALID

