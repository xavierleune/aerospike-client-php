--TEST--
createIndex and dropIndex - Very Long Index Name

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Index", "testCreateAndDropIndexIntegerBinVeryLongIndexNameNegativeWithDBCrash");
--EXPECTF--
%r(ERR_INDEX_NAME_MAXLEN|ERR_CLIENT)%r

