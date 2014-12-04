<?php
require_once 'Common.inc';

/**
 *Basic Increment opeartion tests
*/

class Increment extends AerospikeTestCommon
{
    function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        $key = $this->db->initKey("test", "demo", "increment_test");
        $this->db->put($key, array("bin1"=>"1"));
        $this->keys[] = $key;
   }

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
        return $this->db->increment($this->keys[0], 'bin1', 4, 0,
            array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST,
            Aerospike::OPT_POLICY_COMMIT_LEVEL=>Aerospike::POLICY_COMMIT_LEVEL_MASTER));
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
        $key1 = array("ns" => "test", "set" => "demo", "key" => "----ssss----");
        return $this->db->increment($key1, 'bin1', 4, 0,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_ONCE,
            Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND));
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
        return $this->db->increment($this->keys[0], 'abc', 4);
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
    function testBinIncrementOffsetValueString()
    {
        try {
            $this->db->increment($this->keys[0], 'bin1', "abc");
        } catch (ErrorException $e) {
            return Aerospike::OK;
        }
        return Aerospike::ERR_CLIENT;
    }
}
?>
