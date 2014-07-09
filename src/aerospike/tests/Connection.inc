<?php

require_once 'Common.inc';
/**
 *Basic connection tests
*/

class Connection extends AerospikeTestCommon
{
    /**
     * @test
     * Basic Connection
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testConnect)
     *
     * @test_plans{1.1}
     */
    function testConnect() {
	$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $handle = new Aerospike($config);
        $this->assertNotNull($handle, $msg = "Aerospike Object");
    }
    /**
     * @test
     * Connection parameter array
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCheckParameterArray)
     *
     * @test_plans{1.1}
     */
    function testCheckParameterArray() {
	$config = "";
        $handle = new Aerospike($config);
        $this->assertNotNull($handle, $msg = "Aerospike Object");
    }
    /**
     * @test
     * Empty array for config
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testEmptyArray)
     *
     * @test_plans{1.1}
     */
    function testEmptyArray() {
	$config = array("hosts"=>array(array("name"=>"", "port"=>"")));
        $handle = new Aerospike($config);
        $this->assertNotNull($handle, $msg = "Aerospike Object");
    }
    /**
     * @test
     * Missing Host Key
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testMissingHostKeyFromConfigArray)
     *
     * @test_plans{1.1}
     */
    function testMissingHostsKeyFromConfigArray() {
	$config = array(array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $handle = new Aerospike($config);
        $this->assertNotNull($handle, $msg = "Aerospike Object");
    }
    /**
     * @test
     * Missing Name Key
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testMissingNameKeyFromConfigArray)
     *
     * @test_plans{1.1}
     */
    function testMissingNameKeyFromConfigArray() {
	$config = array("hosts"=>array(array("0"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $handle = new Aerospike($config);
        $this->assertNotNull($handle, $msg = "Aerospike Object");
    }
    /**
     * @test
     * Missing Port Key
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testMissingPortKeyFromConfigArray)
     *
     * @test_plans{1.1}
     */
    function testMissingPortKeyFromConfigArray() {
	$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "qw"=>AEROSPIKE_CONFIG_PORT)));
        $handle = new Aerospike($config);
        $this->assertNotNull($handle, $msg = "Aerospike Object");
    }
    /**
     * @test
     * Multiple Hosts
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testMultipleHostsArray)
     *
     * @test_plans{1.1}
     */
    function testMultipleHostsArray() {
	$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT),array("name"=>"10.71.71.53", "port"=>3000)));
        $handle = new Aerospike($config);
        $this->assertNotNull($handle, $msg = "Aerospike Object");
    }
    /**
     * @test
     * Port value is string
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPortValueIsString)
     *
     * @test_plans{1.1}
     */
    function testPortValueIsString() {
	$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>"3000")));
        $handle = new Aerospike($config);
        $this->assertNotNull($handle, $msg = "Aerospike Object");
    }
    /**
     * @test
     * Port and name sequence changed in array
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testNamePortSequenceInConfig)
     *
     * @test_plans{1.1}
     */
    function testNamePortSequenceInConfig() {
        $config = array("hosts"=>array(array("port"=>AEROSPIKE_CONFIG_PORT,"name"=>AEROSPIKE_CONFIG_NAME)));
        $handle = new Aerospike($config);
        $this->assertNotNull($handle, $msg = "Aerospike Object");
    }
}
?>
