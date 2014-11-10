--TEST--
Drop user - drop user with incorrect policy

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("DropUser", "testDropUserUserPositiveWithIncorrectPolicy");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("DropUser", "testDropUserUserPositiveWithIncorrectPolicy");
--EXPECT--
ERR_CLIENT
