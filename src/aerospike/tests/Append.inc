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
        $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>Aerospike::POLICY_RETRY_NONE));
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
        return($db->append($key, 'Greet', ' World'));
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
        return($db->append($key1, 'Greet', ' World'));
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
        return($db->append($key, 'Greet1234', ' World'));
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
        return($db->append($key, 'Greet', 888888));
    }
}
?>
