--TEST--
PUT with generation policy POLICY_GEN_GT and no generation value.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testPutGenPolicyGTPositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutGenPolicyGTPositive");
--EXPECT--
ERR_RECORD_GENERATION
