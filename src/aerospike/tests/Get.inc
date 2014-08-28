<?php

require_once 'Common.inc';
/**
 *Basic Get tests
*/
class Employee
{
        public $desg = 'Manager';
}

class Get extends AerospikeTestCommon
{
    /*
     * TODO: Remove these constants and map with actual Aerospike class
     * constants.
     */
    static $status_ok = 2000;
    static $status_err = 2001;
    static $status_get_record_fail = 2002; 
    static $put_rec = array("Greet"=>"World_end");

    protected function setUp(&$db, &$key) {
	    $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("test", "demo", "Get_key");
        $db->put($key,array("Greet"=>"World_end"));
    }

    protected function tearDown(&$db, &$key) {
        $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
            =>Aerospike::POLICY_RETRY_NONE));
        $db->close();
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
            $status = $db->get($key,$bin_arr);
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($status);
        }
        $comp_res = array_diff_assoc_recursive(self::$put_rec, $bin_arr);
        if (!empty($comp_res)) {
            return Aerospike::ERR;
        }
        return ($status);
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
		$bin_arr = array("Greet");
        if ($db->isConnected()) {  
            $status = $db->get($key,$rec_arr,$bin_arr,
                array(Aerospike::OPT_READ_TIMEOUT=>2000));
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($status);
        }
        $comp_res = array_diff_assoc_recursive(self::$put_rec, $rec_arr);
        if (!empty($comp_res)) {
            return Aerospike::ERR;
        }
        return ($status);
    }
	/**
	 * @test
	 * Basic GET wrong Parameter
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
            $status = $db->get($key, $rec_array);
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($status);
        }
        $comp_res = array_diff_assoc_recursive(self::$put_rec, $rec_array);
        if (!empty($comp_res)) {
            return Aerospike::ERR;
        }
        return ($status);
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
            return($db->errorno());
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
          $status = $db->get($key1,$get_rec);
      } else {
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          return($status);
      }
      $comp_res = array_diff_assoc_recursive(array("bin1"=>array("speaking", "reading",
          "writing")), $get_rec);
      if (!empty($comp_res)) {
          return Aerospike::ERR;
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
          $status = $db->get($key1,$get_rec, NULL, 
              array(Aerospike::OPT_READ_TIMEOUT=>2000));
      } else {
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          return($status);
      }
      $comp_res = array_diff_assoc_recursive(array("bin1"=>$map1), $get_rec);
      if (!empty($comp_res)) {
          return Aerospike::ERR;
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
          $status = $db->get($key1,$get_rec);
      } else {
          return($db->errorno());
      }
      if($status != Aerospike::OK) {
          return($status);
      }
      $comp_res = array_diff_assoc_recursive($rec, $get_rec);
      if (!empty($comp_res)) {
          return Aerospike::ERR;
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

/**
  * @test
  * Get Nested List of objects with PHP Serializer.
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetNestedListOfObjectsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 #Testcases for object 
 function testGetNestedListOfObjectsWithPHPSerializer($dummydb, $dummykey) {
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

     $put_record = array("bin1"=>$list3);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
 
     if ($status != AEROSPIKE::OK) {
         return($status);
     }
     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }
     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return($status);
 }

/**
  * @test
  * GET Nested List of objects with UDF Serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetNestedListOfObjectsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetNestedListOfObjectsWithUDFSerializer($dummydb, $dummykey) {
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

     $put_record = array("bin1"=>$list3);
     $status = $db->put($key,$put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     #Set Deserializer
     Aerospike::setDeserializer(function ($val) {
         $prefix = substr($val, 0, 3);
         if ($prefix !== 'r||') {
             return unserialize(substr ($val, 3));
         }
         return unserialize(substr ($val, 3));
     });

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return($status);
 }

/**
  * @test
  * GET Nested map of objects with PHP Serializer.   
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetNestedMapOfObjectsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetNestedMapOfObjectsWithPHPSerializer($dummydb, $dummykey) {
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

     $put_record = array("bin1"=>$map3);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return($status);
 }

/**
  * @test
  * GET Nested map of objects with UDF Serializer.   
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetNestedMapOfObjectsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetNestedMapOfObjectsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $put_record = array("bin1"=>$map3);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     #Set Deserializer
     Aerospike::setDeserializer(function ($val) {
         $prefix = substr($val, 0, 3);
         if ($prefix !== 'r||') {
             return unserialize(substr ($val, 3));
         }
         return unserialize(substr ($val, 3));
     });
     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return($status);
 }

/**
  * @test
  * GET List containing Map of objects with PHP Serializer.    
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetListMapObjectsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetListMapObjectsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_map_objects_with_PHP_serializer");

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();

     $map = array(12=>"k5", "k78"=>$obj1, $obj2);
     $list = array("k1", $obj3, $map);
     $put_record = array("bin1"=>$list);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }

/**
  * @test
  * GET List containing Map of objects with UDF Serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetListMapObjectsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetListMapObjectsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $put_record = array("bin1"=>$list);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     #Set Deserializer
     Aerospike::setDeserializer(function ($val) {
         $prefix = substr($val, 0, 3);
         if ($prefix !== 'r||') {
             return unserialize(substr ($val, 3));
         }
         return unserialize(substr ($val, 3));
     });
     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }

/**
  * @test
  * GET Map containing List of objects with PHP Serializer.
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetMapListObjectsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetMapListObjectsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_map_objects_with_PHP_serializer");

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();

     $map = array(12=>"k5", "k78"=>$obj1);
     $list = array("k1", $obj3, $obj2, $map);

     $put_record = array("bin1"=>$list);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }
 
/**
  * @test
  * GET Map containing List of objects with UDF Serializer.
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetMapListObjectsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetMapListObjectsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $put_record = array("bin1"=>$map);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     #Set Deserializer
     Aerospike::setDeserializer(function ($val) {
         $prefix = substr($val, 0, 3);
         if ($prefix !== 'r||') {
             return unserialize(substr ($val, 3));
         }
         return unserialize(substr ($val, 3));
     });
     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }

/**
  * @test
  * GET PUT Nested List of bools with PHP serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetNestedListOfBoolsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 #Testcases for bool
 function testGetNestedListOfBoolsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_of_bools_with_PHP_serializer");

     $list1 = array(12, TRUE);
     $list2 = array($list1, " ");
     $list3 = array("k1", 56, $list2, FALSE);

     $put_record = array("bin1"=>$list3);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return($status);
 }

/**
  * @test
  * GET Nested List of bools with UDF serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetNestedListOfBoolsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetNestedListOfBoolsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $put_record = array("bin1"=>$list3);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     #Set Deserializer
     Aerospike::setDeserializer(function ($val) {
         $prefix = substr($val, 0, 3);
         if ($prefix !== 'r||') {
             return unserialize(substr ($val, 3));
         }
         return unserialize(substr ($val, 3));
     });
     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return($status);
 }

/**
  * @test
  * GET Nested Map of bools with PHP serializer.   
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetNestedMapOfBoolsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetNestedMapOfBoolsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_of_bools_with_PHP_serializer");

     $map1 = array(12=>TRUE, FALSE);
     $map2 = array($map1, "K5"=>" ");
     $map3 = array("k1", TRUE, 56=>$map2);

     $put_record = array("bin1"=>$map3);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return($status);
 }

/**
  * @test
  * GET Nested Map of bools with UDF serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetNestedMapOfBoolsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetNestedMapOfBoolsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $put_record = array("bin1"=>$map3);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     #Set Deserializer
     Aerospike::setDeserializer(function ($val) {
         $prefix = substr($val, 0, 3);
         if ($prefix !== 'r||') {
             return unserialize(substr ($val, 3));
         }
         return unserialize(substr ($val, 3));
     });
     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return($status);
 }

/**
  * @test
  * GET List containing Map of bools with PHP serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetListMapBoolsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetListMapBoolsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $put_record = array("bin1"=>$list);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     #Set Deserializer
     Aerospike::setDeserializer(function ($val) {
         $prefix = substr($val, 0, 3);
         if ($prefix !== 'r||') {
             return unserialize(substr ($val, 3));
         }
         return unserialize(substr ($val, 3));
     });
     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }

/**
  * @test
  * GET List containing Map of bools with UDF serializer. 
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetListMapBoolsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetListMapBoolsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $put_record = array("bin1"=>$list);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     #Set Deserializer
     Aerospike::setDeserializer(function ($val) {
         $prefix = substr($val, 0, 3);
         if ($prefix !== 'r||') {
             return unserialize(substr ($val, 3));
         }
         return unserialize(substr ($val, 3));
     });
     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }

/**
  * @test
  * GET Map containig List of bools with PHP serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetMapListBoolsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetMapListBoolsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $put_record = array("bin1"=>$map);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }
 
/**
  * @test
  * GET Map containing List of bools with UDF Serializer.
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetMapListBoolsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetMapListBoolsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $put_record = array("bin1"=>$map);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     #Set Deserializer
     Aerospike::setDeserializer(function ($val) {
         $prefix = substr($val, 0, 3);
         if ($prefix !== 'r||') {
             return unserialize(substr ($val, 3));
         }
         return unserialize(substr ($val, 3));
     });
     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }

/**
  * @test
  * GET Nested List of floats with PHP serializer.   
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetNestedListOfFloatsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 #Testcases for float
 function testGetNestedListOfFloatsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "list_of_floats_with_PHP_serializer");

     $list1 = array(12, 11.2);
     $list2 = array($list1, " ");
     $list3 = array("k1", 56, $list2, 56.896);

     $put_record = array("bin1"=>$list3);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return($status);
 }

/**
  * @test
  * GET Nested List of floats with UDF serializer. 
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetNestedListOfFloatsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetNestedListOfFloatsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $put_record = array("bin1"=>$list3);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     #Set Deserializer
     Aerospike::setDeserializer(function ($val) {
         $prefix = substr($val, 0, 3);
         if ($prefix !== 'r||') {
             return unserialize(substr ($val, 3));
         }
         return unserialize(substr ($val, 3));
     });
     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return($status);
 }

/**
  * @test
  * GET Nested Map of floats with PHP serializer. 
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetNestedMapOfFloatsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetNestedMapOfFloatsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo", "map_of_floats_with_PHP_serializer");

     $map1 = array(12=>89.4, 55.44);
     $map2 = array($map1, "K5"=>" ");
     $map3 = array("k1", TRUE, 56=>$map2);

     $put_record = array("bin1"=>$map3);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return($status);
 }

/**
  * @test
  * GET Nested Map of floats with UDF serializer.
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetNestedMapOfFloatsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetNestedMapOfFloatsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $map1 = array(12=>89.4, 55.44);
     $map2 = array($map1, "K5"=>" ");
     $map3 = array("k1", TRUE, 56=>$map2);

     $put_record = array("bin1"=>$map3);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     #Set Deserializer
     Aerospike::setDeserializer(function ($val) {
         $prefix = substr($val, 0, 3);
         if ($prefix !== 'r||') {
             return unserialize(substr ($val, 3));
         }
         return unserialize(substr ($val, 3));
     });
     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return($status);
 }

/**
  * @test
  * GET List containing Map of floats with PHP serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetListMapFloatsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetListMapFloatsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $put_record = array("bin1"=>$list);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }

/**
  * @test
  * GET List containing Map of floats with UDF serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetListMapFloatsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetListMapFloatsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $put_record = array("bin1"=>$list);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     #Set Deserializer
     Aerospike::setDeserializer(function ($val) {
         $prefix = substr($val, 0, 3);
         if ($prefix !== 'r||') {
             return unserialize(substr ($val, 3));
         }
         return unserialize(substr ($val, 3));
     });
     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }

/**
  * @test
  * GET Map containing List of floats with PHP serializer.  
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetMapListFloatsWithPHPSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetMapListFloatsWithPHPSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $put_record = array("bin1"=>$list);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }
 
/**
  * @test
  * GET Map containing List of floats with UDF Serializer.
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetMapListFloatsWithUDFSerializer)
  *
  * @test_plans{1.1}
  */
 function testGetMapListFloatsWithUDFSerializer($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $put_record = array("bin1"=>$map);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     #Set Deserializer
     Aerospike::setDeserializer(function ($val) {
         $prefix = substr($val, 0, 3);
         if ($prefix !== 'r||') {
             return unserialize(substr ($val, 3));
         }
         return unserialize(substr ($val, 3));
     });
     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }

