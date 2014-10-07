--TEST--
 Get a record from DB, POLICY_KEY_DIGEST is passed as a option. 

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testGetWithPolicyKeyDigest");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetWithPolicyKeyDigest");
--EXPECT--
OK
