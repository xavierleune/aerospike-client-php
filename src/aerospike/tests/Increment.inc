<?php
require_once 'Common.inc';

/**
 *Basic Increment opeartion tests
*/

class Increment extends AerospikeTestCommon
{
    /**
     * @test
     * Basic bin increment operation
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinIncrementByOffsetValue)
     *
     * @test_plans{1.1}
     */
    function testBinIncrementByOffsetValue() {
	$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "10000");
	$res = $db->increment($key, 'bin1', 4);
	$this->assertTrue($res, $msg = "Bin value increment by 4.");
    }
    /**
     * @test
     * Basic bin increment operation when key not exist
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyRemoveKeyNotExist)
     *
     * @test_plans{1.1}
     */
    function testBinIncrementKeyNotExist() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "-----10000");
	$res = $db->increment($key, 'bin1', 4);
        $this->assertTrue($res, $msg = "Key not exists.");
    }
    /**
     * @test
     * Basic bin increment when bin not exist
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinIncrementBinNotExist)
     *
     * @test_plans{1.1}
     */
    function testBinIncrementBinNotExist() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "10000");
        $res = $db->increment($key, 'abc', 4);
        $this->assertTrue($res, $msg = "Bin created with default initial value.");
    }
    /**
     * @test
     * Basic bin increment when offset value string
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinIncrementOffsetValueString)
     *
     * @test_plans{1.1}
     */
    function testBinIncrementOffsetValueString() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "10000");
        $res = $db->increment($key, 'bin10001', "abc");
        $this->assertTrue($res, $msg = "Offset value must be integer.");
    }
}
?>
