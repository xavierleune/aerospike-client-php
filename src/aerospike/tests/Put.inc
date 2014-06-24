<?php

require_once 'Common.inc';
/**
 *Basic Put tests
*/

class Put extends AerospikeTestCommon
{
	/**
     * @test
     * Basic Single PUT
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
	function testPUT() 
	{
		$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$as = new Aerospike($config);
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
		$rv = $as->put($key,array("bin1"=>"Hello World"));
        $this->assertTrue($rv, $msg = "Aerospike Put Response");
    }
	/**
     * @test
     * Basic Multi PUT
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testMultiPUT)
     *
     * @test_plans{1.1}
     */
	function testMultiPUT() 
	{
		$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$as = new Aerospike($config);
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
		$rv = $as->put($key,array("bin1"=>"Hello World","bin2"=>123456));
        $this->assertTrue($rv, $msg = "Aerospike Put Response");
    }
	/**
     * @test
     * Put with no parameter
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPUTNoParameter)
     *
     * @test_plans{1.1}
     */
	function testPUTNoParameter() 
	{
		$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$as = new Aerospike($config);
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
		$rv = $as->put();
        $this->assertTrue($rv, $msg = "Aerospike Put Response");
    }
	/**
     * @test
     * Put with only key parameter
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPUTNoParameter)
     *
     * @test_plans{1.1}
     */
	function testPUTWithOnlyKeyParameter() 
	{
		$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$as = new Aerospike($config);
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
		$rv = $as->put($key);
        $this->assertTrue($rv, $msg = "Aerospike Put Response");
    }
	/**
     * @test
     * Put without key parameter
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPUTNoParameter)
     *
     * @test_plans{1.1}
     */
	function testPUTWithOnlyDataParameter() 
	{
		$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$as = new Aerospike($config);
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
		$rv = $as->put(array("bin1"=>"Hello World"));
        $this->assertTrue($rv, $msg = "Aerospike Put Response");
    }
	/**
     * @test
     * Put data as normal array ex. array("hi","hello")
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPUTNoParameter)
     *
     * @test_plans{1.1}
     */
	function testPUTWithNormalpPutDataArrayParameter() 
	{
		$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$as = new Aerospike($config);
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
		$rv = $as->put($key,array("Hello World"));
        $this->assertTrue($rv, $msg = "Aerospike Put Response");
    }
	/**
	 * @test
	 * Basic PUT Check key parameter missing in key array
	 *
	 * @pre
	 * Connect using aerospike object to the specified node
	 *
	 * @post
	 * newly initialized Aerospike objects
	 *
	 * @remark
	 * Variants: OO (testCheckKeyParameterMissingInKeyArray)
	 *
	 * @test_plans{1.1}
	 */
	function testCheckKeyParameterMissingInKeyArray() 
	{
		$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$as = new Aerospike($config);
		$key = array("ns"=>"test", "set"=>"demo");
		$rv = $as->put($key,array("bin1"=>"Hello World"));
        $this->assertTrue($rv, $msg = "Aerospike Put Response");
    }
    /**
	 * @test
	 * Basic PUT Check ns(namespace) parameter missing in key array
	 *
	 * @pre
	 * Connect using aerospike object to the specified node
	 *
	 * @post
	 * newly initialized Aerospike objects
	 *
	 * @remark
	 * Variants: OO (testCheckNameSpaceParameterMissingInKeyArray)
	 *
	 * @test_plans{1.1}
	 */
	function testCheckNameSpaceParameterMissingInKeyArray() 
	{
		$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$as = new Aerospike($config);
		$key = array("set"=>"demo","key"=>"10000");
		$rv = $as->put($key,array("bin1"=>"Hello World"));
        $this->assertTrue($rv, $msg = "Aerospike Put Response");
    }
	/**
	 * @test
	 * Basic PUT Check set parameter missing in key array
	 *
	 * @pre
	 * Connect using aerospike object to the specified node
	 *
	 * @post
	 * newly initialized Aerospike objects
	 *
	 * @remark
	 * Variants: OO (testCheckSetParameterMissingInKeyArray)
	 *
	 * @test_plans{1.1}
	 */
	function testCheckSetParameterMissingInKeyArray() 
	{
		$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$as = new Aerospike($config);
		$key = array("ns"=>"test","key"=>"10000");
		$rv = $as->put($key,array("bin1"=>"Hello World"));
        $this->assertTrue($rv, $msg = "Aerospike Put Response");
    }
	/**
	 * @test
	 * Basic PUT Second Parameter Array test
	 *
	 * @pre
	 * Connect using aerospike object to the specified node
	 *
	 * @post
	 * newly initialized Aerospike objects
	 *
	 * @remark
	 * Variants: OO (testCheckSecondParameterTypeArray)
	 *
	 * @test_plans{1.1}
	 */
	function testCheckSecondParameterTypeArray() 
	{
		$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$as = new Aerospike($config);
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
		$bin_arr = "";
		$rv = $as->put($key,$bin_arr);
        $this->assertTrue($rv, $msg = "Aerospike Put Response");
    }
}
?>
