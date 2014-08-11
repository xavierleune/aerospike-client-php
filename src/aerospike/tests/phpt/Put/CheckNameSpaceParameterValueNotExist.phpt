--TEST--
Put - Check NameSpace Value not exist in Database

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Put", "testCheckNameSpaceValueNotExistInDB");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testCheckNameSpaceValueNotExistInDB");
--EXPECT--
ERR_REQUEST_INVALID
