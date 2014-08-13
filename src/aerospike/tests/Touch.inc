<?php
require_once 'Common.inc';

/**
 *Basic touch  opeartion tests
*/

class Touch extends AerospikeTestCommon
{   
      
    protected function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("test", "demo", "touch_key");
        $db->put($key, array("Greet"=>"World_end"));
    }

    protected function tearDown(&$db, &$key) {
        $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>Aerospike::POLICY_RETRY_NONE));
        $db->close();
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

    function testBasicTouchOperation($db, $key) {
        $rv = $db->put($key, array("bin1"=>"Hello World"));
        if ($db->isConnected()) {
        return ($db->touch($key, 10));
        } else {
            return($db->errorno());
        }
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
        if ($db->isConnected()) {
            return ($db->touch($key1, 120));
        } else {
            return($db->errorno());
        }
    }
}
?>
