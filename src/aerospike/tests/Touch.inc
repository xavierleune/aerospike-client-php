<?php
require_once 'Common.inc';

/**
 *Basic touch  opeartion tests
*/

class Touch extends AerospikeTestCommon
{

    protected function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        if (!$this->db->isConnected()) {
            return $this->db->errorno();
        }
        $key = $this->db->initKey("test", "demo", "touch_key");
        $this->db->put($key, array("Greet"=>"World_end"));
        $this->keys[] = $key;
    }
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

    function testBasicTouchOperation() {
        $rv = $this->db->put($this->keys[0], array("bin1"=>"Hello World"));
        return ($this->db->touch($this->keys[0], 10,
            array(Aerospike::OPT_WRITE_TIMEOUT=>200,
            Aerospike::OPT_POLICY_COMMIT_LEVEL=>Aerospike::POLICY_COMMIT_LEVEL_MASTER)));
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
        $key1 = array("ns" => "test", "set" => "demo", "key" => "-----sshh---");
        return ($this->db->touch($key1, 120, array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE)));
    }
}
?>
