<?php

require_once 'Common.inc';
/**
 *Basic Get tests
*/

class Get extends AerospikeTestCommon
{
    /*
     * TODO: Remove these constants and map with actual Aerospike class
     * constants.
     */
    static $status_ok = 0;
    static $status_err = 201;

    protected function setUp(&$db, &$key) {
	    $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("test", "demo", "Get_key");
	    $db->put($key,array("Greet"=>"World_end"));
    }

    protected function tearDown(&$db, &$key) {
        $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
            =>Aerospike::POLICY_RETRY_NONE));
    }
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
	function testGETNoParameter($db, $key) 
    {
        try {
            $db->get();
        } catch (ErrorException $e) {
            return self::$status_ok;
        }
        return self::$status_err;
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
	function testGETTwoParameter($db, $key) 
	{
		$bin_arr = array();
        return($db->get($key,$bin_arr));
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
	function testGETThreeParameter($db, $key) 
	{
		$ret_arr = array();
		$bin_arr = array("bin1");
		return($db->get($key,$ret_arr,$bin_arr));
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
	function testGETWrongParameter($db, $key) 
    {
        try {
		    $db->get($key);
        } catch (ErrorException $e) {
            return self::$status_ok;
        }
        return self::$status_err;
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
	function testCheckSecondParameterTypeArray($db, $key) 
	{
		$ret = "";
		return($db->get($key, $ret));
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
	function testCheckThirdParameterTypeArray($db, $key) 
    {
        try {
		    $bin_arr = "";
            $db->get($key, $ret, $bin_arr);
        } catch (ErrorException $e) {
            return self::$status_ok;
        }
        return self::$status_err;
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
	function testCheckKeyParameterMissingInKeyArray($db, $key) 
    {
        try {
            $key1 = $db->initKey("test", "demo");
		    $ret=array();
            $rv = $db->get($key1, $ret);
        } catch (ErrorException $e) {
            return self::$status_ok;
        }
        return self::$status_err;
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
	function testCheckNameSpaceParameterMissingInKeyArray($db, $key) 
	{
		$key1 = array("set"=>"demo", "key"=>"10000");
		$ret=array();
        return($db->get($key1, $ret));
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
	function testCheckSetParameterMissingInKeyArray($db, $key) 
	{
		$key1 = array("ns"=>"test","key"=>"10000");
		$ret=array();
        return($db->get($key1,$ret));
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
        function testCheckParameterSequenceChangeInKeyArray($db, $key)
        {
                $key1 = $db->initKey("demo", "test", "key_int1");
                $ret=array();
                return($db->get($key1,$ret));
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
        function testCheckSetValueNotExistInDB($db, $key)
        {
                $key1 = $db->initKey("test", "ssss---ssss", "key_int1");
                $ret=array();
                return($db->get($key1,$ret));
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
  function testCheckNameSpaceValueNotExistInDB($db, $key)
  {
      $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
      $db = new Aerospike($config);
      $key1 = $db->initKey("sss--ss", "demo", "key_int1");
      $ret=array();
      return($db->get($key1,$ret));
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
  function testCheckKeyValueNotExistInDB($db, $key)
  {
      $key1 = $db->initKey("test", "demo", "ss--sss");
      $ret=array();
      return($db->get($key1,$ret));
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
  function testCheckListInsert($db, $key)
  {
      $key1 = $db->initKey("test", "demo", "10000");
      $status = $db->put($key1,array("bin1"=>array("speaking", "reading", "writing")));
      if($status != Aerospike::OK) {
          return($status);
      }
      $ret=array();
      $status = $db->get($key1,$ret);
      if($status != Aerospike::OK) {
          return($status);
      }
      $status = $db->remove($key1, array(Aerospike::OPT_POLICY_RETRY
          =>Aerospike::POLICY_RETRY_NONE));
      return($status);
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
  function testCheckMapInsert($db, $key)
  {
      $key1 = $db->initkey("test", "demo", "10000");
      $map1= array("k1"=>10, "k2"=>5, "k3"=>6, "k4"=>7, "k5"=>8);
      $status = $db->put($key1, array("bin1"=>$map1));
      if($status != Aerospike::OK) {
          return($status);
      }
      $ret=array();
      $status = $db->get($key1,$ret);
      if($status != Aerospike::OK) {
          return($status);
      }
      $status = $db->remove($key1, array(Aerospike::OPT_POLICY_RETRY
          =>Aerospike::POLICY_RETRY_NONE));
      return($status);
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
  function testCheckListMapCombineInsert($db, $key)
  {
      $key1 = $db->initkey("test", "demo", "10000");
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
      $status = $db->put($key1,$rec);
      if($status != Aerospike::OK) {
          return($status);
      }
      $ret=array();
      $status = $db->get($key1,$ret);
      if($status != Aerospike::OK) {
          return($status);
      }
      $status = $db->remove($key1, array(Aerospike::OPT_POLICY_RETRY
          =>Aerospike::POLICY_RETRY_NONE));
      return($status);

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
  function testCheckBinMissing($db, $key)
  {
      $ret=array();
      return($db->get($key,$ret,array("bin1","bin2","00000000000bin1000000")));
  }
}  
?>
