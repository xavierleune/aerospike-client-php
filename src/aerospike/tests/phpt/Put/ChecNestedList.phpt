--TEST--
Put - PUT With nested List.

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testNestedList");
--EXPECT--
OK
