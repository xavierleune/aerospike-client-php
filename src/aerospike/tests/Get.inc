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
        $this->assertTypeArray($bin_arr, $msg = "Aerospike Get Response");
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
        $this->assertTypeArray($ret_arr, $msg = "Aerospike Get Response");
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
        /**
         * @test
         * Basic GET Check parameter sequence change in  key array
         *
         * @pre
         * Connect using aerospike object to the specified node
         *
         * @post
         * newly initialized Aerospike objects
         *
         * @remark
         * Variants: OO (testCheckParameterSequenceChangeInKeyArray)
         *
         * @test_plans{1.1}
         */
        function testCheckParameterSequenceChangeInKeyArray()
        {
                $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
                $as = new Aerospike($config);
                $key = array("set"=>"demo","ns"=>"test","key"=>"10000");
                $ret=array();
                $rv = $as->get($key,$ret);
        	$this->assertTrue($rv, $msg = "Aerospike Get Response");
    	}
        /**
         * @test
         * Basic GET Check set parameter value non exist in database
         *
         * @pre
         * Connect using aerospike object to the specified node
         *
         * @post
         * newly initialized Aerospike objects
         *
         * @remark
         * Variants: OO (testCheckSetValueNotExistInDB)
         *
         * @test_plans{1.1}
         */
        function testCheckSetValueNotExistInDB()
        {
                $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
                $as = new Aerospike($config);
                $key = array("ns"=>"demo","set"=>"adfhashfag98712asd","key"=>"10000");
                $ret=array();
                $rv = $as->get($key,$ret);
                $this->assertTrue($rv, $msg = "Aerospike Get Response");
        }
/**
  * @test
  * Basic GET Check namespace parameter value non exist in database
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testCheckNameSpaceValueNotExistInDB)
  *
  * @test_plans{1.1}
  */
  function testCheckNameSpaceValueNotExistInDB()
  {
      $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
      $as = new Aerospike($config);
      $key = array("ns"=>"kjahdjasdsa","set"=>"demo","key"=>"10000");
      $ret=array();
      $rv = $as->get($key,$ret);
      $this->assertTrue($rv, $msg = "Aerospike Get Response");
  }

/**
  * @test
  * Basic GET Check key parameter value non exist in database
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testCheckKeyValueNotExistInDB)
  *
  * @test_plans{1.1}
  */
  function testCheckKeyValueNotExistInDB()
  {
      $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
      $as = new Aerospike($config);
      $key = array("ns"=>"test","set"=>"demo","key"=>"-0000000000010000");
      $ret=array();
      $rv = $as->get($key,$ret);
      $this->assertTrue($rv, $msg = "Aerospike Get Response");
  }
/**
  * @test
  * Basic Get list from single bin
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testCheckListInsert)
  *
  * @test_plans{1.1}
  */
  function testCheckListInsert()
  {
      $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
      $as = new Aerospike($config);
      $key = array("ns"=>"test","set"=>"demo","key"=>"10000");
      $rv = $as->put($key,array("bin1"=>array("speaking", "reading", "writing")));
      $ret=array();
      $rv = $as->get($key,$ret);
      $this->assertTrue($rv, $msg = "Aerospike Get Response");
  }
/**
  * @test
  * Basic Get Map from single bin
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testCheckMapInsert)
  *
  * @test_plans{1.1}
  */
  function testCheckMapInsert()
  {
      $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
      $as = new Aerospike($config);
      $key = array("ns"=>"test","set"=>"demo","key"=>"10000");
      $rv = $as->put($key,array("bin1"=>("month"=>"May", "year"=>2013)));
      $ret=array();
      $rv = $as->get($key,$ret);
      $this->assertTrue($rv, $msg = "Aerospike Get Response");
  }
/**
  * @test
  * Basic Get Map and List combine from single bin
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testCheckListMapCombineInsert)
  *
  * @test_plans{1.1}
  */
  function testCheckListMapCombineInsert()
  {
      $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
      $as = new Aerospike($config);
      $key = array("ns"=>"test","set"=>"demo","key"=>"10000");
      $softskills = array("speaking", "reading", "writing");
      $skills = array("python", "c", "java",$softskills);
      $innermostmap = array("month"=>"May", "year"=>2013);
      $innermap1 = array("degree_name"=>"B.E.", "degree_branch"=>"Comp Sci", "degree_date"=>$innermostmap);
      $innermap = array("degree"=>$innermostmap, "aggregate"=>70);
      $mymap = array("name"=>"aero", "age"=>23, "edu"=>$innermap, "skills"=>$skills);
      $innermostlist = array("twitter", 100);
      $innerlist = array(1,2,"facebook", $innermostlist);
      $mylist = array(10, 20, "whatsup", $innerlist, $mymap);
      $rec = array("bin1"=>$mylist);	
      $rv = $as->put($key,$rec);
      $ret=array();
      $rv = $as->get($key,$ret);
      $this->assertTrue($rv, $msg = "Aerospike Get Response");
  }
/**
  * @test
  * Basic Get single bin missing
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testCheckBinMissing)
  *
  * @test_plans{1.1}
  */
  function testCheckBinMissing()
  {
      $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
      $as = new Aerospike($config);
      $key = array("ns"=>"test","set"=>"demo","key"=>"10000");
      $ret=array();
      $rv = $as->get($key,$ret,array("bin1","bin2","00000000000bin1000000"));
      $this->assertTrue($rv, $msg = "Aerospike Get Response");
  }
}  
?>l
