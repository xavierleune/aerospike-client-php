<?php
require_once 'Common.inc';

/**
 *Basic Key Exists/getMetaData opeartion tests
*/

class Exist extends AerospikeTestCommon
{
   protected function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("test", "demo", "exist_key");
        $db->put($key,array("Greet"=>"World_end"));
   }

   protected function tearDown(&$db, &$key) {
        $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>Aerospike::POLICY_RETRY_NONE));
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
    function testKeyExist($db, $key) {
        return($db->exists($key, $metadata));
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
    function testKeyNotExist($db, $key) {
        $key1 = $db->initKey("test", "demo", "----sss---------");
        return($db->exists($key1, $metadata));
    }
}
