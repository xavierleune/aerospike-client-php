--TEST--
createIndex and dropIndex - Invalid namespace

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Index", "testCreateAndDropIndexIntegerBinInvalidNamespaceNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Index", "testCreateAndDropIndexIntegerBinInvalidNamespaceNegative");
--EXPECT--
ERR_REQUEST_INVALID

