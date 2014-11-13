<?php

require_once 'Common.inc';
/**
 *Basic Query tests
 */

class Query extends AerospikeTestCommon
{

    protected function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        if (!$db->isConnected()) {
            return $db->errorno();
        }
        $this->ensureIndex($db, 'test', 'demo', 'age', 'demo_age_idx', Aerospike::INDEX_TYPE_INTEGER);
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
        if (!$db->isConnected()) {
            $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
            $db = new Aerospike($config);
        }
        $key = $db->initKey("test", "demo", "Get_key");
        $db->remove($key, array(Aerospike::OPT_READ_TIMEOUT =>3000));
        for( $i=1; $i<4; $i++) {
            $key=$db->initKey("test", "demo", "Get_key".$i);
            $db->remove($key, array(Aerospike::OPT_READ_TIMEOUT =>3000));
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
        try {
            $db->query();
        } catch (ErrorException $e) {
            return Parameter_Exception;
        }
        return Undefined_Error;
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
        $where = $db->predicateBetween("age", 30, 39);
        $total = 0;
        $in_thirties = 0;
        $status = $db->query("t", "d", $where, function ($record) use (&$total, &$in_thirties) {
            if (array_key_exists("email", $record["bins"]) &&
                !is_null($record["bins"]["email"])) {
                    $total += (int) $record["bins"]["age"];
                    $in_thirties++;
                }
        }, array("email","age"));
        if ($status !== Aerospike::OK) {
            return $db->errorno();
        }
        return $status;
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
        $where = $db->predicateBetween("age", 30, 39);
        $total = 0;
        $in_thirties = 0;
        $status = $db->query("", "", $where, function ($record) use (&$total, &$in_thirties) {
            if (array_key_exists("email", $record["bins"]) &&
                !is_null($record["bins"]["email"])) {
                    $total += (int) $record["bins"]["age"];
                    $in_thirties++;
            }
        }, array("email","age"));
        if ($status !== Aerospike::OK) {
            return($db->errorno());
        }
        return $status;
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
        $where = $db->predicateBetween("age", 30, 39);
        $total = 0;
        $in_thirties = 0;
        $status = $db->query( "test", "demo", $where, function ($record) use (&$total, &$in_thirties) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("email", $record["bins"]) &&
                        !is_null($record["bins"]["email"])) {
                            $total += (int) $record["bins"]["age"];
                            $in_thirties++;
                        }
                } else {
                    return $db->errorno();
                }
        }, array("first_name", "last_name"));
        if ($status !== Aerospike::OK) {
            return($db->errorno());
        }
        return $status;
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
        $total = 0;
        $in_thirties = 0;
        $where = $db->predicateBetween("age", 30, 39);
            $status = $db->query( "test", "demo", $where, function ($record) use (&$total, &$in_thirties) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("wrong_bin", $record["bins"]) &&
                        (!is_null($record["bins"]["wrong_bin"]))) {
                            global $total, $in_thirties;
                            $total += (int) $record["bins"]["age"];
                            $in_thirties++;
                        }
                } else {
                    return (AEROSPIKE::ERR);
                }
        }, array("email","age"));
        if ($status !== Aerospike::OK) {
            return($db->errorno());
        }
        return $status;
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
        try {
            $where = $db->predicateBetween("age", 30, 39);
            $status = $db->query("test", "demo", $where, function ($record) {
                // we expect to never get here
                return Aerospike::ERR;
            }, "");
        } catch (ErrorException $e) {
            // expected result is an exception for the argument 5 type not being array
            return Parameter_Exception;
        }
        return Aerospike::ERR;
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
        try {
            $where = $db->predicateBetween("age", 30, 39);
            $status = $db->query("test", "demo", $where, NULL, array("email","age"));
        } catch (ErrorException $e) {
            return Parameter_Exception;
        }
        return Undefined_Error;
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
        $where = $db->predicateBetween("age", 30, 39);
        $total = 0;
        $in_thirties = 0;
        $status = $db->query( "test", "demo", array(""), function ($record) use (&$total, &$in_thirties) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("email", $record["bins"]) &&
                        !is_null($record["bins"]["email"])) {
                            $total += (int) $record["bins"]["age"];
                            $in_thirties++;
                        }
                } else {
                    return (AEROSPIKE::ERR);
                }
        }, array("email", "age"), array(Aerospike::OPT_READ_TIMEOUT=>""));
        if ($status !== Aerospike::OK) {
            return($db->errorno());
        }
        return $status;
    }
    /**
     * @test
     * Query - Executing a query with where predicate containing a
     * non-indexed bin.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAggregatePositive)
     *
     * @test_plans{1.1}
     */
    function testQueryNegativeSecondaryIndexNotFound($db)
    {
        $where = $db->predicateEquals("first_nameaa", "raunak");
        $total = 0;
        $in_thirties = 0;
        $status = $db->query("test", "demo", $where, function ($record) use (&$total, &$in_thirties) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    global $total, $in_thirties;
                    if (array_key_exists("email", $record["bins"]) &&
                        (!is_null($record["bins"]["email"]))) {
                            if ($record["bins"]["email"] == "ajit") {
                                return Aerospike::OK;
                            }
                            $total += (int) $record["bins"]["age"];
                            $in_thirties++;
                        }
                } else {
                    return(AEROSPIKE::ERR);
                }
        }, array("email", "age"), array(Aerospike::OPT_READ_TIMEOUT=>2000));
        if ($status !== Aerospike::OK) {
            return($db->errorno());
        }
        return $status;
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
        $where = $db->predicateBetween("age", 30, 39);
        $total = 0;
        $in_thirties = 0;
        $status = $db->query("test", "demo", $where, function ($record) use (&$total, &$in_thirties) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    global $total, $in_thirties;
                    if (array_key_exists("email", $record["bins"]) &&
                        (!is_null($record["bins"]["email"]))) {
                            if ($record["bins"]["email"] == "ajit") {
                                return Aerospike::OK;
                        }
                        $total += (int) $record["bins"]["age"];
                        $in_thirties++;
                    }
                } else {
                    return(AEROSPIKE::ERR);
                }
        }, array("email", "age"), array(Aerospike::OPT_READ_TIMEOUT=>2000));
        if ($status !== Aerospike::OK) {
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
        $total = 0;
        $in_thirties = 0;
        $where = $db->predicateBetween("age", 30, 39);
        $status = $db->query( "test", "demo", $where, function ($record) use (&$total, &$in_thirties) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("email", $record["bins"]) &&
                        !is_null($record["bins"]["email"])) {
                            if ($record["bins"]["email"] == "ajit") {
                            $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
                            $db = new Aerospike($config);
                            $key = $db->initKey("test", "demo", "testKey");
                            $put_status = $db->put($key, array("name"=>"ajit"));
                            $db->close();
                        }
                        $total += (int) $record["bins"]["age"];
                        $in_thirties++;
                    }
                } else {
                    return (AEROSPIKE::ERR);
                }
        }, array("email", "age"), array(Aerospike::OPT_READ_TIMEOUT=>3000));
        if($status != Aerospike::OK) {
            return($db->errorno());
        }
        $key = $db->initKey("test", "demo", "testKey");
        $db->get($key, $record);
        $db->remove($key, array(Aerospike::OPT_READ_TIMEOUT=>3000));
        $result = array_diff_assoc_recursive($record["bins"], array("name"=>"ajit"));
        if (!empty($result)) {
            return Aerospike::ERR;
        }
        return $status;
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
        $where = $db->predicateBetween("age", 30, 39);
        $total = 0;
        $in_thirties = 0;
        $status = $db->query("test", "demo", $where, function ($record) use (&$total, &$in_thirties) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("email", $record["bins"]) &&
                        !is_null($record["bins"]["email"])) {
                            if($record["bins"]["email"]=="ajit") {
                                return Aerospike::OK;
                            }
                            $total += (int) $record["bins"]["age"];
                            $in_thirties++;
                        }
                } else {
                    return (AEROSPIKE::ERR);
                }
        });
        if ($status !== Aerospike::OK) {
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
        $where = $db->predicateBetween("age", 30, 39);
        $total = 0;
        $in_thirties = 0;
        $status = $db->query("test", "demo", $where, function ($record) use (&$total, &$in_thirties) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("email", $record["bins"]) &&
                        !is_null($record["bins"]["email"])) {
                        if ($record["bins"]["email"] == "ajit") {
                            return Aerospike::OK;
                        }
                        $total += (int) $record["bins"]["age"];
                        $in_thirties++;
                    }
               } else {
                   return (AEROSPIKE::ERR);
               }
        }, array( "email", "age"));
        if ($status !== Aerospike::OK) {
            return($db->errorno());
        }
        return $status;
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
        $where = $db->predicateBetween("age", 30, 39);
        $total = 0;
        $in_thirties = 0;
        $status = $db->query( "test", "demo", $where, function ($record) use (&$total, &$in_thirties) {
        }, array("email", "age"), array(Aerospike::OPT_READ_TIMEOUT=>3000));
        if ($status !== Aerospike::OK) {
            return $db->errorno();
        }
        return $status;
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
        try {
            $status = $db->query( "test", "demo", function ($record) use (&$total, &$in_thirties) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("email", $record["bins"]) &&
                        !is_null($record["bins"]["email"])) {
                            $total += (int) $record["bins"]["age"];
                            $in_thirties++;
                        }
                } else {
                    return (AEROSPIKE::ERR);
                }
            },array( "email", "age"));
        } catch (ErrorException $e) {
            return Parameter_Exception;
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
        $where = $db->predicateEquals("age", "");
        try {
            $status = $db->query( "test", "demo", $where, function ($record) {
                // we do not expect to be here
                return AEROSPIKE::ERR;
            }, array( "email", "age"));
        } catch (ErrorException $e) {
            // we expect the query() to throw this exception
            return Parameter_Exception;
        }
        return AEROSPIKE::ERR;
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
        $total = 0;
        $in_thirties = 0;
        $status = $db->query( "test", "demo", array(""), function ($record) use (&$total, &$in_thirties) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("email", $record["bins"]) &&
                        !is_null($record["bins"]["email"])) {
                        $total += (int) $record["bins"]["age"];
                        $in_thirties++;
                    }
                } else {
                    return (AEROSPIKE::ERR);
                }
        },array( "email", "age"));
        if ($status !== Aerospike::OK) {
            return $db->errorno();
        }
        return $status;
    }
}
?>
