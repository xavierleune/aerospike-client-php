<?php
require_once 'Common.inc';

/**
 *Basic Append  opeartion tests
*/

class Append  extends AerospikeTestCommon
{
    protected function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        if (!$this->db->isConnected()) {
            return $this->db->errorno();
        }
        $key = array("ns" => "test", "set" => "demo","key" =>"Append_key");
        $this->db->put($key,array("Greet"=>"World_end"));
        $this->keys[] = $key;
   }

    /**
     * @test
     * Basic append opeartion on bin value
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAppendOnBinValue)
     *
     * @test_plans{1.1}
     */
     function testAppendOnBinValue() {
         return ($this->db->append($this->keys[0], 'Greet', ' World',
             array(Aerospike::OPT_WRITE_TIMEOUT=>1000,
             Aerospike::OPT_POLICY_COMMIT_LEVEL=>Aerospike::POLICY_COMMIT_LEVEL_MASTER)));
    }
    /**
     * @test
     * Basic bin value append operation when key not exist
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAppendKeyNotExist)
     *
     * @test_plans{1.1}
     */
    function testAppendKeyNotExist() {
        $key = array("ns" => "test", "set" => "demo", "key" => "---------AppendKey-------");
        $this->keys[] = $key;
        return($this->db->append($key, 'Greet', ' World',
                array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE,
                Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST)));
    }
    /**
     * @test
     * Basic bin append operatrion when not exist
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAppendBinNotExist)
     *
     * @test_plans{1.1}
     */
    function testAppendBinNotExist() {
        $opts = array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE, 
            Aerospike::OPT_WRITE_TIMEOUT=>1000,
            Aerospike::OPT_POLICY_COMMIT_LEVEL=>Aerospike::POLICY_COMMIT_LEVEL_ALL);
        if ($this->db->isConnected()) {
            return($this->db->append($this->keys[0], 'Greet1234', ' World', $opts));
        } else {
            return($this->db->errorno());
        }
    }
    /**
     * @test
     * Basic bin value append when append value not string
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAppendValueNotString)
     *
     * @test_plans{1.1}
     */
    function testAppendValueNotString() {
        if ($this->db->isConnected()) {
            return($this->db->append($this->keys[0], 'Greet', 888888));
        } else {
            return($this->db->errorno());
        }
    }
}
?>