/**
  * @test
  * GET object which is serialized using SERIALIZER_PHP and deserialized 
  * with SERIALIZER_USER.
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetObjectPHPSerializedAndUDFDeserialized)
  *
  * @test_plans{1.1}
  */
 function testGetObjectPHPSerializedAndUDFDeserialized($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo",
         "list_of_objects_with_PHP_SERIALIZER_USER_deserializer");

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $list1 = array(12, $obj1, $obj3);
     $list2 = array($list1, $obj2, " ");
     $list3 = array("k1", $obj4, $list2);

     $put_record = array("bin1"=>$list3);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
 
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     #Set Deserializer
     Aerospike::setDeserializer(function ($val) {
         $prefix = substr($val, 0, 3);
         if ($prefix !== 'r||') {
             return unserialize(substr ($val, 3));
         }
         return unserialize(substr ($val, 3));
     });

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }

/**
  * @test
  * GET object which is serialized using SERIALIZER_USER and deserialized 
  * with SERIALIZER_PHP.
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetObjectUDFSerializedAndPHPDeserialized)
  *
  * @test_plans{1.1}
  */
 function testGetObjectUDFSerializedAndPHPDeserialized($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $list = array(12, $obj2, 63.2, " ", $obj4, FALSE);
     $map = array("k1"=>$obj3, "k8"=>TRUE, 56=>$list, $obj1, "k22"=>56.75);

     $put_record = array("bin1"=>$map);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }

