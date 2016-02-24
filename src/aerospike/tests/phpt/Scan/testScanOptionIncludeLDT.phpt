--TEST--
scan() with option OPT_SCAN_INCLUDELDT

--DESCRIPTION--
scan() all the records from namespace.set and include LDT bin values along with
the LDT bin names

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Scan", "testScanOptionIncludeLDT");
?>

--EXPECT--
OK

