<?php
require_once 'Common.inc';

/**
 *Basic remove opeartion tests
*/

class Remove extends AerospikeTestCommon
{
    /**
     * @test
     * Basic key remove operation
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyRemove)
     *
     * @test_plans{1.1}
     */
    function testKeyRemove() {
	$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "10000");
	$res = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY => Aerospike::POLICY_RETRY_NONE));
	$this->assertTrue($res, $msg = "Key removed.");
    }
    /**
     * @test
     * Basic key remove operation when key not exist
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
    function testKeyRemoveKeyNotExist() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "-----10000");
        $res = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY => Aerospike::POLICY_RETRY_NONE));
        $this->assertTrue($res, $msg = "Key not exists.");
    }
    /**
     * @test
     * Basic bin remove operation
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinRemove)
     *
     * @test_plans{1.1}
     */
    function testBinRemove() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "10000");
        $res= $db->removeBin($key, "bin1");
        $this->assertTrue($res, $msg = "Bin removed.");
    }
    /**
     * @test
     * Basic key remove operation when key not exist
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinRemoveBinNotExist)
     *
     * @test_plans{1.1}
     */
    function testBinRemoveBinNotExist() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "10000");
        $res= $db->removeBin($key, "-------------bin1------------");
        $this->assertTrue($res, $msg = "Bin not exists.");
    }
}
?>