/**
  * @test
  * GET bool which is serialized using SERIALIZER_PHP and deserialized 
  * with SERIALIZER_USER.
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetBoolPHPSerializedAndUDFDeserialized)
  *
  * @test_plans{1.1}
  */
 function testGetBoolPHPSerializedAndUDFDeserialized($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo",
         "list_of_bools_with_PHP_SERIALIZER_USER_deserializer");

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $list1 = array(12, $obj1, $obj3);
     $list2 = array($list1, $obj2, " ");
     $list3 = array("k1", $obj4, $list2);

     $put_record = array("bin1"=>$list3);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
 
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     #Set Deserializer
     Aerospike::setDeserializer(function ($val) {
         $prefix = substr($val, 0, 3);
         if ($prefix !== 'r||') {
             return unserialize(substr ($val, 3));
         }
         return unserialize(substr ($val, 3));
     });

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }

/**
  * @test
  * GET bool which is serialized using SERIALIZER_USER and deserialized 
  * with SERIALIZER_PHP.
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetBoolUDFSerializedAndPHPDeserialized)
  *
  * @test_plans{1.1}
  */
 function testGetBoolUDFSerializedAndPHPDeserialized($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $list = array(12, $obj2, 63.2, " ", $obj4, FALSE);
     $map = array("k1"=>$obj3, "k8"=>TRUE, 56=>$list, $obj1, "k22"=>56.75);

     $put_record = array("bin1"=>$map);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }

