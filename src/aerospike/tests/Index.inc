<?php
require_once 'Common.inc';

/**
 *Basic Index creation and drop tests
*/

class Index extends AerospikeTestCommon
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
        $key = $db->initKey("test", "demo", "index_test");
        $db->put($key, array("bin1"=>"1", "bin2"=>2, "bin3"=>array(1,2,3)));
   }

   function tearDown(&$db, &$key) {
       $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
           =>Aerospike::POLICY_RETRY_NONE));
       $db->close();
   }

    /**
     * @test
     * Basic index create operation on integer bin
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexStringBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateIndexIntegerBinPositive($db, $key) {
        if ($db->isConnected()) {
            return($db->createIndex("test", "demo", "bin2",
                Aerospike::INDEX_TYPE_INTEGER, "bin2_integer_index"));
        } else {
            return($db->errorno());
        }
    }

    /**
     * @test
     * Basic index create operation on string bin
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexIntegerBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateIndexStringBinPositive($db, $key) {
        if ($db->isConnected()) {
            return($db->createIndex("test", "demo", "bin1",
                Aerospike::INDEX_TYPE_STRING, "bin1_string_index"));
        } else {
            return($db->errorno());
        }
    }
}
?>
