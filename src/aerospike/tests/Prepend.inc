<?php
require_once 'Common.inc';

/**
 *Basic Prepend  opeartion tests
*/

class Prepend extends AerospikeTestCommon
{
    protected function setUp(&$db, &$key) {
	    $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("test", "demo", "Prepend_key");
	    $db->put($key,array("Greet"=>"World_end"));
    }

    protected function tearDown(&$db, &$key) {
        $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
            =>Aerospike::POLICY_RETRY_NONE));
        $db->close();
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
    function testPrependOnBinValue($db, $key) {
        if ($db->isConnected()) {
            return($db->prepend($key, 'Greet', 'Hello ',
                array(Aerospike::OPT_WRITE_TIMEOUT=>1000)));
        } else {
            return($db->errorno());
        }
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
    function testPrependKeyNotExist($db, $key) {
        $key1 = $db->initKey("test", "demo", "---sss----");
        if ($db->isConnected()) {
            return($db->prepend($key1, 'Greet', ' World',
                array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE)));
        } else {
            return($db->errorno());
        }
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
        function testPrependBinNotExist($db, $key) {
        if ($db->isConnected()) {
            return($db->prepend($key, 'sssssssssss', ' World',
                array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_ONCE)));
        } else {
            return($db->errorno());
        }
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
    function testPrependValueNotString($db, $key) {
        $opts = array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE,
            Aerospike::OPT_WRITE_TIMEOUT=>1000,
            Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND);
        if ($db->isConnected()) {
            return($db->prepend($key, 'Greet', 888888, $opts));
        } else {
            return($db->errorno());
        }
    }
}
?>
