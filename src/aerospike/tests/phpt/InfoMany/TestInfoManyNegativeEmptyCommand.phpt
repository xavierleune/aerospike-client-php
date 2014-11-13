--TEST--
InfoMany - Negative with empty command

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("InfoMany", "testInfoManyNegativeEmptyCommand");
--XFAIL--
Should fail until the C client can accept null/empty info commands
--EXPECT--
OK
