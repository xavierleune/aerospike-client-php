--TEST--
PUT with generation policy POLICY_GEN_IGNORE and generation value.  

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testPutGenPolicyIgnoreWithGenValPositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutGenPolicyIgnoreWithGenValPositive");
--EXPECT--
OK
