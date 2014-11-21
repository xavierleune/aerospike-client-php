<?php
require_once 'Common.inc';

/**
 * initKey helper function tests
*/

class InitKey extends AerospikeTestCommon
{
    protected function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        if (!$this->db->isConnected()) {
            return $this->db->errorno();
        }
    }
    /**
     * @test
     * Basic initKey operation
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testInitKeyHelper)
     *
     * @test_plans{1.1}
     */
    function testInitKeyHelper() {
        $key = $this->db->initKey("demo", "test", 10000);
        if (!$key) {
            return Aerospike::ERR;
        } else {
            return Aerospike::OK;
        }
    }
    /**
     * @test
     * Basic initKey namespace value integer
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testNameSpaceValue)
     *
     * @test_plans{1.1}
     */
    function testNameSpaceValueInt() {
        $key = $this->db->initKey(2312312, "test", 10000);
        if (!$key) {
            return Aerospike::ERR;
        } else {
            return Aerospike::OK;
        }
    }
    /**
     * @test
     * Basic initKey set value integer
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testSetValueInt)
     *
     * @test_plans{1.1}
     */
    function testSetValueInt() {
        $key = $this->db->initKey("demo", 12312312, 10000);
        if (!$key) {
            return Aerospike::ERR;
        } else {
            return Aerospike::OK;
        }
    }
}
?>
