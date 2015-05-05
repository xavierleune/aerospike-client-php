--TEST--
Aggregate - unknown modules

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateUnknownModuleNegative");
--EXPECT--
ERR_CLIENT

