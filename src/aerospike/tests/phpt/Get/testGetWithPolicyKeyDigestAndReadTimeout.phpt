--TEST--
 Get a record from DB, POLICY_KEY_DIGEST and read timeout is passed in
 options. 

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Get", "testGetWithPolicyKeyDigestAndReadTimeout");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetWithPolicyKeyDigestAndReadTimeout");
--EXPECT--
OK
