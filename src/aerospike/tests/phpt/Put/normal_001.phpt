--TEST--
Put - Put float value

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "normal_001");
--EXPECT--
OK
