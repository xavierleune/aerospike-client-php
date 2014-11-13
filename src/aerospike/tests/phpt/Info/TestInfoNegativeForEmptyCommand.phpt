--TEST--
Info - Check for empty command name

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Info", "testInfoNegativeForEmptyCommand");
--XFAIL--
Should fail until the C client can accept null/empty info commands
--EXPECT--
OK
