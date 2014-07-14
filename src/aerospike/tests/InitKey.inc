<?php
require_once 'Common.inc';

/**
 * initKey helper function tests
*/

class InitKey extends AerospikeTestCommon
{
    /**
     * @test
     * Basic initKey operation
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testInitKeyHelper)
     *
     * @test_plans{1.1}
     */
    function testInitKeyHelper() {
	$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("demo", "test", 10000);
	$this->assertTrue($res, $msg = "Key array returned.");
    }
    /**
     * @test
     * Basic initKey namespace value integer
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testNameSpaceValue)
     *
     * @test_plans{1.1}
     */
    function testNameSpaceValueInt() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey(2312312, "test", 10000);
        $this->assertTrue($res, $msg = "Namespace value must not  be integer.");
    }
    /**
     * @test
     * Basic initKey set value integer
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testSetValueInt)
     *
     * @test_plans{1.1}
     */
    function testSetValueInt() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("demo", 12312312, 10000);
        $this->assertTrue($res, $msg = "Set value must not be integer.");
    }
}
?>
