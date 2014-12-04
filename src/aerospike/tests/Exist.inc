<?php
require_once 'Common.inc';

/**
 *Basic Key Exists/getMetaData opeartion tests
*/

class Exist extends AerospikeTestCommon
{
    protected function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        if (!$this->db->isConnected()) {
            return $this->db->errorno();
        }
        $key = $this->db->initKey("test", "demo", "exist_key");
        $this->db->put($key,array("Greet"=>"World_end"));
        $this->keys[] = $key;
    }

    /**
     * @test
     * Basic key exist/getmedata operation
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyExist)
     *
     * @test_plans{1.1}
     */
   function testKeyExist() {
       return $this->db->exists($this->keys[0], $metadata,
           array(Aerospike::OPT_READ_TIMEOUT=>3000,
           Aerospike::OPT_POLICY_REPLICA=>Aerospike::POLICY_REPLICA_MASTER,
           Aerospike::OPT_POLICY_CONSISTENCY=>Aerospike::POLICY_CONSISTENCY_ONE));
    }
    /**
     * @test
     * Basic key exist operation when key not exist
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyNotExist)
     *
     * @test_plans{1.1}
     */
    function testKeyNotExist() {
        $key1 = $this->db->initKey("test", "demo", "----sss---------");
        return $this->db->exists($key1, $metadata);
    }
}
