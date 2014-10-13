<?php
require_once 'Common.inc';
/**

 *Basic Aggregate tests

 */
class Aggregate extends AerospikeTestCommon
{
    /*
     * TODO: Remove these constants and map with actual Aerospike class
     * constants.
     */
    static $parameter_exception = 2000;
    static $status_err = 2001;
    static $lua_user_path = "tests/lua";

    public function __construct() {
        self::$lua_user_path = file_exists("/opt/aerospike/client-php/usr-lua") ? "/opt/aerospike/client-php/usr-lua" : (file_exists("/usr/local/aerospike/client-php/usr-lua") ? "/usr/local/aerospike/client-php/usr-lua" : "tests/lua");
    }

    protected function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("test", "demo", "Get_key");
        $db->put($key,array("first_name"=>"raunak", "age"=>29));
        $key = $db->initKey("test", "demo", "Get_key1");
        $db->put($key,array("first_name"=>"pavan", "age"=>27));
        $key = $db->initKey("test", "demo", "Get_key2");
        $db->put($key,array("first_name"=>"bhushan", "age"=>22));
        $key = $db->initKey("test", "demo", "Get_key3");
        $db->put($key,array("first_name"=>"ajit", "age"=>32));
        $register_status =
            $db->register(self::$lua_user_path . "/stream_udf.lua", "stream_udf.lua");
        if ($register_status != Aerospike::OK) {
            return($db->errorno());
        }
        $db->createIndex("test", "demo", "age", Aerospike::INDEX_TYPE_INTEGER,
            "age_index");
        sleep(2);
    }

    protected function tearDown(&$db, &$key) {
        if (!$db->isConnected()) {
            $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
                "port"=>AEROSPIKE_CONFIG_PORT)));
            $db = new Aerospike($config);
        }
        $this->removeKeys($db);
        $deregister_status = $db->deregister("stream_udf.lua");
        if ($deregister_status != Aerospike::OK) {
            return($db->errorno());
        }
        $db->dropIndex("test", "age_index");
        $db->close();
    }

    /**
     * @test
     * Aggregate - first name of emp where age in between
     * 20 and 29.
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
    function testAggregatePositive($db, $key)
    {
	sleep(10);
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->aggregate("test", "demo", $where,
            "stream_udf", "group_count", array("first_name"),
            $returned);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        if (strcmp("test", $returned["key"]["ns"]) == 0 && strcmp("demo",
            $returned["key"]["set"]) == 0) {
                if (array_key_exists("raunak", $returned["bins"]) &&
                    array_key_exists("pavan", $returned["bins"]) &&
                    array_key_exists("bhushan", $returned["bins"])) {
                        return Aerospike::OK;
                    } else {
                        return Aerospike::ERR;
                    }
            }
    }

    /**
     * @test
     * Aggregate - Applying aggregation with where predicate containing a
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
    function testAggregateNegativeSecondaryIndexNotFound($db, $key)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $where = $db->predicateEquals("first_nameaa", "raunak");
        $status = $db->aggregate("test", "demo", $where,
            "stream_udf", "group_count", array("first_name"), $names);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return $status;
    }
    /**
     * @test
     * Aggregate - Where predicate not satisfied by any record.
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
    function testAggregatePositiveEmptyAggregationResult($db, $key)
    {
	sleep(10);
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $where = $db->predicateEquals("age", 1000);
        $status = $db->aggregate("test", "demo", $where,
            "stream_udf", "group_count", array("first_name"), $names);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        if (empty($names["bins"])) {
            return Aerospike::OK;
        }
        return Aerospike::ERR;
    }
    /**
     * @test
     * Aggregate - namespace and set are incorrect.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAggregateUnknownNamespaceAndSetNegative)
     *
     * @test_plans{1.1}
     */

    function testAggregateUnknownNamespaceAndSetNegative($db, $key)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->aggregate("t", "d", $where,
            "stream_udf", "group", array("first_name"), $names);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * Aggregate - Module name is incorrect.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAggregateUnknownModuleNegative)
     *
     * @test_plans{1.1}
     */

    function testAggregateUnknownModuleNegative($db, $key)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->aggregate("test", "demo", $where,
            "agg", "group_count", array("first_name"), $names);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * Aggregate - Unknown function passed.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAggregateUnknownFunctionNegative)
     *
     * @test_plans{1.1}
     */

    function testAggregateUnknownFunctionNegative($db, $key)
    {
        gc_enable();
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->aggregate("test", "demo", $where,
            "stream_udf", "group", array("first_name"), $names);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * Aggregate - Empty namespace and set.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAggregateEmptyNamespaceSetNegative)
     *
     * @test_plans{1.1}
     */

    function testAggregateEmptyNamespaceSetNegative($db, $key)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->aggregate("", "", $where,
            "stream_udf", "group", array("first_name"), $names);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * Aggregate - Empty module and function
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAggregateEmptyFunctionUDFNameNegative)
     *
     * @test_plans{1.1}
     */

    function testAggregateEmptyFunctionUDFNameNegative($db, $key)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->aggregate("test", "demo", $where,
            "", "", array("first_name"), $names);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * Aggregate - String variable is passed as argument to UDF.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAggregateArgumentIsVariableNegative)
     *
     * @test_plans{1.1}
     */

    function testAggregateArgumentIsVariableNegative($db, $key)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $arg = "args";
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->aggregate("test", "demo", $where,
            "stream_udf", "group", $arg, $names);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * Aggregate - Empty argument passed to UDF.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAggregateArgsEmptyNegative)
     *
     * @test_plans{1.1}
     */

    function testAggregateArgsEmptyNegative($db, $key)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $arg = "args";
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->aggregate("test", "demo", $where,
            "stream_udf", "group", "", $names);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * Aggregate - Extra parameter in call to lua function
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
    function testAggregateExtraParameter($db, $key)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->aggregate("test", "demo", $where,
            "stream_udf", "group_count", array("first_name", "last_name"),
            $names);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * Aggregate - Lua function definition contains extra parameter
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
    function testAggregateLuaExtraParameter($db, $key)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $register_status =
            $db->register(self::$lua_user_path . "/aggregate_udf_extra_parameter.lua",
                "aggregate_udf_extra_parameter");
        if ($register_status != Aerospike::OK) {
            return($db->errorno());
        }
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->aggregate("test", "demo", $where,
            "aggregate_udf_extra_parameter", "group_count",
            array("first_name"), $names);
        $deregister_status = $db->deregister("aggregate_udf_extra_parameter");
        if ($deregister_status != Aerospike::OK) {
            return($db->errorno());
        }
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * Aggregate - Lua function definition contains less parameters
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
    function testAggregateLuaLessParameter($db, $key)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $register_status =
            $db->register(self::$lua_user_path . "/aggregate_udf_less_parameter.lua",
                "aggregate_udf_less_parameter.lua");
        if ($register_status != Aerospike::OK) {
            return($db->errorno());
        }
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->aggregate("test", "demo", $where,
            "aggregate_udf_less_parameter", "group_count",
            array("first_name"), $names);
        $deregister_status = $db->deregister("aggregate_udf_less_parameter.lua");
        if ($deregister_status != Aerospike::OK) {
            return($db->errorno());
        }
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * Basic Aggregate with where missing
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
    function testAggregateWithWhereParameterMissing($db, $key)
    {
        if ($db->isConnected()) {
            try {
                $register_status =
                    $db->register(self::$lua_user_path . "/aggregate_udf_less_parameter.lua",
                    "aggregate_udf_less_parameter.lua");
                if ($register_status != Aerospike::OK) {
                    return($db->errorno());
                }
                $status = $db->aggregate("test", "demo", "aggregate_udf_less_parameter",
                    "group_count", array("first_name"), $names);
                $deregister_status = $db->deregister("aggregate_udf_less_parameter.lua");
                if ($deregister_status != Aerospike::OK) {
                    return($db->errorno());
                }
            } catch (ErrorException $e) {
                return self::$parameter_exception;
            }
            return self::$status_err;
        } else {
            return($db->errorno());
        }
    }

    /**
     * @test
     * Aggregate with where containing string value
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
    function testAggregateWithWhereContainingStringValue($db, $key)
    {
        if ($db->isConnected()) {
            try {
                $register_status =
                    $db->register(self::$lua_user_path . "/aggregate_udf_less_parameter.lua",
                    "aggregate_udf_less_parameter.lua");
                if ($register_status != Aerospike::OK) {
                    return($db->errorno());
                }
                $where = $db->predicateBetween("age","21","ages");
                $status = $db->aggregate("test", "demo", $where,
                    "aggregate_udf_less_parameter", "group_count",
                    array("first_name"), $names);
                $deregister_status = $db->deregister("aggregate_udf_less_parameter.lua");
                if ($deregister_status != Aerospike::OK) {
                    return($db->errorno());
                }
            } catch (ErrorException $e) {
                return self::$parameter_exception;
            }
        } else {
            return($db->errorno());
        }
        return $status;
    }

    /**
     * @test
     * Aggregate with where containing null string
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
    function testAggregateWithWhereContainingNullString($db, $key)
    {
        if ($db->isConnected()) {
            $register_status =
                $db->register(self::$lua_user_path . "/aggregate_udf_less_parameter.lua",
                "aggregate_udf_less_parameter.lua");
            if ($register_status != Aerospike::OK) {
                return($db->errorno());
            }
            $status = $db->aggregate("test", "demo", array(""),
                "aggregate_udf_less_parameter", "group_count",
                array("first_name"), $names);
            $deregister_status = $db->deregister("aggregate_udf_less_parameter.lua");
            if ($deregister_status != Aerospike::OK) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
        return ($status);
    }
    /**
     * @test
     * Aggregate with where containing bool value
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
    function testAggregateWithWhereContainingBoolValue($db, $key)
    {
	sleep(10);
        if ($db->isConnected()) {
            $where = $db->predicateBetween("age", true, true);
            $status = $db->aggregate("test", "demo", $where,
                "stream_udf", "group_count", array("first_name"),
                $names);
        } else {
            return($db->errorno());
        }
        return ($status);
    }
    /**
     * @test
     * Aggregate with where containing array value
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
    function testAggregateWithWhereContainingArrayValue($db, $key)
    {
        if ($db->isConnected()) {
            try {
                $register_status =
                    $db->register(self::$lua_user_path . "/aggregate_udf_less_parameter.lua",
                    "aggregate_udf_less_parameter.lua");
                if ($register_status != Aerospike::OK) {
                    return($db->errorno());
                }
                $where = $db->predicateBetween("age", array(""), array(""));
                $status = $db->aggregate("test", "demo", $where,
                    "aggregate_udf_less_parameter","group_count",
                    array("first_name"), $names);
                $deregister_status = $db->deregister("aggregate_udf_less_parameter.lua");
                if ($deregister_status != Aerospike::OK) {
                    return($db->errorno());
                }
            } catch (ErrorException $e) {
                return self::$parameter_exception;
            }
        } else {
            return($db->errorno());
        }
        return $status;
    }

    function removeKeys(&$db) {
        $key = $db->initKey("test", "demo", "Get_key");
        $db->remove($key, array(Aerospike::POLICY_RETRY_NONE));
        $key = $db->initKey("test", "demo", "Get_key1");
        $db->remove($key, array(Aerospike::POLICY_RETRY_NONE));
        $key = $db->initKey("test", "demo", "Get_key2");
        $db->remove($key, array(Aerospike::POLICY_RETRY_NONE));
        $key = $db->initKey("test", "demo", "Get_key3");
        $db->remove($key, array(Aerospike::POLICY_RETRY_NONE));
    }
}

?>
