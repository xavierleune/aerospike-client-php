<?php
require_once 'Common.inc';

/**
 *Basic Key Exists/getMetaData opeartion tests
*/

class GetMany extends AerospikeTestCommon
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
     * Basic getMany operation with all arguments.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGetManyPositive)
     *
     * @test_plans{1.1}
     */
   function testGetManyPositive($db, $key) {
       if ($db->isConnected()) {
           $key1 = $db->initKey("test", "demo", "getMany1");
           $put_record1 = array("binA"=>10);
           $db->put($key, $put_record1);
           $key2 = $db->initKey("test", "demo", "getMany2");
           $put_record2 = array("binB"=>20);
           $db->put($key, $put_record2);
           $key3 = $db->initKey("test", "demo", "getMany3");
           $put_record3 = array("binC"=>30);
           $db->put($key, $put_record3);

           $keys = array($key1, $key2, $key3);
           $status = $db->getMany($keys, $records,
               array(Aerospike::OPT_READ_TIMEOUT=>3000)));
           if ($status != AEROSPIKE_OK) {
               return $db->errorno();
           }
           if (empty($records)) {
               return Aerospike::ERR;
           }

           $i = 1;
           foreach($records as $records_key=>$value) {
               if (strcmp($(key.$i)["key"], $records_key)) {
                   if (!empty(array_diff_assoc_recursive($(put_record.$i),
                       $value))) {
                           return Aerospike::ERR;
                       }
               }
           }
       } else {
           return $db->errorno();
       }
       return $status;
   }
    /**
     * @test
     * Basic getMany operation with empty key array.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGetManyEmptyKeyArrayNegative)
     *
     * @test_plans{1.1}
     */
    function testGetManyEmptyKeyArrayNegative($db, $key) {
       if ($db->isConnected()) {
           $key1 = $db->initKey("test", "demo", "getMany1");
           $put_record1 = array("binA"=>10);
           $db->put($key, $put_record1);
           $key2 = $db->initKey("test", "demo", "getMany2");
           $put_record2 = array("binB"=>20);
           $db->put($key, $put_record2);
           $key3 = $db->initKey("test", "demo", "getMany3");
           $put_record3 = array("binC"=>30);
           $db->put($key, $put_record3);

           $keys = array();
           $status = $db->getMany($keys, $records,
               array(Aerospike::OPT_READ_TIMEOUT=>3000)));
           if ($status != AEROSPIKE_OK) {
               return $db->errorno();
           }
           if (empty($records)) {
               return Aerospike::ERR;
           }

           $i = 1;
           foreach($records as $records_key=>$value) {
               if (strcmp($(key.$i)["key"], $records_key)) {
                   if (!empty(array_diff_assoc_recursive($(put_record.$i),
                       $value))) {
                           return Aerospike::ERR;
                       }
               }
           }
       } else {
           return $db->errorno();
       }
       return $status;
    }
    /**
     * @test
     * Basic getMany operation with empty string passed 
       in place of key array .
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGetManyKeyEmptyStringNegative)
     *
     * @test_plans{1.1}
     */
    function testGetManyKeyEmptyStringNegative($db, $key) {
       if ($db->isConnected()) {
           $key1 = $db->initKey("test", "demo", "getMany1");
           $put_record1 = array("binA"=>10);
           $db->put($key, $put_record1);
           $key2 = $db->initKey("test", "demo", "getMany2");
           $put_record2 = array("binB"=>20);
           $db->put($key, $put_record2);
           $key3 = $db->initKey("test", "demo", "getMany3");
           $put_record3 = array("binC"=>30);
           $db->put($key, $put_record3);

           $keys = array();
           $status = $db->getMany("", $records,
               array(Aerospike::OPT_READ_TIMEOUT=>3000)));
           if ($status != AEROSPIKE_OK) {
               return $db->errorno();
           }
           if (empty($records)) {
               return Aerospike::ERR;
           }

           $i = 1;
           foreach($records as $records_key=>$value) {
               if (strcmp($(key.$i)["key"], $records_key)) {
                   if (!empty(array_diff_assoc_recursive($(put_record.$i),
                       $value))) {
                           return Aerospike::ERR;
                       }
               }
           }
       } else {
           return $db->errorno();
       }
       return $status;
    }
    /**
     * @test
     * Basic getMany operation with no arguments.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGetManyNoArgumentsNegative)
     *
     * @test_plans{1.1}
     */
    function testGetManyNoArgumentsNegative($db, $key) {
       if ($db->isConnected()) {
           $key1 = $db->initKey("test", "demo", "getMany1");
           $put_record1 = array("binA"=>10);
           $db->put($key, $put_record1);
           $key2 = $db->initKey("test", "demo", "getMany2");
           $put_record2 = array("binB"=>20);
           $db->put($key, $put_record2);
           $key3 = $db->initKey("test", "demo", "getMany3");
           $put_record3 = array("binC"=>30);
           $db->put($key, $put_record3);

           $keys = array();
           $status = $db->getMany();
           if ($status != AEROSPIKE_OK) {
               return $db->errorno();
           }
           if (empty($records)) {
               return Aerospike::ERR;
           }

           $i = 1;
           foreach($records as $records_key=>$value) {
               if (strcmp($(key.$i)["key"], $records_key)) {
                   if (!empty(array_diff_assoc_recursive($(put_record.$i),
                       $value))) {
                           return Aerospike::ERR;
                       }
               }
           }
       } else {
           return $db->errorno();
       }
       return $status;
    }
    /**
     * @test
     * Basic getMany, Metadata is empty string variable.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGetManyMetadataEmptyStringNegative)
     *
     * @test_plans{1.1}
     */
    function testGetManyMetadataEmptyStringNegative($db, $key) {
       if ($db->isConnected()) {
           $key1 = $db->initKey("test", "demo", "getMany1");
           $put_record1 = array("binA"=>10);
           $db->put($key, $put_record1);
           $key2 = $db->initKey("test", "demo", "getMany2");
           $put_record2 = array("binB"=>20);
           $db->put($key, $put_record2);
           $key3 = $db->initKey("test", "demo", "getMany3");
           $put_record3 = array("binC"=>30);
           $db->put($key, $put_record3);

           $keys = array();
           $status = $db->getMany($keys, "",
               array(Aerospike::OPT_READ_TIMEOUT=>3000)));
           if ($status != AEROSPIKE_OK) {
               return $db->errorno();
           }
           if (empty($records)) {
               return Aerospike::ERR;
           }

           $i = 1;
           foreach($records as $records_key=>$value) {
               if (strcmp($(key.$i)["key"], $records_key)) {
                   if (!empty(array_diff_assoc_recursive($(put_record.$i),
                       $value))) {
                           return Aerospike::ERR;
                       }
               }
           }
       } else {
           return $db->errorno();
       }
       return $status;
    }
    /**
     * @test
     * Basic getMany, Metadata is of type string.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGetManyMetadataStringVariableNegative)
     *
     * @test_plans{1.1}
     */
    function testGetManyMetadataStringVariableNegative($db, $key) {
       if ($db->isConnected()) {
           $key1 = $db->initKey("test", "demo", "getMany1");
           $put_record1 = array("binA"=>10);
           $db->put($key, $put_record1);
           $key2 = $db->initKey("test", "demo", "getMany2");
           $put_record2 = array("binB"=>20);
           $db->put($key, $put_record2);
           $key3 = $db->initKey("test", "demo", "getMany3");
           $put_record3 = array("binC"=>30);
           $db->put($key, $put_record3);

           $keys = array();
           $status = $db->getMany($keys, "abc",
               array(Aerospike::OPT_READ_TIMEOUT=>3000)));
           if ($status != AEROSPIKE_OK) {
               return $db->errorno();
           }
           if (empty($records)) {
               return Aerospike::ERR;
           }

           $i = 1;
           foreach($records as $records_key=>$value) {
               if (strcmp($(key.$i)["key"], $records_key)) {
                   if (!empty(array_diff_assoc_recursive($(put_record.$i),
                       $value))) {
                           return Aerospike::ERR;
                       }
               }
           }
       } else {
           return $db->errorno();
       }
       return $status;
    }
    /**
     * @test
     * Basic getMany, Options is of type integer.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGetManyOptionsIntegerNegative)
     *
     * @test_plans{1.1}
     */
    function testGetManyOptionsIntegerNegative($db, $key) {
       if ($db->isConnected()) {
           $key1 = $db->initKey("test", "demo", "getMany1");
           $put_record1 = array("binA"=>10);
           $db->put($key, $put_record1);
           $key2 = $db->initKey("test", "demo", "getMany2");
           $put_record2 = array("binB"=>20);
           $db->put($key, $put_record2);
           $key3 = $db->initKey("test", "demo", "getMany3");
           $put_record3 = array("binC"=>30);
           $db->put($key, $put_record3);

           $keys = array();
           $status = $db->getMany($keys, $records,
               12));
           if ($status != AEROSPIKE_OK) {
               return $db->errorno();
           }
           if (empty($records)) {
               return Aerospike::ERR;
           }

           $i = 1;
           foreach($records as $records_key=>$value) {
               if (strcmp($(key.$i)["key"], $records_key)) {
                   if (!empty(array_diff_assoc_recursive($(put_record.$i),
                       $value))) {
                           return Aerospike::ERR;
                       }
               }
           }
       } else {
           return $db->errorno();
       }
       return $status;
    }
}
