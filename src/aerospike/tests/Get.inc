<?php

require_once 'Common.inc';
/**
 *Basic Get tests
*/

class Get extends AerospikeTestCommon
{
	/**
	 * @test
	 * Basic GET without parameter
	 *
	 * @pre
	 * Connect using aerospike object to the specified node
	 *
	 * @post
	 * newly initialized Aerospike objects
	 *
	 * @remark
	 * Variants: OO (testGETNoParameter)
	 *
	 * @test_plans{1.1}
	 */
	function testGETNoParameter() 
	{
		$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$as = new Aerospike($config);
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
		$rv = $as->get();
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
	 * Variants: OO (testGETTwoParameter)
	 *
	 * @test_plans{1.1}
	 */
	function testGETTwoParameter() 
	{
		$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$as = new Aerospike($config);
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
		$bin_arr = array();
		$rv = $as->get($key,$bin_arr);
        $this->assertTrue($rv, $msg = "Aerospike Put Response");
    }
	/**
	 * @test
	 * Basic GET Three Parameter
	 *
	 * @pre
	 * Connect using aerospike object to the specified node
	 *
	 * @post
	 * newly initialized Aerospike objects
	 *
	 * @remark
	 * Variants: OO (testGETThreeParameter)
	 *
	 * @test_plans{1.1}
	 */
	function testGETThreeParameter() 
	{
		$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$as = new Aerospike($config);
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
		$ret_arr = array();
		$bin_arr = array("bin1");
		$rv = $as->get($key,$ret_arr,$bin_arr);
        $this->assertTrue($rv, $msg = "Aerospike Get Response");
    }
	/**
	 * @test
	 * Basic GET Three Parameter
	 *
	 * @pre
	 * Connect using aerospike object to the specified node
	 *
	 * @post
	 * newly initialized Aerospike objects
	 *
	 * @remark
	 * Variants: OO (testGETThreeParameter)
	 *
	 * @test_plans{1.1}
	 */
	function testGETWrongParameter() 
	{
		$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$as = new Aerospike($config);
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
		$rv = $as->get($key);
        $this->assertTrue($rv, $msg = "Aerospike Get Response");
    }
	/**
	 * @test
	 * Basic GET Second Parameter Array
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
		$ret = "";
		$rv = $as->get($key,$ret);
        $this->assertTrue($rv, $msg = "Aerospike Get Response");
    }
	/**
	 * @test
	 * Basic GET Third Parameter Array
	 *
	 * @pre
	 * Connect using aerospike object to the specified node
	 *
	 * @post
	 * newly initialized Aerospike objects
	 *
	 * @remark
	 * Variants: OO (testCheckThirdParameterTypeArray)
	 *
	 * @test_plans{1.1}
	 */
	function testCheckThirdParameterTypeArray() 
	{
		$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$as = new Aerospike($config);
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
		$ret = array();
		$bin_arr = "";
		$rv = $as->get($key,$ret,$bin_arr);
        $this->assertTrue($rv, $msg = "Aerospike Get Response");
    }
	/**
	 * @test
	 * Basic GET Check key parameter missing in key array
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
		$ret=array();
		$rv = $as->get($key,$ret);
        $this->assertTrue($rv, $msg = "Aerospike Get Response");
    }	

  /**
	 * @test
	 * Basic GET Check ns(namespace) parameter missing in key array
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
		$ret=array();
		$rv = $as->get($key,$ret);
        $this->assertTrue($rv, $msg = "Aerospike Get Response");
    }
	/**
	 * @test
	 * Basic GET Check set parameter missing in key array
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
		$ret=array();
		$rv = $as->get($key,$ret);
        $this->assertTrue($rv, $msg = "Aerospike Get Response");
    }
  }
  
?>
