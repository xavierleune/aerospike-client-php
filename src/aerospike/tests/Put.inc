<?php

require_once 'Common.inc';
/**
 *Basic Put tests
*/

class Employee 
{
    public $desg = 'Manager';
}
class Put extends AerospikeTestCommon
{
    /*
     * TODO: Remove these constants and map with actual Aerospike class
     * constants.
     */
    static $status_ok = 2000;
    static $status_err = 2001;

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
	function testPUT($dummydb, $dummykey) 
    {
		$config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$db = new Aerospike($config);
        $key = $db->initKey("test", "demo", "put_test");
        if ($db->isConnected()) {
            $status = $db->put($key,array("bin1"=>"Hello World"));
        } else{
            $db->close();
            return($db->errorno());
        }
        if ($status != Aerospike::OK) {
            $db->close();
            return($status);
        }
        if ($db->isConnected()) {
            $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                =>Aerospike::POLICY_RETRY_NONE));
            $db->close();
        } else{
            $db->close();
            return($db->errorno());
        }
        return($status);
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
	function testMultiPUT($dummydb, $dummykey) 
	{
		$config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$db = new Aerospike($config);
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"key_multi");
        if ($db->isConnected()) {
            $status = $db->put($key,array("bin1"=>"Hello
                World","bin2"=>123456));
        } else{
            $db->close();
            return($db->errorno());
        }
        if ($status != Aerospike::OK) {
            $db->close();
            return($status);
        }
        if ($db->isConnected()) {
            $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                =>Aerospike::POLICY_RETRY_NONE));
            $db->close();
        } else{
            $db->close();
            return($db->errorno());
        }
        return($status);
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
	function testPUTNoParameter($dummydb, $dummykey) 
    {
        try {
    		$config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
    		$db = new Aerospike($config);
    		$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
            if ($db->isConnected()) {
                $status = $db->put();
            } else{
                $db->close();
                return($db->errorno());
            }
        } catch (ErrorException $e) {
            $db->close();
            return self::$status_ok;
        }
        $db->close();
        return self::$status_err;
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
	function testPUTWithOnlyKeyParameter($dummydb, $dummykey) 
    {
        try {
    		$config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
    		$db = new Aerospike($config);
    		$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
            if ($db->isConnected()) {
                $status = $db->put($key);
            } else{
                $db->close();
                return($db->errorno());
            }
        } catch (ErrorException $e) {
            $db->close();
            return self::$status_ok;
        }
        $db->close();
        return self::$status_err;
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
	function testPUTWithOnlyDataParameter($dummydb, $dummykey) 
    {    
        try {
    		$config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
    		$db = new Aerospike($config);
    		$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
            if ($db->isConnected()) {
                $status = $db->put(array("bin1"=>"Hello World"));
            } else{
                $db->close();
                return($db->errorno());
            }
        } catch (ErrorException $e) {
            $db->close();
            return self::$status_ok;
        }
            $db->close();
            return self::$status_err;
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
	function testPUTWithNormalpPutDataArrayParameter($dummydb, $dummykey) 
	{
		$config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$db = new Aerospike($config);
        $key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
        if ($db->isConnected()) {
            $status = $db->put($key, array("Hello World"));
        } else{
            $db->close();
            return($db->errorno());
        }
        if ($status != Aerospike::OK) {
            $db->close();
            return($status);
        }
        if ($db->isConnected()) {
            $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                => Aerospike::POLICY_RETRY_NONE));
            $db->close();
        } else{
            $db->close();
            return($db->errorno());
        }
        return($status);
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
	function testCheckKeyParameterMissingInKeyArray($dummydb, $dummykey) 
	{
		$config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$db = new Aerospike($config);
		$key = array("ns"=>"test", "set"=>"demo");
        if ($db->isConnected()) {
            $status = $db->put($key,array("bin1"=>"Hello World"));
        } else{
            $db->close();
            return($db->errorno());
        }
        if ($status != Aerospike::OK) {
            $db->close();
            return($status);
        }
        if ($db->isConnected()) {
            $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                =>Aerospike::POLICY_RETRY_NONE));
            $db->close();
        } else{
            $db->close();
            return($db->errorno());
        }
        return($status);
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
	function testCheckNameSpaceParameterMissingInKeyArray($dummydb, $dummykey) 
    {
		$config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$db = new Aerospike($config);
		$key = array("set"=>"demo","key"=>"10000");
        if ($db->isConnected()) {
            $status = $db->put($key,array("bin1"=>"Hello World"));
        } else{
            $db->close();
            return($db->errorno());
        }
        if ($status != Aerospike::OK) {
            $db->close();
            return($status);
        }
        if ($db->isConnected()) {
            $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                =>Aerospike::POLICY_RETRY_NONE));
            $db->close();
        } else{
            $db->close();
            return($db->errorno());
        }
        return($status);
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
	function testCheckSetParameterMissingInKeyArray($dummydb, $dummykey) 
	{
		$config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
		$db = new Aerospike($config);
		$key = array("ns"=>"test","key"=>"10000");
        if ($db->isConnected()) {
            $status = $db->put($key,array("bin1"=>"Hello World"));
        } else{
            $db->close();
            return($db->errorno());
        }
        if ($status != Aerospike::OK) {
            $db->close();
            return($status);
        }
        if ($db->isConnected()) {
            $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                =>Aerospike::POLICY_RETRY_NONE));
            $db->close();
        } else{
            $db->close();
            return($db->errorno());
        }
        return($status);
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
	function testCheckSecondParameterTypeArray($dummydb, $dummykey) 
    {    
        try {
    		$config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
    		$db = new Aerospike($config);
    		$key = array("ns"=>"test", "set"=>"demo", "key"=>"10000");
    		$bin_arr = "";
            if ($db->isConnected()) {
                $status = $db->put($key,$bin_arr);
            } else {
                $db->close();
                return($db->errorno());
            }
        } catch (ErrorException $e) {
            $db->close();
            return self::$status_ok;
        }
        $db->close();
        return self::$status_err;
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
        function testCheckParameterSequenceChangeInKeyArray($dummydb, $dummykey)
        {
             $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
             $db = new Aerospike($config);
             $key = $db->initKey("demo", "test", "key_int1");
             if ($db->isConnected()) {
                 $status = $db->put($key,array("bin1"=>"Hello World"));
             } else{
                 $db->close();
                 return($db->errorno());
             }
             if($status != Aerospike::OK) {
                 $db->close();
                 return($status);
             }
             if ($db->isConnected()) {
                  $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                      =>Aerospike::POLICY_RETRY_NONE));
                  $db->close();
             } else {
                $db->close();
                return($db->errorno());
             }
                return($status);
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
  function testCheckKeyValueNotExistInDB($dummydb, $dummykey)
  {
      $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
      $db = new Aerospike($config);
      $key = array("ns"=>"test","set"=>"demo","key"=>"-------------");
      if ($db->isConnected()) {
          $status = $db->put($key,array("bin1"=>"Hello World"), 500,
              array(Aerospike::OPT_POLICY_RETRY =>Aerospike::POLICY_RETRY_NONE));
      } else {
          $db->close();
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          $db->close();
          return($status);
      }
      if ($db->isConnected()) {
          $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
              =>Aerospike::POLICY_RETRY_NONE));
          $db->close();
      } else {
          $db->close();
          return($db->errorno());
      }
      return($status);
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
  function testCheckNameSpaceValueNotExistInDB($dummydb, $dummykey)
  {
      $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
      $db = new Aerospike($config);
      $key = array("ns"=>"aksdksajfasjk","set"=>"demo","key"=>"10000");
      if ($db->isConnected()) {
          $status = $db->put($key,array("bin1"=>"Hello World"));
      } else {
          $db->close();
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          $db->close();
          return($status);
      }
      if ($db->isConnected()) {
          $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
              =>Aerospike::POLICY_RETRY_NONE));
          $db->close();
      } else {
          $db->close();
          return($db->errorno());
      }
      return($status);
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
  function testCheckSetValueNotExistInDB($dummydb, $dummykey)
  {
      $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
      $db = new Aerospike($config);
      $key = array("ns"=>"test","set"=>"sadklasndaasd","key"=>"10000");
      if ($db->isConnected()) {
          $status = $db->put($key,array("bin1"=>"Hello World"));
      } else {
          $db->close();
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          $db->close();
          return($status);
      }
      if ($db->isConnected()) {
          $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
              =>Aerospike::POLICY_RETRY_NONE));
          $db->close();
      } else {
          return($db->errorno());
      }
      $db->close();
      return($status);
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
  function testCheckListInsert($dummydb, $dummykey)
  {
      $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
      $db = new Aerospike($config);
      $key = array("ns"=>"test","set"=>"demo","key"=>"10000");
      if ($db->isConnected()) {
          $status = $db->put($key,array("bin1"=>array("speaking",
              "reading", "writing", 100,
              array(Aerospike::OPT_POLICY_EXISTS =>Aerospike::POLICY_EXISTS_CREATE))));
      } else {
          $db->close();
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          $db->close();
          return($status);
      }
      if ($db->isConnected()) {
          $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
              =>Aerospike::POLICY_RETRY_NONE));
          $db->close();
      } else {
          $db->close();
          return($db->errorno());
      }
      return($status);
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
  function testCheckMapInsert($dummydb, $dummykey)
  {
      $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
      $db = new Aerospike($config);
      $key = array("ns"=>"test","set"=>"demo","key"=>"key_map");
      $opts =array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE,
          Aerospike::OPT_WRITE_TIMEOUT => 1500, 
          Aerospike::OPT_POLICY_EXISTS => Aerospike::POLICY_EXISTS_CREATE);
      $map1= array("k1"=>10,"k2"=>5,"k3"=>6,"k4"=>7,"k5"=>8);
      if ($db->isConnected()) {
          $status = $db->put($key,array("bin1"=>$map1, 10, $opts));
      } else {
          $db->close();
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          $db->close();
          return($status);
      }
      if ($db->isConnected()) {
          $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                =>Aerospike::POLICY_RETRY_NONE));
          $db->close();
      } else {
          $db->close();
          return($db->errorno());
      }
      return($status);
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
  function testCheckListMapCombineInsert($dummydb, $dummykey)
  {
      $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
      $db = new Aerospike($config);
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
      if ($db->isConnected()) {
          $status = $db->put($key,$rec, NULL, 
               array(Aerospike::OPT_WRITE_TIMEOUT => 2000));
      } else {
          $db->close();
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          $db->close();
          return($status);
      }
      if ($db->isConnected()) {
          $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
              =>Aerospike::POLICY_RETRY_NONE));
          $db->close();
      } else {
          $db->close();
          return($db->errorno());
      }
      return($status);
  }

