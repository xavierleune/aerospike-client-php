<?php

require_once 'Common.inc';
/**
 *Basic Put tests
*/

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
            return($db->errorno());
        }
        if ($status != Aerospike::OK) {
            return($status);
        }
        if ($db->isConnected()) {
            $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                            =>Aerospike::POLICY_RETRY_NONE));
        } else{
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
            $status = $db->put($key,array("bin1"=>"Hello World","bin2"=>123456));
        } else{
            return($db->errorno());
        }
        if ($status != Aerospike::OK) {
            return($status);
        }
        if ($db->isConnected()) {
            $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                            =>Aerospike::POLICY_RETRY_NONE));
        } else{
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
                return($db->errorno());
            }
        } catch (ErrorException $e) {
            return self::$status_ok;
        }
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
                return($db->errorno());
            }
        } catch (ErrorException $e) {
            return self::$status_ok;
        }
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
                return($db->errorno());
            }
        } catch (ErrorException $e) {
            return self::$status_ok;
        }
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
            return($db->errorno());
        }
        if ($status != Aerospike::OK) {
            return($status);
        }
        if ($db->isConnected()) {
            $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                =>Aerospike::POLICY_RETRY_NONE));
        } else{
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
            return($db->errorno());
        }
        if ($status != Aerospike::OK) {
            return($status);
        }
        if ($db->isConnected()) {
            $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                =>Aerospike::POLICY_RETRY_NONE));
        } else{
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
            return($db->errorno());
        }
        if ($status != Aerospike::OK) {
            return($status);
        }
        if ($db->isConnected()) {
            $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
              =>Aerospike::POLICY_RETRY_NONE));
        } else{
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
            return($db->errorno());
        }
        if ($status != Aerospike::OK) {
            return($status);
        }
        if ($db->isConnected()) {
            $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                =>Aerospike::POLICY_RETRY_NONE));
        } else{
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
                return($db->errorno());
            }
        } catch (ErrorException $e) {
            return self::$status_ok;
        }
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
                 return($db->errorno());
             }
             if($status != Aerospike::OK) {
                 return($status);
             }
             if ($db->isConnected()) {
                  $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                     =>Aerospike::POLICY_RETRY_NONE));
             } else {
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
          $status = $db->put($key,array("bin1"=>"Hello World"));
      } else {
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          return($status);
      }
      if ($db->isConnected()) {
          $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
          =>Aerospike::POLICY_RETRY_NONE));
      } else {
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
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          return($status);
      }
      if ($db->isConnected()) {
          $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
          =>Aerospike::POLICY_RETRY_NONE));
      } else {
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
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          return($status);
      }
      if ($db->isConnected()) {
          $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
          =>Aerospike::POLICY_RETRY_NONE));
      } else {
          return($db->errorno());
      }
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
          $status = $db->put($key,array("bin1"=>array("speaking", "reading", "writing")));
      } else {
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          return($status);
      }
      if ($db->isConnected()) {
          $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
          =>Aerospike::POLICY_RETRY_NONE));
      } else {
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
      $map1= array("k1"=>10,"k2"=>5,"k3"=>6,"k4"=>7,"k5"=>8);
      if ($db->isConnected()) {
          $status = $db->put($key,array("bin1"=>$map1));
      } else {
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          return($status);
      }
      if ($db->isConnected()) {
          $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
          =>Aerospike::POLICY_RETRY_NONE));
      } else {
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
          $status = $db->put($key,$rec);
      } else {
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          return($status);
      }
      if ($db->isConnected()) {
          $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
          =>Aerospike::POLICY_RETRY_NONE));
      } else {
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
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          return($status);
      }
      if ($db->isConnected()) {
          $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
          =>Aerospike::POLICY_RETRY_NONE));
      } else {
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
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          return($status);
      }
      if ($db->isConnected()) {
          $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
          =>Aerospike::POLICY_RETRY_NONE));
      } else {
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
     $config = array("hosts"=>array(array("addr"=>"10.43.3.695",
         "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     $key = $db->initKey("test", "demo", "list_list_strval");
     $list1 = array("p","q","r","s","d","op",12,56,89,55);
     $list2 = array(105,"r","s","t","p",$list1);
     if ($db->isConnected()) {
         $status = $db->put($key,array("bin1"=>$list2));
     } else {
         return($db->errorno());
     }
     if($status != Aerospike::OK) {
         return($status);
     }
     if ($db->isConnected()) {
         $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
         =>Aerospike::POLICY_RETRY_NONE));
     } else {
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
         return($db->errorno());
     }
     if($status != Aerospike::OK) {
         return($status);
     }
     if ($db->isConnected()) {
         $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
         =>Aerospike::POLICY_RETRY_NONE));
     } else {
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
         return($db->errorno());
     }
     if($status != Aerospike::OK) {
         return($status);
     }
     if ($db->isConnected()) {
         $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
         =>Aerospike::POLICY_RETRY_NONE));
     } else {
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
         return($db->errorno());
     }
     if($status != Aerospike::OK) {
         return($status);
     }
     if ($db->isConnected()) {
         $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
         =>Aerospike::POLICY_RETRY_NONE));
     } else {
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
     $config1 = array("hosts"=>array(array("addr"=>"172.20.25.179", "port"=>3000)));
     $db = new Aerospike($config,'alias1');
     $db1 = new Aerospike($config1,'alias1');
     $key = $db->initKey("test", "demo", "SameAlias");
     if ($db->isConnected()) {
         $status = $db->put($key,array("bin1"=>"Hello"));
     } else {
         return($db->errorno());
     }
     if ($db1->isConnected()) {
         $status = $db1->put($key,array("bin1"=>"Hello"));
     } else {
         return($db1->errorno());
     }
     if ($db->isConnected()) {
         $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
         =>Aerospike::POLICY_RETRY_NONE));
     } else {
         return($db->errorno());
     }
     if ($db1->isConnected()) {
         $status = $db1->remove($key, array(Aerospike::OPT_POLICY_RETRY
         =>Aerospike::POLICY_RETRY_NONE));
     } else {
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
     $config1 = array("hosts"=>array(array("addr"=>"172.20.25.179", "port"=>3000)));
     $db = new Aerospike($config,'alias1');
     $db1 = new Aerospike($config1,'alias2');
     $key = $db->initKey("test", "demo", "DifferentAlias");
     if ($db->isConnected()) {
         $status = $db->put($key,array("bin1"=>"Hello"));
     } else {
         return($db->errorno());
     }
     if ($db1->isConnected()) {
         $status = $db1->put($key,array("bin1"=>"Hello"));
     } else {
         return($db1->errorno());
     }
     if ($db->isConnected()) {
         $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
         =>Aerospike::POLICY_RETRY_NONE));
     } else {
         return($db->errorno());
     }
     if ($db1->isConnected()) {
         $status = $db1->remove($key, array(Aerospike::OPT_POLICY_RETRY
         =>Aerospike::POLICY_RETRY_NONE));
     } else {
         return($db1->errorno());
     }
     return($status);
 }     
}
?>
