<?php
require_once 'Common.inc';

/**
 *Basic touch  opeartion tests
*/

class Touch extends AerospikeTestCommon
{
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
    protected function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("test", "demo", "touch_key");
        $db->put($key, array("Greet"=>"World_end"));
    }

    protected function tearDown(&$db, &$key) {
        $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>Aerospike::POLICY_RETRY_NONE));
    }

    function testBasicTouchOperation($db, $key) {
        $rv = $db->put($key, array("bin1"=>"Hello World"));
        return ($db->touch($key, 10));
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
    function testTouchOperationKeyNotExist($db, $key) {
        $key1 = array("ns" => "test", "set" => "demo", "key" => "-----sshh---");
        return ($db->touch($key1, 120));
    }
}
?>
