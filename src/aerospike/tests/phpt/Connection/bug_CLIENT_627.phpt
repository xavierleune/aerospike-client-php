--TEST--
connect() with COMPRESSION_THRESHOLD option passed.

--DESCRIPTION--
connect() with config option COMPRESSION_THRESHOLD

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "bug_CLIENT_627");
?>

--EXPECT--
OK
