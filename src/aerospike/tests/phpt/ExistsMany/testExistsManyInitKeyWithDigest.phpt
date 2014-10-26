--TEST--
 Basic existsMany, initKey with digest.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("ExistsMany", "testExistsManyInitKeyWithDigest");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ExistsMany", "testExistsManyInitKeyWithDigest");
--EXPECT--
OK
