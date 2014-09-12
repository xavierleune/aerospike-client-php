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
    protected function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("test", "demo", "Get_key");
        $db->put($key,array("first_name"=>"raunak","age"=>29));
        $key = $db->initKey("test", "demo", "Get_key1");
        $db->put($key,array("first_name"=>"pavan","age"=>27));
        $key = $db->initKey("test", "demo", "Get_key2");
        $db->put($key,array("first_name"=>"bhushan","age"=>22));
        $key = $db->initKey("test", "demo", "Get_key3");
        $db->put($key,array("first_name"=>"ajit","age"=>32));
        $register_status = $db->register("./tests/phpt/Aggregate/aggregate_udf.lua", "aggregate_udf");
        if ($register_status != Aerospike::OK) {
            return($db->errorno());
        }
    }

    protected function tearDown(&$db, &$key) {
        if (!$db->isConnected()) {
            $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
            $db = new Aerospike($config);
        }
        $this->removeKeys($db);
        $deregister_status = $db->deregister("aggregate_udf");
        if ($deregister_status != Aerospike::OK) {
            return($db->errorno());
        }
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
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->aggregate("aggregate_udf", "group_count", array("first_name"), "test", "demo", $where, $names);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        $testArray = array("raunak","pavan","bhushan");
        $result = array_diff_assoc_recursive($testArray, $names);
        if(!empty($result)) {
            return Aerospike::ERR;
        }
        return Aerospike::OK;
    }

    /**
     * @test
     * Aggregate - correct arguments with string values for bin
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
    function testAggregatePositiveString($db, $key)
    {
        $this->removeKeys($db);
        $key = $db->initKey("test", "demo", "Get_key");
        $db->put($key,array("first_name"=>"raunak","age"=>"29"));
        $key = $db->initKey("test", "demo", "Get_key1");
        $db->put($key,array("first_name"=>"pavan","age"=>"27"));
        $key = $db->initKey("test", "demo", "Get_key2");
        $db->put($key,array("first_name"=>"bhushan","age"=>"27"));
        $key = $db->initKey("test", "demo", "Get_key3");
        $db->put($key,array("first_name"=>"ajit","age"=>"32"));
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $where = $db->predicateEquals("age", 27);
        $status = $db->aggregate("aggregate_udf", "group_count", array("first_name"), "test", "demo", $where, $names);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        $testArray = array("pavan","bhushan");
        $result = array_diff_assoc_recursive($testArray, $names);
        $this->removeKeys($db);
        if(!empty($result)) {
            return Aerospike::ERR;
        }
        return Aerospike::OK;
    }
    /**
     * @test
     * Aggregate - correct arguments with blob values for bin
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
    function testAggregatePositiveBlob($db, $key)
    {
        $this->removeKeys($db);
        $key = $db->initKey("test", "demo", "Get_key");
        $db->put($key,array("first_name"=>"raunak","age"=>true));
        $key = $db->initKey("test", "demo", "Get_key1");
        $db->put($key,array("first_name"=>"pavan","age"=>false));
        $key = $db->initKey("test", "demo", "Get_key2");
        $db->put($key,array("first_name"=>"bhushan","age"=>true));
        $key = $db->initKey("test", "demo", "Get_key3");
        $db->put($key,array("first_name"=>"ajit","age"=>true));
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $where = $db->predicateEquals("age",true);
        $status = $db->aggregate("aggregate_udf", "group_count", array("first_name"), "test", "demo", $where, $names);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        $testArray = array("raunak","bhushan","ajit");
        $result = array_diff_assoc_recursive($testArray, $names);
        $this->removeKeys($db);
        if(!empty($result)) {
            return Aerospike::ERR;
        }
        return Aerospike::OK;
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
        $status = $db->aggregate("aggregate_udf", "group", array("first_name"),
            "t", "d", $where, $names);
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
        $status = $db->aggregate("agg", "group_count", array("first_name"), "test", "demo", $where, $names);
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
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->aggregate("aggregate_udf", "group", array("first_name"),
            "test", "demo", $where, $names);
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
        $status = $db->aggregate("aggregate_udf", "group", array("first_name"),
            "", "", $where, $names);
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
        $status = $db->aggregate("", "", array("first_name"), "test", "demo", $where, $names);
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
        $status = $db->aggregate("aggregate_udf", "group", $arg, "test", "demo", $where, $names);
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
        $status = $db->aggregate("aggregate_udf", "group", "", "test", "demo", $where, $names);
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
        $status = $db->aggregate("aggregate_udf", "group_count", array("first_name", "last_name"),"test", "demo", $where, $names);
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
        $register_status = $db->register("./tests/phpt/Aggregate/aggregate_udf_extra_parameter.lua", "aggregate_udf_extra_parameter");
        if ($register_status != Aerospike::OK) {
            return($db->errorno());
        }
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->aggregate("aggregate_udf_extra_parameter", "group_count", array("first_name"), "test", "demo", $where, $names);
        $degister_status = $db->deregister("aggregate_udf_extra_parameter");
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
        $register_status = $db->register("./tests/phpt/Aggregate/aggregate_udf_less_parameter.lua", "aggregate_udf_less_parameter");
        if ($register_status != Aerospike::OK) {
            return($db->errorno());
        }
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->aggregate("aggregate_udf_less_parameter", "group_count", array("first_name"), "test", "demo", $where, $names);
        $degister_status = $db->deregister("aggregate_udf_less_parameter");
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
    function testAggregateWithWhereParameterMissing($db,$key)
    {
        if ($db->isConnected()) {
            try {
                $status = $db->aggregate("aggregate_udf_less_parameter", "group_count", array("first_name"), "test", "demo", $names);
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
    function testAggregateWithWhereContainingStringValue($db,$key)
    {
        if ($db->isConnected()) {
            $where = $db->predicateBetween("age","21","ages");
            $status = $db->aggregate("aggregate_udf_less_parameter", "group_count", array("first_name"), "test", "demo", $where, $names);
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
    function testAggregateWithWhereContainingNullString($db,$key)
    {
        if ($db->isConnected()) {
            $status = $db->aggregate("aggregate_udf_less_parameter", "group_count", array("first_name"), "test", "demo", array(""), $names);
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
    function testAggregateWithWhereContainingBoolValue($db,$key)
    {
        if ($db->isConnected()) {
            $where = $db->predicateBetween("age", true, true);
            $status = $db->aggregate("aggregate_udf_less_parameter", "group_count", array("first_name"), "test", "demo", $where, $names);
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
    function testAggregateWithWhereContainingArrayValue($db,$key)
    {
        if ($db->isConnected()) {
            $where = $db->predicateBetween("age", array(""), array(""));
            $status = $db->aggregate("aggregate_udf_less_parameter", "group_count", array("first_name"), "test", "demo", $where, $names);
        } else {
            return($db->errorno());
        }
        if($status != Aerospike::OK) {
            return($db->errorno());
        }
        return ($status);
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
