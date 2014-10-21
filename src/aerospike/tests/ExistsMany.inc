<?php
require_once 'Common.inc';

/**
 *Basic Key Exists/getMetaData opeartion tests
*/

class ExistsMany extends AerospikeTestCommon
{
   protected function setUp(&$db, &$key) {
       $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
       $db = new Aerospike($config);
   }

   protected function tearDown(&$db, &$key) {
       for($i = 0;$i < 3;$i++) { 
           $key = $db->initKey("test", "demo", "existsMany".($i+1));
           $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>Aerospike::POLICY_RETRY_NONE));
       }
       $db->close();
   }
    /**
     * @test
     * Basic existsMany operation with all arguments.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testExistsManyPositive)
     *
     * @test_plans{1.1}
     */
   function testExistsManyPositive($db, $key) {
       if ($db->isConnected()) {
           $key1 = $db->initKey("test", "demo", "existsMany1");
           $db->put($key,array("binA"=>10));
           $key2 = $db->initKey("test", "demo", "existsMany2");
           $db->put($key,array("binB"=>20));
           $key3 = $db->initKey("test", "demo", "existsMany3");
           $db->put($key,array("binC"=>30));

           $keys = array($key1, $key2, $key3);
           $status = $db->existsMany($keys, $metadata,
               array(Aerospike::OPT_READ_TIMEOUT=>3000)));
           if ($status != AEROSPIKE_OK) {
               return $db->errorno();
           }
           if (empty($metadata)) {
               return Aerospike::ERR;
           }
       } else {
           return $db->errorno();
       }
   }
    /**
     * @test
     * Basic existsMany operation with empty key array.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testExistsManyEmptyKeyArrayNegative)
     *
     * @test_plans{1.1}
     */
    function testExistsManyEmptyKeyArrayNegative($db, $key) {
        if ($db->isConnected()) {
            $key = $db->initKey("test", "demo", "existsMany1");
            $db->put($key,array("binA"=>10));
            $key = $db->initKey("test", "demo", "existsMany2");
            $db->put($key,array("binB"=>20));
            $key = $db->initKey("test", "demo", "existsMany3");
            $db->put($key,array("binC"=>30));

            $key_array = array();
            return($db->existsMany($key_array, $metadata));
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic existsMany operation with empty string passed 
       in place of key array .
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testExistsManyKeyEmptyStringNegative)
     *
     * @test_plans{1.1}
     */
    function testExistsManyKeyEmptyStringNegative($db, $key) {
        if ($db->isConnected()) {
            $key_array = "";
            return($db->existsMany($key_array, $metadata));
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic existsMany operation with no arguments.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testExistsManyNoArgumentsNegative)
     *
     * @test_plans{1.1}
     */
    function testExistsManyNoArgumentsNegative($db, $key) {
        if ($db->isConnected()) {
            return($db->existsMany());
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic existsMany, Metadata is empty string variable.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testExistsManyMetadataEmptyStringNegative)
     *
     * @test_plans{1.1}
     */
    function testExistsManyMetadataEmptyStringNegative($db, $key) {
        if ($db->isConnected()) {
            $keys = array($key1, $key2, $key3);
            return($db->existsMany($keys, ""));
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic existsMany, Metadata is of type string.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testExistsManyMetadataStringVariableNegative)
     *
     * @test_plans{1.1}
     */
    function testExistsManyMetadataStringVariableNegative($db, $key) {
        if ($db->isConnected()) {
            $keys = array($key1, $key2, $key3);
            return($db->existsMany($keys, "abc"));
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic existsMany, Options is of type integer.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testExistsManyOptionsIntegerNegative)
     *
     * @test_plans{1.1}
     */
    function testExistsManyOptionsIntegerNegative($db, $key) {
        if ($db->isConnected()) {
            $keys = array($key1, $key2, $key3);
            return($db->existsMany($keys, $metadata, 20));
        } else {
            return($db->errorno());
        }
    }
}
