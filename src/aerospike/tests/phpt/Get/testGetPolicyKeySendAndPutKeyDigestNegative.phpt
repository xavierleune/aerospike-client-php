--TEST--
 Get a record from DB, initkey is done with digest and while put
 POLICY_KEY_DIGEST is passed and for get POLICY_KEY_SEND is passed in
 options. 

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
//aerospike_phpt_skipif("Get", "testGetPolicyKeySendAndPutKeyDigestNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
//aerospike_phpt_runtest("Get", "testGetPolicyKeySendAndPutKeyDigestNegative");
--EXPECT--
OK
