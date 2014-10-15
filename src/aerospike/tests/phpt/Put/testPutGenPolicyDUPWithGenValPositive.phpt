--TEST--
PUT with generation policy POLICY_GEN_DUP and generation value.  

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testPutGenPolicyDUPWithGenValPositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutGenPolicyDUPWithGenValPositive");
--EXPECT--
OK
