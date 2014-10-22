--TEST--
InfoMany - Positive check for bins with config

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("InfoMany", "testInfoManyPositiveForBinsWithConfig");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("InfoMany", "testInfoManyPositiveForBinsWithConfig");
--EXPECT--
OK
