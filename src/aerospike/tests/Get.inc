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
    static $status_ok = 2000;
    static $status_err = 2001;

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
        if ($db->isConnected()) { 
        try {
            $db->get();
        } catch (ErrorException $e) {
            return self::$status_ok;
        }
        return self::$status_err;
        } else {
            return($db->errorno());
        }
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
        if ($db->isConnected()) {  
            return($db->get($key,$bin_arr));
        } else {
            return($db->errorno());
        }
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
		$bin_arr = array("bin1");
        if ($db->isConnected()) {  
            return($db->get($key,$rec_arr,$bin_arr));
        } else {
            return($db->errorno());
        }
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
        if ($db->isConnected()) {
        try {
		    $db->get($key);
        } catch (ErrorException $e) {
            return self::$status_ok;
        }
        return self::$status_err;
        } else {
            return($db->errorno());
        }
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
        $rec_array = "";
        if ($db->isConnected()) {  
            return($db->get($key, $rec_array));
        } else {
            return($db->errorno());
        }
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
          
        if ($db->isConnected()) {  
        try {
            $db->get($key, $rec, "");
        } catch (ErrorException $e) {
            return self::$status_ok;
        }
        return self::$status_err;
        } else {
            return($db->errorno());
        }

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
        if ($db->isConnected()) {  
        try {
            $key1 = $db->initKey("test", "demo");
            $rv = $db->get($key1, $rec);
        } catch (ErrorException $e) {
            return self::$status_ok;
        }
        return self::$status_err;
        } else {
            return($db->errorno());
        }
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
        if ($db->isConnected()) {  
            return($db->get($key1, $rec));
        } else { 
            return($db->errorno());
        }
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
        if ($db->isConnected()) {  
            return($db->get($key1,$rec));
        } else {
            return($db->errorno());
        }
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
                if ($db->isConnected()) {  
                    return($db->get($key1,$rec));
                } else {
                    return($db->errorno());
                }
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
                if ($db->isConnected()) {  
                    return($db->get($key1,$rec));
                } else {
                    return($db->errorno());
                }
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
      if ($db->isConnected()) {  
          return($db->get($key1,$rec));
      } else {
          return($db->errorno());
      }

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
      if ($db->isConnected()) {  
          return($db->get($key1,$rec));
      } else {
          return($db->errorno());
      }
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
      if ($db->isConnected()) {  
          $status = $db->put($key1,array("bin1"=>array("speaking", "reading", "writing")));
      } else{
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          return($status);
      }
      if ($db->isConnected()) {  
          $status = $db->get($key1,$rec);
      } else {
          return($db->errorno());
      }

      if($status != Aerospike::OK) {
          return($status);
      }
      if ($db->isConnected()) {  
      $status = $db->remove($key1, array(Aerospike::OPT_POLICY_RETRY
          =>Aerospike::POLICY_RETRY_NONE));
      } else {
          return($db->errorno());
      }
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
      if ($db->isConnected()) {  
          $status = $db->put($key1, array("bin1"=>$map1));
      } else {
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          return($status);
      }
      if ($db->isConnected()) {  
          $status = $db->get($key1,$ret);
      } else {
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          return($status);
      }
      if ($db->isConnected()) {  
      $status = $db->remove($key1, array(Aerospike::OPT_POLICY_RETRY
          =>Aerospike::POLICY_RETRY_NONE));
      } else {
          return($db->errorno());
      }
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
      if ($db->isConnected()) {  
          $status = $db->put($key1,$rec);
      } else {
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          return($status);
      }
      if ($db->isConnected()) {  
          $status = $db->get($key1,$ret);
      } else {
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          return($status);
      }
      if ($db->isConnected()) {  
      $status = $db->remove($key1, array(Aerospike::OPT_POLICY_RETRY
          =>Aerospike::POLICY_RETRY_NONE));
      } else {
          return($db->errorno());
      }
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
      if ($db->isConnected()) {  
          return($db->get($key,$rec,array("bin1","bin2","00000000000bin1000000")));
      } else {
          return($db->errorno());
      }
  }
}  
?>
