--TEST--
InfoMany - Negative with incorrect structure for config

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("InfoMany", "testInfoManyNegativeIncorrectConfigStructure");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("InfoMany", "testInfoManyNegativeIncorrectConfigStructure");
--EXPECT--
ERR_PARAM
