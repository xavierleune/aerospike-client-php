<?php

require_once 'Common.inc';
/**
 *Basic Query tests
 */

class Query extends AerospikeTestCommon
{
    /*
     * TODO: Remove these constants and map with actual Aerospike class
     * constants.
     */
    static $status_ok = 2000;
    static $status_err = 2001;
    static $status_get_record_fail = 2002;

    protected function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("test", "demo", "Get_key");
        $db->put($key,array("email"=>"raunak", "age"=>29));
        $key = $db->initKey("test", "demo", "Get_key1");
        $db->put($key,array("email"=>"pavan", "age"=>27));
        $key = $db->initKey("test", "demo", "Get_key2");
        $db->put($key,array("email"=>"bhushan", "age"=>22));
        $key = $db->initKey("test", "demo", "Get_key3");
        $db->put($key,array("email"=>"ajit", "age"=>32));
    }

    protected function tearDown(&$db, &$key) {
        $key = $db->initKey("test", "demo", "Get_key");
        $db->remove($key, array(Aerospike::OPT_READ_TIMEOUT
            =>3000));
        for( $i=1; $i<4; $i++) {
            $key=$db->initKey("test", "demo", "Get_key".$i);
            $db->remove($key, array(Aerospike::OPT_READ_TIMEOUT
                =>3000));
        }
        $db->close();
    }
    /**
     * @test
     * Basic Query without parameter
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
    function testQueryNoParameter($db, $key)
    {
        if ($db->isConnected()) {
            try {
                $db->query();
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
     * Query with incorrect ns and set
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
    function testQueryWithIncorrectNsSet($db)
    {
        if ($db->isConnected()) {
            $where = $db->predicateBetween("age", 30, 39);
            $status = $db->query("t", "d", $where, function ($record) {
                if (array_key_exists('email', $record) && !is_null($record['email'])) {
                    $total += (int) $record['age'];
                    $in_thirties++;
                }
            }, array("email","age"));
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($db->errorno());
        }
        return ($status);
    }
    /**
     * @test
     * Query with empty ns and set
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
    function testQueryWithEmptyNsSet($db)
    {
        if ($db->isConnected()) {
            $where = $db->predicateBetween("age", 30, 39);
            $status = $db->query("", "", $where, function ($record) {
                if (array_key_exists('email', $record) && !is_null($record['email'])) {
                    $total += (int) $record['age'];
                    $in_thirties++;
                }
            }, array("email","age"));
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($db->errorno());
        }
        return ($status);
    }
    /**
     * @test
     * Query with incorrect name of bins
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
    function testQueryWithIncorrectNameOfBins($db)
    {
        if ($db->isConnected()) {
            $where = $db->predicateBetween("age", 30, 39);
            $status = $db->query( "test", "demo", $where, function ($record) {
                if (array_key_exists('email', $record) && !is_null($record['email'])) {
                    $total += (int) $record['age'];
                    $in_thirties++;
                }
            }, array("first_name", "last_name"));
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($db->errorno());
        }
        return ($status);
    }
    /**
     * @test
     * Query with wrong bin name in callback
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
    function testQueryWithWrongBinNameInCallback($db)
    {
        if ($db->isConnected()) {
    	    $total = 0;
	        $in_thirties = 0;
            $where = $db->predicateBetween("age", 30, 39);
            $status = $db->query( "test", "demo", $where, function ($record) {
                if (array_key_exists('wrong_bin', $record) &&
                    (!is_null($record['wrong_bin']))) {
                    global $total, $in_thirties;
                    $total += (int) $record['age'];
                    $in_thirties++;
                }
            }, array("email","age"));
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($db->errorno());
        }
        return ($status);
    }
    /**
     * @test
     * Query with empty bin value
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
    function testQueryWithEmptyBinValue($db)
    {
        if ($db->isConnected()) {
            $where = $db->predicateBetween("age", 30, 39);
	    try {
	            $status = $db->query( "test", "demo", $where, function ($record) {
        	        if (array_key_exists('email', $record) && !is_null($record['email'])) {
                	    $total += (int) $record['age'];
	                    $in_thirties++;
        	        }
	            }, "");
	    } catch (Exception $e) {
		return self::$status_ok;
	    }
	} else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($db->errorno());
        }
        return ($status);
    }
    /**
     * @test
     * Query with callback parameter missing
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
    function testQueryWithCallbackParameterMissing($db) {
        if ($db->isConnected()) {
            try {
                $where = $db->predicateBetween("age", 30, 39);
            	$status = $db->query("test", "demo", $where, NULL, array("email","age"));
            } catch (ErrorException $e) {
                return self::$status_ok;
            }
            return self::$status_err;
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($db->errorno());
        }
        return ($status);
    }
    /**
     * @test
     * Query with incorrect value for optionals
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
    function testQueryWithIncorrectValueForOptionals($db)
    {
        if ($db->isConnected()) {
            $where = $db->predicateBetween("age", 30, 39);
            $status = $db->query( "test", "demo", array(""), function ($record) {
                if (array_key_exists('email', $record) && !is_null($record['email'])) {
                    $total += (int) $record['age'];
                    $in_thirties++;
                }
            }, array("email", "age"), array(Aerospike::OPT_READ_TIMEOUT=>""));
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($db->errorno());
        }
        return ($status);
    }
    /**
     * @test
     * Query with correct arguments
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
    function testQueryWithCorrectArguments($db)
    {
        if ($db->isConnected()) {
            $where = $db->predicateBetween("age", 30, 39);
    	    $total = 0;
	        $in_thirties = 0;
            $status = $db->query("test", "demo", $where, function ($record) {
        		global $total, $in_thirties;
                if (array_key_exists('email', $record) && (!is_null($record['email']))) {
                    if ($record['email'] == "ajit") {
                        return Aerospike::OK;
                    }
                    $total += (int) $record['age'];
                    $in_thirties++;
                }
            }, array("email", "age"), array(Aerospike::OPT_READ_TIMEOUT=>2000));
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($db->errorno());
        }
        return $status;
    }
    /**
     * @test
     * Query with correct arguments and put in callback
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
    function testQueryWithPutInCallback($db)
    {
        if ($db->isConnected()) {
    	    $total = 0;
	        $in_thirties = 0;
            $where = $db->predicateBetween("age", 30, 39);
            $status = $db->query( "test", "demo", $where, function ($record) {
                global $total, $in_thirties;
                if (array_key_exists('email', $record) && !is_null($record['email'])) {
                    if ($record['email'] == "ajit") {
                        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
                        $db = new Aerospike($config);
                        $key = $db->initKey("test", "demo", "testKey");
                        $put_status = $db->put($key, array("name"=>"ajit"));
                        $db->close();
                    }
                    $total += (int) $record['age'];
                    $in_thirties++;
                }
            }, array("email", "age"), array(Aerospike::OPT_READ_TIMEOUT=>3000));
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($db->errorno());
        }
        $key = $db->initKey("test", "demo", "testKey");
        $db->get($key, $record);
        $db->remove($key, array(Aerospike::OPT_READ_TIMEOUT=>3000));
        $result = array_diff_assoc_recursive($record, array("name"=>"ajit"));
        if(!empty($result)) {
            return Aerospike::ERR;
        }
        return Aerospike::OK;
    }
    /**
     * @test
     * Query without optional arguments
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
    function testQueryWithOptionalArguments($db)
    {
        if ($db->isConnected()) {
            $where = $db->predicateBetween("age", 30, 39);
    	    $total = 0;
	        $in_thirties = 0;
            $status = $db->query("test", "demo", $where, function ($record) {
                global $total, $in_thirties;
                if (array_key_exists('email', $record) && !is_null($record['email'])) {
                    if($record['email']=="ajit") {
                        return Aerospike::OK;
                    }
                    $total += (int) $record['age'];
                    $in_thirties++;
                }
            });
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($db->errorno());
        }
        return ($status);
    }
    /**
     * @test
     * Query with one optional argument
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
    function testQueryWithOneOptionalArgument($db)
    {
        if ($db->isConnected()) {
            $where = $db->predicateBetween("age", 30, 39);
    	    $total = 0;
	        $in_thirties = 0;
            $status = $db->query("test", "demo", $where, function ($record) {
                global $total, $in_thirties;
                if (array_key_exists('email', $record) && !is_null($record['email'])) {
                    if ($record['email'] == "ajit") {
                        return Aerospike::OK;
                    }
                    $total += (int) $record['age'];
                    $in_thirties++;
                }
            }, array( "email", "age"));
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($db->errorno());
        }
        return ($status);
    }
    /**
     * @test
     * Query with empty callback function
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
    function testQueryWithEmptyCallbackFunction($db)
    {
        if ($db->isConnected()) {
            $where = $db->predicateBetween("age", 30, 39);
            $status = $db->query( "test", "demo", $where, function ($record) {
            }, array("email", "age"), array(Aerospike::OPT_READ_TIMEOUT=>3000));
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($db->errorno());
        }
        return ($status);
    }
    /**
     * @test
     * Basic Query with where missing
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
    function testQueryWithWhereParameterMissing($db)
    {
        if ($db->isConnected()) {
            try {
                $status = $db->query( "test", "demo", function ($record) {
                    if (array_key_exists('email', $record) && !is_null($record['email'])) {
                        $total += (int) $record['age'];
                        $in_thirties++;
                    }
                },array( "email", "age"));
            } catch (ErrorException $e) {
                return self::$status_ok;
            }
        } else {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * Query with where containing null value
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
    function testQueryWithWhereContainingNullValue($db)
    {
        if ($db->isConnected()) {
            $where = $db->predicateEquals("age", "");
            try {
                $status = $db->query( "test", "demo", $where, function ($record) {
                    if (array_key_exists('email', $record) && !is_null($record['email'])) {
                        $total += (int) $record['age'];
                        $in_thirties++;
                }
            }, array( "email", "age"));
            } catch (ErrorException $e) {
                return self::$status_ok;
            }
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($db->errorno());
        }
        return ($status);
    }

    /**
     * @test
     * Query with where containing null string
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
    function testQueryWithWhereContainingNullString($db)
    {
        if ($db->isConnected()) {
            $status = $db->query( "test", "demo", array(""), function ($record) {
                if (array_key_exists('email', $record) && !is_null($record['email'])) {
                    $total += (int) $record['age'];
                    $in_thirties++;
                }
            },array( "email", "age"));
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($db->errorno());
        }
        return ($status);
    }
}
?>
