--TEST--
InfoMany - Positive check for bins

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("InfoMany", "testInfoManyPositiveForBins");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("InfoMany", "testInfoManyPositiveForBins");
--EXPECT--
OK
