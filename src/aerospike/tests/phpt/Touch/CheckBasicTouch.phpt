--TEST--
Touch - Basic Touch opeartion

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Touch", "testBasicTouchOpeartion");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Touch", "testBasicTouchOpeartion");
--EXPECT--
