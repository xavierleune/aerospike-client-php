<?php
require_once 'Common.inc';

/**
 *Basic Append  opeartion tests
*/

class Append  extends AerospikeTestCommon
{
    /**
     * @test
     * Basic append opeartion on bin value
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAppendOnBinValue)
     *
     * @test_plans{1.1}
     */
    function testAppendOnBinValue() {
	$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "10000");
	$db->put($key,array("Greet"=>"Hello"));
	$res = $db->append($key, 'Greet', ' World');
	$this->assertTrue($res, $msg = "Append Success.");
    }
    /**
     * @test
     * Basic bin value append operation when key not exist
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAppendKeyNotExist)
     *
     * @test_plans{1.1}
     */
    function testAppendKeyNotExist() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "-----10000");
        $res = $db->append($key, 'Greet', ' World');
        $this->assertTrue($res, $msg = "Key not exists.");
    }
    /**
     * @test
     * Basic bin append operatrion when not exist
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAppendBinNotExist)
     *
     * @test_plans{1.1}
     */
    function testAppendBinNotExist() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "10000");
        $res = $db->append($key, 'Greet1234', ' World');
        $this->assertTrue($res, $msg = "Bin not exists.");
    }
    /**
     * @test
     * Basic bin value append when append value not string
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAppendValueNotString)
     *
     * @test_plans{1.1}
     */
    function testAppendValueNotString() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = array("ns" => "demo", "set" => "test", "key" => "10000");
        $res = $db->append($key, 'Greet', 888888);
        $this->assertTrue($res, $msg = "Append value not string ");
    }
}
?>
