<?php
require_once 'Common.inc';

/**
 *Basic Increment opeartion tests
*/

class Increment extends AerospikeTestCommon
{
    /*
     * TODO: Remove these constants and map with actual Aerospike class
     * constants.
     */
    static $status_ok = 0;
    static $status_err = 201;

    function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("test", "demo", "increment_test");
        $db->put($key,array("bin1"=>"1"));
   }

   function tearDown(&$db, &$key) {
       $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
           =>Aerospike::POLICY_RETRY_NONE));
       $db->close();
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
    function testBinIncrementByOffsetValue($db, $key) {
        if ($db->isConnected()) {
            return($db->increment($key, 'bin1', 4, 0,
                array(Aerospike::OPT_WRITE_TIMEOUT=>100,
                Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST)));
        } else {
            return($db->errorno());
        }
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
    function testBinIncrementKeyNotExist($db, $key) {
        $key1 = array("ns" => "test", "set" => "demo", "key" => "----ssss----");
        if ($db->isConnected()) {
            return($db->increment($key1, 'bin1', 4, 0,
                array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_ONCE,
                Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND)));
        } else {
            return($db->errorno());
        }
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
    function testBinIncrementBinNotExist($db, $key) {
        if ($db->isConnected()) {
            return($db->increment($key, 'abc', 4));
        } else {
            return($db->errorno());
        }
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
    function testBinIncrementOffsetValueString($db, $key)
    {   
        if ($db->isConnected()) {
        try {
            $db->increment($key, 'bin1', "abc");
        } catch (ErrorException $e) {
            return self::$status_ok;
        }
        return self::$status_err;
        } else {
            return($db->errorno());
        }
    }
}
?>
