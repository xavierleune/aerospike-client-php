<?php
require_once 'Common.inc';

/**
 *Basic remove opeartion tests
*/

class Remove extends AerospikeTestCommon
{
    protected function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        if (!$this->db->isConnected()) {
            return $this->db->errorno();
        }
        $key = $this->db->initKey("test", "demo", "remove_test");
        $this->db->put($key,array("bin1"=>"1"));
        $this->keys[] = $key;
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
    function testKeyRemove() {
        return ($this->db->remove($this->keys[0], array(Aerospike::OPT_POLICY_RETRY =>Aerospike::POLICY_RETRY_NONE,
                Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND)));
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
        $key1 = array("ns" => "test", "set" => "demo", "key" => "-----ss-----");
        return ($this->db->remove($key1, array(Aerospike::OPT_POLICY_RETRY => Aerospike::POLICY_RETRY_NONE,
                Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST)));
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
        return $this->db->removeBin($this->keys[0], array("bin1"));
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
        return($this->db->removeBin($this->keys[0], array("-------------bin1------------")));
    }
}
?>