/**
  * @test
  * Basic PUT Nested List  
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
  function testNestedList($dummydb, $dummykey) {
      $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
          "port"=>AEROSPIKE_CONFIG_PORT)));
      $db = new Aerospike($config);
      $key = $db->initKey("test", "demo", "list_max3");
      $list2 = array("aa");
      $list3 = array($list2,1,"kk");
      $list4 = array(46,$list3);
      if ($db->isConnected()) {
          $status = $db->put($key,array("bin1"=>$list4));
      } else {
          $db->close();
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          $db->close();
          return($status);
      }
      if ($db->isConnected()) {
          $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
              =>Aerospike::POLICY_RETRY_NONE));
          $db->close();
      } else {
          $db->close();
          return($db->errorno());
      }
      return($status);
  }

/**
  * @test
  * Basic PUT Insert Int List and string List combine 
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
  function testIntlistStringlist($dummydb, $dummykey) {
      $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
          "port"=>AEROSPIKE_CONFIG_PORT)));
      $db = new Aerospike($config);
      $key = $db->initKey("test", "demo", "st");
      $list1 = array("aa","vv","hh");
      $list2 = array(100,566,52,$list1);
      if ($db->isConnected()) {
          $status = $db->put($key,array("bin1"=>$list2));
      } else {
          $db->close();
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          $db->close();
          return($status);
      }
      if ($db->isConnected()) {
          $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
              =>Aerospike::POLICY_RETRY_NONE));
          $db->close();
      } else {
          $db->close();
          return($db->errorno());
      }
      return($status);
  }

/**
  * @test
  * Insert List with wrong IP
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
  function testListWithWrongIP($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>"X.X.X.X",
         "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     $key = $db->initKey("test", "demo", "list_list_strval");
     $list1 = array("p","q","r","s","d","op",12,56,89,55);
     $list2 = array(105,"r","s","t","p",$list1);
     if ($db->isConnected()) {
         $status = $db->put($key,array("bin1"=>$list2));
     } else {
         $db->close();
         return($db->errorno());
     }
     if($status != Aerospike::OK) {
         $db->close();
         return($status);
     }
     if ($db->isConnected()) {
         $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
             =>Aerospike::POLICY_RETRY_NONE));
         $db->close();
     } else {
         $db->close();
         return($db->errorno());
     }
     return($status);
 }

/**
  * @test
  * Basic PUT Insert Map with int value 
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
 function testMapWithIntval($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     $key = $db->initKey("test", "demo", "reival");
     $map1 = array("k10"=>10);
     $map2 = array(1,"k6"=>"abc",$map1);
     if ($db->isConnected()) {
         $status = $db->put($key,array("bin1"=>$map2));
     } else {
         $db->close();
         return($db->errorno());
     }
     if($status != Aerospike::OK) {
         $db->close();
         return($status);
     }
     if ($db->isConnected()) {
         $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
             =>Aerospike::POLICY_RETRY_NONE));
         $db->close();
     } else {
         $db->close();
         return($db->errorno());
     }
     return($status);
 }

/**
  * @test
  * Basic PUT Insert List int and string 
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
 function testIntStringList($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     $key = $db->initKey("test", "demo", "list6_key");
     $list1 = array("aa",2);
     $list2 = array(3,"k1","k4"=>$list1);
     if ($db->isConnected()) {
         $status = $db->put($key,array("bin1"=>$list2));
     } else {
         $db->close();
         return($db->errorno());
     }
     if($status != Aerospike::OK) {
         $db->close();
         return($status);
     }
     if ($db->isConnected()) {
         $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
             =>Aerospike::POLICY_RETRY_NONE));
         $db->close();
     } else {
         $db->close();
         return($db->errorno());
     }
     return($status);
 }

/**
  * @test
  * Basic PUT AssocArray 
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
 function testPutAssocArray($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     $key = $db->initKey("test", "demo", "key_val_int");
     $map2 = array("k10"=>89,85);
     if ($db->isConnected()) {
         $status = $db->put($key,array("bin1"=>$map2));
     } else {
         $db->close();
         return($db->errorno());
     }
     if($status != Aerospike::OK) {
         $db->close();
         return($status);
     }
     if ($db->isConnected()) {
         $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
             =>Aerospike::POLICY_RETRY_NONE));
         $db->close();
     } else {
         $db->close();
         return($db->errorno());
     }
     return($status);
 }

/**
  * @test
  * Basic PUT with Different Config Same Alias 
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
  * */        
 function testPutwithDifferentConfigSamealias(){
    
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
         "port"=>AEROSPIKE_CONFIG_PORT)));
     $config1 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME2,
         "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config,'alias1');
     $db1 = new Aerospike($config1,'alias1');
     $key = $db->initKey("test", "demo", "SameAlias");
     if ($db->isConnected()) {
         $status = $db->put($key,array("bin1"=>"Hello"));
     } else {
         $db->close();
         return($db->errorno());
     }
     if ($db1->isConnected()) {
         $status = $db1->put($key,array("bin1"=>"Hello"));
     } else {
         $db->close();
         return($db1->errorno());
     }
     if ($db->isConnected()) {
         $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
         =>Aerospike::POLICY_RETRY_NONE));
     } else {
         $db->close();
         return($db->errorno());
     }
     if ($db1->isConnected()) {
         $status = $db1->remove($key, array(Aerospike::OPT_POLICY_RETRY
             =>Aerospike::POLICY_RETRY_NONE));
         $db->close();
     } else {
         $db->close();
         return($db1->errorno());
     }
     return($status);
 }

