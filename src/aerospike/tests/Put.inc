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
        /**
         * @test
         * Basic PUT Check sequence of key parameter in Key array
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
                $rv = $as->put($key,array("bin1"=>"Hello World"));
        	$this->assertTrue($rv, $msg = "Aerospike Put Response");
    	}
/**
  * @test
  * Basic PUT Check key parameter value non exist in database
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
      $rv = $as->put($key,array("bin1"=>"Hello World"));
      $this->assertTrue($rv, $msg = "Aerospike Put Response");
  }

/**
  * @test
  * Basic PUT Check namespace parameter value non exist in database
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
      $key = array("ns"=>"aksdksajfasjk","set"=>"demo","key"=>"10000");
      $rv = $as->put($key,array("bin1"=>"Hello World"));
      $this->assertTrue($rv, $msg = "Aerospike Put Response");
  }

/**
  * @test
  * Basic PUT Check Set parameter value non exist in database
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
      $key = array("ns"=>"test","set"=>"sadklasndaasd","key"=>"10000");
      $rv = $as->put($key,array("bin1"=>"Hello World"));
      $this->assertTrue($rv, $msg = "Aerospike Put Response");
  }
/**
  * @test
  * Basic PUT Insert list 
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
      $this->assertTrue($rv, $msg = "Aerospike Put Response");
  }
/**
  * @test
  * Basic PUT Insert Map 
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
      $this->assertTrue($rv, $msg = "Aerospike Put Response");
  }
/**
  * @test
  * Basic PUT Insert Map and List combine 
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
      $this->assertTrue($rv, $msg = "Aerospike Put Response");
  }
}
?>
