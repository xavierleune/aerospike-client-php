<?php
require_once 'Common.inc';

/**
 *Basic remove opeartion tests
*/

class Remove extends AerospikeTestCommon
{

    protected function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("test", "demo", "remove_test");
        if ($db->isConnected()) {
            $db->put($key,array("bin1"=>"1"));
        }
    }

    protected function tearDown(&$db, &$key) {
        if ($db->isConnected()) {
            $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                =>Aerospike::POLICY_RETRY_NONE));
        }
    }

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
    function testKeyRemove($db, $key) {
        if ($db->isConnected()) {
            return ($db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                =>Aerospike::POLICY_RETRY_NONE)));
        } else {
            return($db->errorno());
        }

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
    function testKeyRemoveKeyNotExist($db, $key) {
        $key1 = array("ns" => "test", "set" => "demo", "key" => "-----ss-----");
        if ($db->isConnected()) {
            return ($db->remove($key1, array(Aerospike::OPT_POLICY_RETRY =>
                Aerospike::POLICY_RETRY_NONE)));
        } else {
            return($db->errorno());
        }
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
    function testBinRemove($db, $key) {
        if ($db->isConnected()) {
            return $db->removeBin($key, array("bin1"));
        } else {
            return($db->errorno());
        }
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
    function testBinRemoveBinNotExist($db, $key) {
        if ($db->isConnected()) {
            return($db->removeBin($key, array("-------------bin1------------")));
        } else {
            return($db->errorno());
        }
    }
}
?>
