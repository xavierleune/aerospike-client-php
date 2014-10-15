--TEST--
InfoMany - Positive check for bins with config and policy

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("InfoMany", "testInfoManyPositiveForBinsWithPolicy");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("InfoMany", "testInfoManyPositiveForBinsWithPolicy");
--EXPECT--
OK
