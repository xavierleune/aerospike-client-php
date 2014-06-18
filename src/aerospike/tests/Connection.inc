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
     * Variants: OO (testConnectBasic)
     *
     * @test_plans{1.1}
     */
    function testConnectBasic() {
$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $handle = new Aerospike($config);

        $this->assertNotNull($handle, $msg = "Aerospike Object");
    }
	/**
     * @test
     * Basic PUT
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPUT)
     *
     * @test_plans{1.1}
     */
function testPUT() {
$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $as = new Aerospike($config);
	   $key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
	   $rv = $as->put($key,array("bin1"=>"Hello World"));
        $this->assertTrue($rv, $msg = "Aerospike Put Response");
    }
	/**
     * @test
     * Basic GET
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGET)
     *
     * @test_plans{1.1}
     */
function testGET() {
$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $as = new Aerospike($config);
	$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
	$arr = array();
	$temp = $as->get($key,$arr);
	//print_r($arr);
        $this->assertTypeArray($arr, $msg = "Aerospike get Response");
	//return true;
    }   
  }
?>
