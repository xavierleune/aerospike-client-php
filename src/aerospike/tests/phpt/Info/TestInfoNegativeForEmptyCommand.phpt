--TEST--
Info - Check for empty command name

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Info", "testInfoNegativeForEmptyCommand");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Info", "testInfoNegativeForEmptyCommand");
--EXPECT--
OK
