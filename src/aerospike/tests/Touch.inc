<?php
require_once 'Common.inc';

/**
 *Basic touch  opeartion tests
*/

class Touch extends AerospikeTestCommon
{
    /**
     * @test
     * Basic Touch operation
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBasicTouchOpeartion)
     *
     * @test_plans{1.1}
     */
    function testBasicTouchOpeartion() {
	$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "10000");
	$res = $db->touch($key, 120);
	$this->assertTrue($res, $msg = "Added 120 seconds to the record's expiration.");
    }
    /**
     * @test
     * Basic Touch operation when key not exist
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testTouchOperationKeyNotExist)
     *
     * @test_plans{1.1}
     */
    function testTouchOperationKeyNotExist() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "-----10000");
        $res = $db->touch($key, 120);
        $this->assertTrue($res, $msg = "Key not exists.");
    }
}
?>
