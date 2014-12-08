--TEST--
Apply UDF on record, Where UDF puts bytes array in DB.

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Udf", "testUdfPositiveApplyPutByteArray");
--EXPECT--
OK
