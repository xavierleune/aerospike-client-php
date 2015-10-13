--TEST--
Aggregate - Aggregate float values

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "normal_001");
--EXPECT--
OK
