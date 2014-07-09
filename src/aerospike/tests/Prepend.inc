<?php
require_once 'Common.inc';

/**
 *Basic Prepend  opeartion tests
*/

class Prepend extends AerospikeTestCommon
{
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
    function testPrependOnBinValue() {
	$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "10000");
	$db->put($key,array("Greet"=>"World"));
	$res = $db->prepend($key, 'Greet', 'Hello ');
	$this->assertTrue($res, $msg = "Prepend Success.");
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
    function testPrependKeyNotExist() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "-----10000");
        $res = $db->prepend($key, 'Greet', ' World');
        $this->assertTrue($res, $msg = "Key not exists.");
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
    function testPrependBinNotExist() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "10000");
        $res = $db->prepend($key, 'Greet1234', ' World');
        $this->assertTrue($res, $msg = "Bin not exists.");
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
    function testPrependValueNotString() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "10000");
        $res = $db->prepend($key, 'Greet', 888888);
        $this->assertTrue($res, $msg = "Prepend value not string ");
    }
}
?>
