<?php
require_once 'Common.inc';

/**
 *Basic Append  opeartion tests
*/

class Append  extends AerospikeTestCommon
{
    protected function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "test", "set" => "demo","key" =>"Append_key");
        $db->put($key,array("Greet"=>"World_end"));
   }

    function tearDown(&$db, &$key) {
        if ($db->isConnected()) {
            $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>Aerospike::POLICY_RETRY_NONE));
            $db->close();
        }
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
        function testAppendOnBinValue($db, $key) {
        if ($db->isConnected()) {
            return($db->append($key, 'Greet', ' World',
                array(Aerospike::OPT_WRITE_TIMEOUT=>1000)));
        } else {
            return($db->errorno());
        }
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
    function testAppendKeyNotExist($db, $key) {
        $key1 = array("ns" => "test", "set" => "demo", "key" =>
              "---------AppendKey-------");
        if ($db->isConnected()) {
            return($db->append($key1, 'Greet', ' World',
                array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE)));
        } else {
            return($db->errorno());
        }
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
    function testAppendBinNotExist($db, $key) {
        $opts = array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE, 
                     Aerospike::OPT_WRITE_TIMEOUT=>1000);
        if ($db->isConnected()) {
            return($db->append($key, 'Greet1234', ' World', $opts));
        } else {
            return($db->errorno());
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
    function testAppendValueNotString($db, $key) {
        if ($db->isConnected()) {
            return($db->append($key, 'Greet', 888888));
        } else {
            return($db->errorno());
        }
    }
}
?>
