--TEST--
PUT with initkey digest and option is POLICY_KEY_DIGEST.  

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testPutInitKeyWithDigestAndOptionKeyDigestPositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put",
"testPutInitKeyWithDigestAndOptionKeyDigestPositive");
--EXPECT--
OK