/**
  * @test
  * GET float which is serialized using SERIALIZER_PHP and deserialized 
  * with SERIALIZER_USER.
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetFloattPHPSerializedAndUDFDeserialized)
  *
  * @test_plans{1.1}
  */
 function testGetFloatPHPSerializedAndUDFDeserialized($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
         return ($db->errorno());
     }
     $key = $db->initKey("test", "demo",
         "list_of_floats_with_PHP_SERIALIZER_USER_deserializer");

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $list1 = array(12, $obj1, $obj3);
     $list2 = array($list1, $obj2, " ");
     $list3 = array("k1", $obj4, $list2);

     $put_record = array("bin1"=>$list3);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
 
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     #Set Deserializer
     Aerospike::setDeserializer(function ($val) {
         $prefix = substr($val, 0, 3);
         if ($prefix !== 'r||') {
             return unserialize(substr ($val, 3));
         }
         return unserialize(substr ($val, 3));
     });

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
     return ($status);
 }

/**
  * @test
  * GET float which is serialized using SERIALIZER_USER and deserialized 
  * with SERIALIZER_PHP.
  *
  * @pre
  * Connect using aerospike object to the specified node
  *
  * @post
  * newly initialized Aerospike objects
  *
  * @remark
  * Variants: OO (testGetFloatUDFSerializedAndPHPDeserialized)
  *
  * @test_plans{1.1}
  */
 function testGetFloatUDFSerializedAndPHPDeserialized($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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

     $put_record = array("bin1"=>$map);
     $status = $db->put($key, $put_record, NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }

     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res))
     {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
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
  * Variants: OO (testGetMapOfBoolsKeyIsBool)
  *
  * @test_plans{1.1}
  */
 function testGetMapOfBoolsKeyIsBool($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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
         return($status);
     }
     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }
     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res)) {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
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
  * Variants: OO (testGetMapOfFloatsKeyIsFloat)
  *
  * @test_plans{1.1}
  */
 function testGetMapOfFloatsKeyIsFloat($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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
         return($status);
     }
     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }
     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res)) {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
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
  * Variants: OO (testGetMapOfObjectsKeyIsObject)
  *
  * @test_plans{1.1}
  */
 /*function testGetMapOfObjectsKeyIsObject($dummydb, $dummykey) {
     $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
     $db = new Aerospike($config);
     if(!$db->isConnected())
     {
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
         return($status);
     }
     $status = $db->get($key, $get_record, array("bin1"));
     if ($status != AEROSPIKE::OK) {
         return($status);
     }
     $comp_res = array_diff_assoc_recursive($put_record, $get_record);
     if(!empty($comp_res)) {
         return self::$status_get_record_fail;
     }
     $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
         Aerospike::POLICY_RETRY_NONE));
 }*/
}  
?>
