--TEST--
Touch - Basic Touch opeartion

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Touch", "testBasicTouchOperation");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Touch", "testBasicTouchOperation");
--EXPECT--
OK
