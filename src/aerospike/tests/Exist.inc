<?php
require_once 'Common.inc';

/**
 *Basic Key Exists/getMetaData opeartion tests
*/

class Exist extends AerospikeTestCommon
{
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
    function testKeyExist() {
	$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "10000");
	$res = $db->exists($key, $metadata);
	$this->assertTrue($res, $msg = "Key exists.");
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
    function testKeyNotExist() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "-----10000");
        $res = $db->exists($key, $metadata);
        $this->assertTrue($res, $msg = "Key not exists.");
    }
}
