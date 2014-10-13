--TEST--
createIndex and dropIndex - Very Long Index Name

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Index", "testCreateAndDropIndexIntegerBinVeryLongIndexNameNegativeWithDBCrash");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Index", "testCreateAndDropIndexIntegerBinVeryLongIndexNameNegativeWithDBCrash");
--EXPECT--
AEROSPIKE_ERR_INDEX_NAME_MAXLEN