/**
  * @test
  * PUT with Same config same alias.  
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
 function testPutwithDifferentConfigDifferentAlias(){
    
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
         "port"=>AEROSPIKE_CONFIG_PORT)));
     $config1 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME2,
         "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config,'alias1');
     $db1 = new Aerospike($config1,'alias2');
     $key = $db->initKey("test", "demo", "DifferentAlias");
     if ($db->isConnected()) {
         $status = $db->put($key,array("bin1"=>"Hello"));
     } else {
         $db->close();
         return($db->errorno());
     }
     if ($db1->isConnected()) {
         $status = $db1->put($key,array("bin1"=>"Hello"));
     } else {
         $db1->close();
         return($db1->errorno());
     }
     if ($db->isConnected()) {
         $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
             =>Aerospike::POLICY_RETRY_NONE));
         $db->close();
     } else {
         $db->close();
         return($db->errorno());
     }
     if ($db1->isConnected()) {
         $status = $db1->remove($key, array(Aerospike::OPT_POLICY_RETRY
             =>Aerospike::POLICY_RETRY_NONE));
         $db1->close();
     } else {
         $db1->close();
         return($db1->errorno());
     }
     return($status);
 }     

/**
  * @test
  * PUT List of objects and serializer option is SERIALIZER_NONE.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutListOfObjectsSerializerNone)
  *
  * @test_plans{1.1}
  */
 function testPutListOfObjectsSerializerNone($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_of_objects_with_none_serializer");
     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $list1 = array(12, $obj1, $obj3);
     $list2 = array($list1, $obj2);
     $list3 = array("k1", $obj4, $list2);
     $status = $db->put($key, array("bin1"=>$list3), NULL, array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_NONE));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test
  * PUT Nested List of objects with PHP Serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutNestedListOfObjectsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutNestedListOfObjectsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_of_objects_with_PHP_serializer");
     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $list1 = array(12, $obj1, $obj3);
     $list2 = array($list1, $obj2, " ");
     $list3 = array("k1", $obj4, $list2);
     $status = $db->put($key, array("bin1"=>$list3), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT Nested List of objects with UDF Serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutNestedListOfObjectsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutNestedListOfObjectsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_of_objects_with_UDF_serializer");
     #Set Serializer
     Aerospike::setSerializer(function ($val) {
         if (is_bool ($val)) {
             return "b||". serialize($val);
         }
         if (is_object ($val)) {
             return "o||". serialize($val);
         }
         if (is_double ($val)) {
             return "d||". serialize($val);
         }
         return "r||". $val;
     });

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $list1 = array(12, $obj1, $obj3);
     $list2 = array($list1, $obj2, " ");
     $list3 = array("k1", $obj4, $list2);
     $status = $db->put($key, array("bin1"=>$list3), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT Map of objects with Serializer option is SERIALIZER_NONE.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutMapOfObjectsSerializerNone)
  *
  * @test_plans{1.1}
  */
 function testPutMapOfObjectsSerializerNone($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_of_objects_with_none_serializer");
     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $map1 = array(12=>$obj1, $obj3);
     $map2 = array($map1, "k45"=>$obj2);
     $map3 = array("k1"=>12, $obj4, "k4"=>$map2);
     $status = $db->put($key, array("bin1"=>$map3), NULL, array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_NONE));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT Nested map of objects with PHP Serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutNestedMapOfObjectsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutNestedMapOfObjectsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_of_objects_with_PHP_serializer");

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $map1 = array(12=>$obj1, $obj3);
     $map2 = array($map1, $obj2, " ");
     $map3 = array("k1", $obj4, 56=>$map2);
     $status = $db->put($key, array("bin1"=>$map3), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT Nested map of objects with UDF Serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutNestedMapOfObjectsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutNestedMapOfObjectsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {   
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_of_objects_with_UDF_serializer");

     #Set Serializer
     Aerospike::setSerializer(function ($val) {
         if (is_bool ($val)) {
             return "b||". serialize($val);
         }
         if (is_object ($val)) {
             return "o||". serialize($val);
         }
         if (is_double ($val)) {
             return "d||". serialize($val);
         }
         return "r||". $val;
     });

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $map1 = array(12=>$obj1, $obj3);
     $map2 = array($map1, $obj2, " ");
     $map3 = array("k1", $obj4, 56=>$map2);
     $status = $db->put($key, array("bin1"=>$map3), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT List containing Map of objects with PHP Serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutListMapObjectsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutListMapObjectsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_map_objects_with_PHP_serializer");

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();

     $map = array(12=>"k5", "k78"=>$obj1, $obj2);
     $list = array("k1", $obj3, $map);
     $status = $db->put($key, array("bin1"=>$list), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return ($status);
 }

/**
  * @test  
  * PUT List containing Map of objects with UDF Serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutListMapObjectsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutListMapObjectsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_map_objects_with_UDF_serializer");

     #Set Serializer
     Aerospike::setSerializer(function ($val) {
         if (is_bool ($val)) {
             return "b||". serialize($val);
         }
         if (is_object ($val)) {
             return "o||". serialize($val);
         }
         if (is_double ($val)) {
             return "d||". serialize($val);
         }
         return "r||". $val;
     });

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $map = array(12=>$obj1, $obj2, "aa"=>$obj3);
     $list = array("k1", $obj4, $map, 55);
     $status = $db->put($key, array("bin1"=>$list), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return ($status);
 }

/**
  * @test  
  * PUT Map containing List of objects with PHP Serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutMapListObjectsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutMapListObjectsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_map_objects_with_PHP_serializer");

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $list = array(12, $obj2, 56, " ", $obj4);
     $map = array("k1"=>$obj3, $obj1, 56=>$list);
     $status = $db->put($key, array("bin1"=>$map), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return ($status);
 }
 
/**
  * @test  
  * PUT Map containing List of objects with UDF Serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutMapListObjectsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutMapListObjectsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {  
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_list_objects_with_UDF_serializer");

     #Set Serializer
     Aerospike::setSerializer(function ($val) {
         if (is_bool ($val)) {
             return "b||". serialize($val);
         }
         if (is_object ($val)) {
             return "o||". serialize($val);
         }
         if (is_double ($val)) {
             return "d||". serialize($val);
         }
         return "r||". $val;
     });

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $list = array(12, $obj2, 56, " ", $obj4);
     $map = array("k1"=>$obj3, $obj1, 56=>$list);
     $status = $db->put($key, array("bin1"=>$map), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return ($status);
 }

/**
  * @test  
  * PUT Map of objects, Object as a key.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutMapOfObjectsKeyIsObject)
  *
  * @test_plans{1.1}
  */
 /*function testPutMapOfObjectsKeyIsObject($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_of_objects_key_is_object");

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();

     $map = array("k1", $obj1=>$obj2, "k2");
     $status = $db->put($key, array("bin1"=>$map), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }*/

/**
  * @test  
  * PUT List of bools and seializer option is SERIALIZER_NONE.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutListOfBoolsSerializerNone)
  *
  * @test_plans{1.1}
  */
 #Testcases for bool
 function testPutListOfBoolsSerializerNone($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_of_bools_with_none_serializer");

     $list1 = array(12, TRUE);
     $list2 = array($list1, FALSE, 89);
     $list3 = array("k1", TRUE, $list2);
     $status = $db->put($key, array("bin1"=>$list3), NULL, array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_NONE));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT Nested List of bools with PHP serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutNestedListOfBoolsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutNestedListOfBoolsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_of_bools_with_PHP_serializer");

     $list1 = array(12, TRUE);
     $list2 = array($list1, " ");
     $list3 = array("k1", 56, $list2, FALSE);
     $status = $db->put($key, array("bin1"=>$list3), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT Nested List of bools with UDF serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutNestedListOfBoolsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutNestedListOfBoolsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_of_bools_with_UDF_serializer");

     #Set Serializer
     Aerospike::setSerializer(function ($val) {
         if (is_bool ($val)) {
             return "b||". serialize($val);
         }
         if (is_object ($val)) {
             return "o||". serialize($val);
         }
         if (is_double ($val)) {
             return "d||". serialize($val);
         }
         return "r||". $val;
     });

     $list1 = array(12, TRUE);
     $list2 = array($list1, " ");
     $list3 = array("k1", 56, $list2, FALSE);
     $status = $db->put($key, array("bin1"=>$list3), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT Map of bools and serializer option is SERIALIZER_NONE.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutMapOfBoolsSerializerNone)
  *
  * @test_plans{1.1}
  */
 function testPutMapOfBoolsSerializerNone($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_of_bools_with_none_serializer");

     $map1 = array(12=>TRUE, "k2");
     $map2 = array($map1, "aa"=>FALSE);
     $map3 = array("k1"=>12, 89, "k4"=>$map2);
     $status = $db->put($key, array("bin1"=>$map3), NULL, array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_NONE));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT Nested Map of bools with PHP serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutNestedMapOfBoolsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutNestedMapOfBoolsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_of_bools_with_PHP_serializer");

     $map1 = array(12=>TRUE, FALSE);
     $map2 = array($map1, "K5"=>" ");
     $map3 = array("k1", TRUE, 56=>$map2);
     $status = $db->put($key, array("bin1"=>$map3), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT Nested Map of bools with UDF serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutNestedMapOfBoolsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutNestedMapOfBoolsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_of_bools_with_UDF_serializer");

     #Set Serializer
     Aerospike::setSerializer(function ($val) {
         if (is_bool ($val)) {
             return "b||". serialize($val);
         }
         if (is_object ($val)) {
             return "o||". serialize($val);
         }
         if (is_double ($val)) {
             return "d||". serialize($val);
         }
         return "r||". $val;
     });

     $map1 = array(12=>TRUE, FALSE);
     $map2 = array($map1, "K5"=>" ");
     $map3 = array("k1", TRUE, 56=>$map2);
     $status = $db->put($key, array("bin1"=>$map3), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT List containing Map of bools with PHP serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutListMapBoolsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutListMapBoolsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {  
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_map_bools_with_PHP_serializer");

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $map = array(12=>$obj1, FALSE, $obj2, "aa"=>$obj3);
     $list = array("k1", $obj4, $map, 55=>TRUE);
     $status = $db->put($key, array("bin1"=>$list), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return ($status);
 }

/**
  * @test  
  * PUT List containing Map of bools with UDF serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutListMapBoolsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutListMapBoolsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_map_bools_with_UDF_serializer");

     #Set Serializer
     Aerospike::setSerializer(function ($val) {
         if (is_bool ($val)) {
             return "b||". serialize($val);
         }
         if (is_object ($val)) {
             return "o||". serialize($val);
         }
         if (is_double ($val)) {
             return "d||". serialize($val);
         }
         return "r||". $val;
     });

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $map = array(12=>$obj1, FALSE, $obj2, "aa"=>$obj3);
     $list = array("k1", $obj4, $map, 55=>TRUE);
     $status = $db->put($key, array("bin1"=>$list), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return ($status);
 }

/**
  * @test  
  * PUT Map containig List of bools with PHP serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutMapListBoolsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutMapListBoolsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_list_bools_with_PHP_serializer");

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $list = array(12, $obj2, 56, " ", $obj4, FALSE);
     $map = array("k1"=>$obj3, "k8"=>TRUE, 56=>$list, $obj1);
     $status = $db->put($key, array("bin1"=>$list), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return ($status);
 }
 
/**
  * @test  
  * PUT Map containig List of bools with UDF serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutMapListBoolsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutMapListBoolsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_list_bools_with_UDF_serializer");

     #Set Serializer
     Aerospike::setSerializer(function ($val) {
         if (is_bool ($val)) {
             return "b||". serialize($val);
         }
         if (is_object ($val)) {
             return "o||". serialize($val);
         }
         if (is_double ($val)) {
             return "d||". serialize($val);
         }
         return "r||". $val;
     });

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $list = array(12, $obj2, 56, " ", $obj4, FALSE);
     $map = array("k1"=>$obj3, "k8"=>TRUE, 56=>$list, $obj1);
     $status = $db->put($key, array("bin1"=>$map), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return ($status);
 }

/**
  * @test  
  * PUT Map of bools, Bool as a key.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutMapOfBoolsKeyIsBool)
  *
  * @test_plans{1.1}
  */
 /*function testPutMapOfBoolsKeyIsBool($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_of_bools_key_is_bool");

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();

     $map = array("k1", TRUE=>$obj2, $obj1);
     $status = $db->put($key, array("bin1"=>$map), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return ($status);
 }*/

/**
  * @test  
  * PUT List of floats and serializer option is SERIALIZER_NONE.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutListOfFloatsSerializerNone)
  *
  * @test_plans{1.1}
  */
 #Testcases for float
 function testPutListOfFloatsSerializerNone($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_of_floats_with_none_serializer");

     $list1 = array(12.23, TRUE);
     $list2 = array($list1, 56, 89.2);
     $list3 = array("k1", $list2);
     $status = $db->put($key, array("bin1"=>$list3), NULL, array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_NONE));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT Nested List of floats with PHP serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutNestedListOfFloatsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutNestedListOfFloatsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_of_floats_with_PHP_serializer");

     $list1 = array(12, 11.2);
     $list2 = array($list1, " ");
     $list3 = array("k1", 56, $list2, 56.896);
     $status = $db->put($key, array("bin1"=>$list3), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT Nested List of floats with UDF serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutNestedListOfFloatsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutNestedListOfFloatsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_of_floats_with_UDF_serializer");

     #Set Serializer
     Aerospike::setSerializer(function ($val) {
         if (is_bool ($val)) {
             return "b||". serialize($val);
         }
         if (is_object ($val)) {
             return "o||". serialize($val);
         }
         if (is_double ($val)) {
             return "d||". serialize($val);
         }
         return "r||". $val;
     });

     $list1 = array(12, 11.2);
     $list2 = array($list1, " ");
     $list3 = array("k1", 56, $list2, 56.896);
     $status = $db->put($key, array("bin1"=>$list3), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         $db->close(); 
         return($status);
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT Map of floats and serializer option is SERIALIZER_NONE.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutMapOfFloatsSerializerNone)
  *
  * @test_plans{1.1}
  */
 function testPutMapOfFloatsSerializerNone($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_of_floats_with_none_serializer");

     $map1 = array(12=>89.4, "k2");
     $map2 = array($map1, "k8"=>17.1);
     $map3 = array("k1"=>12, 89, "k4"=>$map2);
     $status = $db->put($key, array("bin1"=>$map3), NULL, array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_NONE));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT Nested Map of floats with PHP serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutNestedMapOfFloatsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutNestedMapOfFloatsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_of_floats_with_PHP_serializer");

     $map1 = array(12=>89.4, 55.44);
     $map2 = array($map1, "K5"=>" ");
     $map3 = array("k1", TRUE, 56=>$map2);
     $status = $db->put($key, array("bin1"=>$map3), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT Nested Map of floats with UDF serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutNestedMapOfFloatsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutNestedMapOfFloatsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_of_floats_with_UDF_serializer");

     #Set Serializer
     Aerospike::setSerializer(function ($val) {
         if (is_bool ($val)) {
             return "b||". serialize($val);
         }
         if (is_object ($val)) {
             return "o||". serialize($val);
         }
         if (is_double ($val)) {
             return "d||". serialize($val);
         }
         return "r||". $val;
     });

     $map1 = array(12=>TRUE, FALSE);
     $map2 = array($map1, "K5"=>" ");
     $map3 = array("k1", TRUE, 56=>$map2);
     $status = $db->put($key, array("bin1"=>$map3), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return($status);
 }

/**
  * @test  
  * PUT List containing Map of floats with PHP serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutListMapFloatsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutListMapFloatsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_map_floats_with_PHP_serializer");

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $map = array(12=>$obj1, $obj2, "aa"=>25.6, "k10"=>TRUE);
     $list = array("k1", $obj4, $map, 55=>89.56, FALSE, 45.78);
     $status = $db->put($key, array("bin1"=>$list), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
 }

/**
  * @test  
  * PUT List containing Map of floats with UDF serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutListMapFloatsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutListMapFloatsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_map_floats_with_UDF_serializer");

     #Set Serializer
     Aerospike::setSerializer(function ($val) {
         if (is_bool ($val)) {
             return "b||". serialize($val);
         }
         if (is_object ($val)) {
             return "o||". serialize($val);
         }
         if (is_double ($val)) {
             return "d||". serialize($val);
         }
         return "r||". $val;
     });

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $map = array(12=>$obj1, $obj2, "aa"=>25.6, "k10"=>TRUE);
     $list = array("k1", $obj4, $map, 55=>89.56, FALSE, 45.78);
     $status = $db->put($key, array("bin1"=>$list), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return ($status);
 }

/**
  * @test  
  * PUT Map containing List of floats with PHP serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutMapListFloatsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutMapListFloatsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_list_floats_with_PHP_serializer");

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $list = array(12, $obj2, 63.2, " ", $obj4, FALSE);
     $map = array("k1"=>$obj3, "k8"=>TRUE, 56=>$list, $obj1, "k22"=>56.75);
     $status = $db->put($key, array("bin1"=>$list), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return ($status);
 }
 
/**
  * @test  
  * PUT Map containing List of floats with UDF serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutMapListFloatsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testPutMapListFloatsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_list_floats_with_UDF_serializer");

     #Set Serializer
     Aerospike::setSerializer(function ($val) {
         if (is_bool ($val)) {
             return "b||". serialize($val);
         }
         if (is_object ($val)) {
             return "o||". serialize($val);
         }
         if (is_double ($val)) {
             return "d||". serialize($val);
         }
         return "r||". $val;
     });

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $list = array(12, $obj2, 63.2, " ", $obj4, FALSE);
     $map = array("k1"=>$obj3, "k8"=>TRUE, 56=>$list, $obj1, "k22"=>56.75);
     $status = $db->put($key, array("bin1"=>$map), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return ($status);
 }

/**
  * @test  
  * PUT Map of floats, Float as a key.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testPutMapOfFloatsKeyIsFloat)
  *
  * @test_plans{1.1}
  */
 /*function testPutMapOfFloatsKeyIsFloat($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         $db->close();
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_of_bools_key_is_bool");

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();

     $map = array("k1", 78.5=>56.35, $obj1=>1, $obj2);
     $status = $db->put($key, array("bin1"=>$map), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         $db->close();
         return($status);
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     $db->close();
     return ($status);
 }*/
}
?>
