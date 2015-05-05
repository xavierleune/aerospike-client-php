--TEST--
Info - Check for incorrect port no

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Info", "testInfoNegativeForIncorrectPort");
--EXPECT--
ERR_TIMEOUT
