--TEST--
 PUT with initkey digest and option is POLICY_KEY_SEND.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testPutInitKeyWithDigestAndOptionKeySendPositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutInitKeyWithDigestAndOptionKeySendPositive");
--EXPECT--
OK
