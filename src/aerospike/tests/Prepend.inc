<?php
require_once 'Common.inc';

/**
 *Basic Prepend  opeartion tests
*/

class Prepend extends AerospikeTestCommon
{
    protected function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        if (!$this->db->isConnected()) {
            return $this->db->errorno();
        }
        $key = $this->db->initKey("test", "demo", "Prepend_key");
        $this->db->put($key,array("Greet"=>"World_end"));
        $this->keys[] = $key;
    }

    /**
     * @test
     * Basic prepend opeartion on bin value
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPrependOnBinValue)
     *
     * @test_plans{1.1}
     */
    function testPrependOnBinValue() {
        return($this->db->prepend($this->keys[0], 'Greet', 'Hello ', array(Aerospike::OPT_WRITE_TIMEOUT=>1000)));
    }
    /**
     * @test
     * Basic bin value prepend operation when key not exist
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPrependKeyNotExist)
     *
     * @test_plans{1.1}
     */
    function testPrependKeyNotExist() {
        $key1 = $this->db->initKey("test", "demo", "---sss----");
        return $this->db->prepend($key1, 'Greet', ' World', array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
    }
    /**
     * @test
     * Basic bin prepend operatrion when not exist
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPrependBinNotExist)
     *
     * @test_plans{1.1}
     */
        function testPrependBinNotExist() {
            return $this->db->prepend($this->keys[0], 'sssssssssss', ' World', array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_ONCE));
    }
    /**
     * @test
     * Basic bin value prepend when prepend value not string
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPrependValueNotString)
     *
     * @test_plans{1.1}
     */
    function testPrependValueNotString() {
        $opts = array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE,
            Aerospike::OPT_WRITE_TIMEOUT=>1000,
            Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND);
        return $this->db->prepend($this->keys[0], 'Greet', 888888, $opts);
    }
}
?>
