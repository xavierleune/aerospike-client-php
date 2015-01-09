<?php

require_once 'Common.inc';
/**
 *Basic Query tests
 */

class Query extends AerospikeTestCommon
{

    protected function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        if (!$this->db->isConnected()) {
            return $this->db->errorno();
        }
        $key = $this->db->initKey("test", "demo", "Get_key");
        $this->db->put($key,array("email"=>"john", "age"=>29));
        $this->keys[] = $key;
        $key = $this->db->initKey("test", "demo", "Get_key1");
        $this->db->put($key,array("email"=>"smith", "age"=>27));
        $this->keys[] = $key;
        $key = $this->db->initKey("test", "demo", "Get_key2");
        $this->db->put($key,array("email"=>"adam", "age"=>22));
        $this->keys[] = $key;
        $key = $this->db->initKey("test", "demo", "Get_key3");
        $this->db->put($key,array("email"=>"ellie", "age"=>32));
        $this->keys[] = $key;
        $this->ensureIndex('test', 'demo', 'age', 'demo_age_idx', Aerospike::INDEX_TYPE_INTEGER);
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
    function testQueryNoParameter()
    {
        try {
            $this->db->query();
        } catch (ErrorException $e) {
            return Aerospike::ERR_PARAM;
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
    function testQueryWithIncorrectNsSet()
    {
        $where = $this->db->predicateBetween("age", 30, 39);
        $total = 0;
        $in_thirties = 0;
        $callback = function ($record) use (&$total, &$in_thirties) {
            if (array_key_exists("email", $record["bins"]) &&
                !is_null($record["bins"]["email"])) {
                    $total += (int) $record["bins"]["age"];
                    $in_thirties++;
                }
        };
        $status = $this->db->query("t", "d", $where, $callback,
            array("email","age"));
        if ($status != Aerospike::OK) {
            return $this->db->errorno();
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
    function testQueryWithEmptyNsSet()
    {
        $where = $this->db->predicateBetween("age", 30, 39);
        $total = 0;
        $in_thirties = 0;
        $callback = function ($record) use (&$total, &$in_thirties) {
            if (array_key_exists("email", $record["bins"]) &&
                !is_null($record["bins"]["email"])) {
                    $total += (int) $record["bins"]["age"];
                    $in_thirties++;
            }
        };
        $status = $this->db->query("", "", $where,
            $callback, array("email","age"));
        if ($status != Aerospike::OK) {
            return $this->db->errorno();
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
    function testQueryWithIncorrectNameOfBins()
    {
        $where = $this->db->predicateBetween("age", 30, 39);
        $total = 0;
        $in_thirties = 0;
        $callback = function ($record) use (&$total, &$in_thirties) {
            $key = $this->db->initKey("test", "demo", "testing");
            $this->db->put($key, array("bin1"=>10));
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("email", $record["bins"]) &&
                        !is_null($record["bins"]["email"])) {
                            $total += (int) $record["bins"]["age"];
                            $in_thirties++;
                        } else {
                            return Aerospike::ERR_CLIENT;
                       }
                } else {
                    return Aerospike::ERR_CLIENT;
                }
        };
        $status = $this->db->query( "test", "demo", $where,
            $callback, array("first_name", "last_name"));
        $key = $this->db->initKey("test", "demo", "testing");
        $this->db->get($key, $record);
        if(array_key_exists('bins',$record)) {
        if(10 == $record['bins']['bin1']) {
            $this->db->remove($key);
            return Aerospike::OK;
        }
        } else {
            return Aerospike::ERR_RECORD_NOT_FOUND;
        }
        return Aerospike::ERR_CLIENT;
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
    function testQueryWithWrongBinNameInCallback()
    {
        $total = 0;
        $in_thirties = 0;
        $db= $this->db;
        $where = $this->db->predicateBetween("age", 30, 39);
        $callback = function ($record) use (&$total, &$in_thirties, &$db) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("wrong_bin", $record["bins"]) &&
                        (!is_null($record["bins"]["wrong_bin"]))) {
                        $key = $db->initKey("test", "demo", "testing");
                        $db->put($key, array("bin1"=>10));
                        $total += (int) $record["bins"]["age"];
                        $in_thirties++;
                    }
                }
        };
        $status = $this->db->query( "test", "demo", $where,
            $callback, array("email","age"));
        $key = $this->db->initKey("test", "demo", "testing");
        $this->db->get($key, $record);
        if(array_key_exists('bins',$record)) {
        if(10 == $record['bins']['bin1']) {
            $this->db->remove($key);
            return Aerospike::OK;
        }
        } else {
            return Aerospike::ERR_RECORD_NOT_FOUND;
        }
        return Aerospike::ERR_CLIENT;
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
    function testQueryWithEmptyBinValue()
    {
        try {
            $where = $this->db->predicateBetween("age", 30, 39);
            $callback = function ($record) {
                // we expect to never get here
                return Aerospike::ERR_CLIENT;
            };
            $status = $this->db->query("test", "demo", $where,
                $callback, "");
        } catch (ErrorException $e) {
            // expected result is an exception for the argument 5 type not being array
            return Aerospike::ERR_PARAM;
        }
        return Aerospike::ERR_CLIENT;
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
    function testQueryWithCallbackParameterMissing() {
        try {
            $where = $this->db->predicateBetween("age", 30, 39);
            $status = $this->db->query("test", "demo", $where,
                NULL, array("email","age"));
        } catch (ErrorException $e) {
            return Aerospike::ERR_PARAM;
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
    function testQueryWithIncorrectValueForOptionals()
    {
        $total = 0;
        $in_thirties = 0;
        $callback = function ($record) use (&$total, &$in_thirties) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("email", $record["bins"]) &&
                        !is_null($record["bins"]["email"])) {
                            $total += (int) $record["bins"]["age"];
                            $in_thirties++;
                        }
                } else {
                    return (AEROSPIKE::ERR_PARAM);
                }
        };
        $status = $this->db->query( "test", "demo", array(), $callback,
            array("email", "age"), array(Aerospike::OPT_READ_TIMEOUT=>""));
        if ($status != Aerospike::OK) {
            return $this->db->errorno();
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
    function testQueryNegativeSecondaryIndexNotFound()
    {
        $where = $this->db->predicateEquals("first_nameaa", "raunak");
        $total = 0;
        $in_thirties = 0;
        $callback = function ($record) use (&$total, &$in_thirties) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("email", $record["bins"]) &&
                        (!is_null($record["bins"]["email"]))) {
                            if ($record["bins"]["email"] == "ajit") {
                                return Aerospike::OK;
                            }
                            $total += (int) $record["bins"]["age"];
                            $in_thirties++;
                        }
                } else {
                    return(AEROSPIKE::ERR_PARAM);
                }
        };
        $status = $this->db->query("test", "demo", $where, $callback,
            array("email", "age"), array(Aerospike::OPT_READ_TIMEOUT=>2000));
        if ($status != Aerospike::OK) {
            return $this->db->errorno();
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
    function testQueryWithCorrectArguments()
    {
        $where = $this->db->predicateBetween("age", 30, 39);
        $total = 0;
        $db= $this->db;
        $in_thirties = 0;
        $callback = function ($record) use (&$total, &$in_thirties, &$db) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                if (array_key_exists("email", $record["bins"]) &&
                    (!is_null($record["bins"]["email"]))) {
                    if ($record["bins"]["email"] == "ellie") {
                        $key = $db->initKey("test", "demo", "testing");
                        $db->put($key, array("bin1"=>10));
                        return Aerospike::OK;
                    }
                    $total += (int) $record["bins"]["age"];
                    $in_thirties++;
                }
            }
        };
        $status = $this->db->query("test", "demo", $where, $callback,
            array("email", "age"));
        $key = $this->db->initKey("test", "demo", "testing");
        $this->db->get($key, $record);
        if(array_key_exists('bins',$record)) {
        if(10 == $record['bins']['bin1']) {
            $this->db->remove($key);
            return Aerospike::OK;
        }
        }
        return Aerospike::ERR_CLIENT;
        /*if ($status != Aerospike::OK) {
            return $this->db->errorno();
        }
        return $status;*/
    }
    /**
     * @test
     * Query with correct arguments and predicateEqual
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
    function testQueryWithPredicateEqual()
    {
        $where = $this->db->predicateEquals("age", 22);
        $total = 0;
        $db = $this->db;
        $in_thirties = 0;
        $callback = function ($record) use (&$total, &$in_thirties, &$db) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                if (array_key_exists("email", $record["bins"]) &&
                    (!is_null($record["bins"]["email"]))) {
                    if ($record["bins"]["email"] == "adam") {
                        $key = $db->initKey("test", "demo", "testing");
                        $db->put($key, array("bin1"=>10));
                        return Aerospike::OK;
                    }
                    $total += (int) $record["bins"]["age"];
                    $in_thirties++;
                }
            }
        };
        $status = $this->db->query("test", "demo", $where, $callback,
            array("email", "age"));
        $key = $this->db->initKey("test", "demo", "testing");
        $this->db->get($key, $record);
        if(array_key_exists('bins',$record)) {
        if(10 == $record['bins']['bin1']) {
            $this->db->remove($key);
            return Aerospike::OK;
        }
        }
        return Aerospike::ERR_CLIENT;
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
    function testQueryWithPutInCallback()
    {
        $total = 0;
        $in_thirties = 0;
        $where = $this->db->predicateBetween("age", 30, 39);
        $db = $this->db;
        $callback = function ($record) use (&$total, &$in_thirties, $db) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("email", $record["bins"]) &&
                        !is_null($record["bins"]["email"])) {
                        if ($record["bins"]["email"] == "ellie") {
                        $key = $db->initKey("test", "demo", "testKey");
                        $db->put($key, array("name"=>"ellie"));
                    }
                    $total += (int) $record["bins"]["age"];
                    $in_thirties++;
                }
            }
        };
        $status = $this->db->query( "test", "demo", $where, $callback,
            array("email", "age"));
        if ($status != Aerospike::OK) {
            return $this->db->errorno();
        }
        $key = $this->db->initKey("test", "demo", "testKey");
        $this->db->get($key, $record);
        $this->db->remove($key, array(Aerospike::OPT_READ_TIMEOUT=>3000));
        $result = array_diff_assoc_recursive($record["bins"], array("name"=>"ellie"));
        if (!empty($result)) {
            return Aerospike::ERR_CLIENT;
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
    function testQueryWithOptionalArguments()
    {
        $where = $this->db->predicateBetween("age", 30, 39);
        $total = 0;
        $db = $this->db;
        $in_thirties = 0;
        $callback = function ($record) use (&$total, &$in_thirties, &$db) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("email", $record["bins"]) &&
                        !is_null($record["bins"]["email"])) {
                        if ($record["bins"]["email"]=="ellie") {
                            ; // do nothing
                            $key = $db->initKey("test", "demo", "testing");
                            $db->put($key, array("bin1"=>10));
                        } else {
                            $total += (int) $record["bins"]["age"];
                            $in_thirties++;
                        }
                    }
            }
        };
        $status = $this->db->query("test", "demo", $where, $callback,
            array("email", "age"));
        $key = $this->db->initKey("test", "demo", "testing");
        $this->db->get($key, $record);
        if(array_key_exists('bins',$record)) {
        if(10 == $record['bins']['bin1']) {
            $this->db->remove($key);
            return Aerospike::OK;
        }
        }
        return Aerospike::ERR_CLIENT;
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
    function testQueryWithOneOptionalArgument()
    {
        $where = $this->db->predicateBetween("age", 30, 39);
        $total = 0;
        $db= $this->db;
        $in_thirties = 0;
        $callback = function ($record) use (&$total, &$in_thirties, &$db) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("email", $record["bins"]) &&
                        !is_null($record["bins"]["email"])) {
                        if ($record["bins"]["email"] == "ellie") {
                            // do nothing
                            $key = $db->initKey("test", "demo", "testing");
                            $db->put($key, array("bin1"=>10));
                        } else {
                            $total += (int) $record["bins"]["age"];
                            $in_thirties++;
                        }
                    }
               }
        };
        $status = $this->db->query("test", "demo", $where, $callback,
            array( "email", "age"), array(Aerospike::OPT_READ_TIMEOUT=>2000));
        $key = $this->db->initKey("test", "demo", "testing");
        $this->db->get($key, $record);
        if(array_key_exists('bins',$record)) {
            if(10 == $record['bins']['bin1']) {
            $this->db->remove($key);
            return Aerospike::OK;
        }
        } else {
            return Aerospike::ERR_RECORD_NOT_FOUND;
        }
        return Aerospike::ERR_CLIENT;
        /*if ($status !== Aerospike::OK) {
            return $this->db->errorno();
        }
        return $status;*/
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
    function testQueryWithEmptyCallbackFunction()
    {
        $where = $this->db->predicateBetween("age", 20, 29);
        $callback =  function ($record) { };
        $status = $this->db->query( "test", "demo", $where, $callback,
            array("email", "age"));
        if ($status != Aerospike::OK) {
            return $this->db->errorno();
        }
        return $status;
    }
    /**
     * @test
     * Basic Query with where parameter missing
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testQueryWithWhereParameterMissing)
     *
     * @test_plans{1.1}
     */
    function testQueryWithWhereParameterMissing()
    {
        try {
            $total = 0;
            $in_thirties = 0;
            $callback = function ($record) use (&$total, &$in_thirties) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("email", $record["bins"]) &&
                        !is_null($record["bins"]["email"])) {
                            $total += (int) $record["bins"]["age"];
                            $in_thirties++;
                        }
                }
            };
            $status = $this->db->query("test", "demo", $callback,array( "email", "age"));
        } catch (ErrorException $e) {
            return Aerospike::ERR_PARAM;
        }
        return $status;
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
    function testQueryWithWhereContainingNullValue()
    {
        $where = $this->db->predicateEquals("age", "");
        $callback =  function ($record) {
            // we do not expect to be here
            return false;
        };
        try {
            $status = $this->db->query( "test", "demo", $where,
                $callback, array("email", "age"));
        } catch (ErrorException $e) {
            // we expect the query() to throw this exception
            return Aerospike::ERR_PARAM;
        }
        return AEROSPIKE::ERR_PARAM;
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
    function testQueryWithWhereContainingNullString()
    {
        $total = 0;
        $in_thirties = 0;
        $callback = function ($record) use (&$total, &$in_thirties) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("email", $record["bins"]) &&
                        !is_null($record["bins"]["email"])) {
                        $total += (int) $record["bins"]["age"];
                        $in_thirties++;
                    }
                }
        };
        $status = $this->db->query( "test", "demo", array(""), $callback,
            array( "email", "age"));
        if ($status != Aerospike::OK) {
            return $this->db->errorno();
        }
        return $status;
    }
    /**
     * @test
     * Query without where predicate.(i.e.scan)
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testQueryWithoutPredicate)
     *
     * @test_plans{1.1}
     */
    function testQueryWithoutPredicate()
    {
        $count = 0;
        $callback = function ($record) use (&$count) {
            if (strcmp("test", $record["key"]["ns"]) == 0 && strcmp("demo",
                $record["key"]["set"]) == 0) {
                    if (array_key_exists("email", $record["bins"]) &&
                        !is_null($record["bins"]["email"])) {
                            if ($record["bins"]["email"] == "john" ||
                                $record["bins"]["email"] == "smith" ||
                                $record["bins"]["email"] == "adam" ||
                                $record["bins"]["email"] == "ellie") {
                                    $count++;
                            }
                        }
                } else {
                    return (AEROSPIKE::ERR_PARAM);
                }
        };
        $status = $this->db->query( "test", "demo", array(), $callback, array("email", "age"));
        if ($status != Aerospike::OK) {
            return $this->db->errorno();
        }
        if ($count != 4) {
            return (AEROSPIKE::ERR_PARAM);
        }
        return $status;
    }
}
?>
