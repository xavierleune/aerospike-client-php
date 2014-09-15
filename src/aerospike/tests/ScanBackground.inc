<?php
require_once 'Common.inc';
/**

 *Basic Aggregate tests

 */
class ScanBackground extends AerospikeTestCommon
{
    /*
     * TODO: Remove these constants and map with actual Aerospike class
     * constants.
     */
    private $ages = array(29, 27, 22, 32);
    protected function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("test", "demo", "Get_key");
        $db->put($key, array("first_name"=>"raunak", "age"=>($this->ages[0])));
        $key = $db->initKey("test", "demo", "Get_key1");
        $db->put($key, array("first_name"=>"pavan", "age"=>($this->ages[1])));
        $key = $db->initKey("test", "demo", "Get_key2");
        $db->put($key, array("first_name"=>"bhushan", "age"=>($this->ages[2])));
        $key = $db->initKey("test", "demo", "Get_key3");
        $db->put($key, array("first_name"=>"ajit", "age"=>($this->ages[3])));
         
        $register_status =
            $db->register("my_udf.lua", "my_udf.lua");
        if ($register_status != Aerospike::OK) {
            return($db->errorno());
        }
    }

    protected function tearDown(&$db, &$key) {
        if (!$db->isConnected()) {
            $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
            $db = new Aerospike($config);
        }
        $key = $db->initKey("test", "demo", "Get_key");
        $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
            =>Aerospike::POLICY_RETRY_NONE));
        for($i=1; $i<4; $i++) {
            $key=$db->initKey("test", "demo", "Get_key".$i);
            $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                =>Aerospike::POLICY_RETRY_NONE));
        }
        $deregister_status = $db->deregister("my_udf.lua");
        if ($deregister_status != Aerospike::OK) {
            return($db->errorno());
        }
        $db->close();
    }

    /**
     * @test
     * ScanBackground - All correct arguments
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
    function testScanBackgroundPositive($db, $key)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanBackground("my_udf", "mytransform", array(20),"test", "demo", $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        do {
            sleep(10);
            $status = $db->scanInfo($scan_id, $info);
            if ($status != Aerospike::OK) {
                return($db->errorno());
            }
        } while($info['status'] != Aerospike::SCAN_STATUS_COMPLETED);

        $ages_got = array();
        $i = 0;
        $key = $db->initKey("test", "demo", "Get_key");
        if (Aerospike::OK == $db->get($key, $get_record, array("age"))) {
            $ages_got[$i] = $get_record["age"];
            $i++;
        }
        $key = $db->initKey("test", "demo", "Get_key1");
        if (Aerospike::OK == $db->get($key, $get_record, array("age"))) {
            $ages_got[$i] = $get_record["age"];
            $i++;
        }
        $key = $db->initKey("test", "demo", "Get_key2");
        if (Aerospike::OK == $db->get($key, $get_record, array("age"))) {
            $ages_got[$i] = $get_record["age"];
            $i++;
        }
        $key = $db->initKey("test", "demo", "Get_key3");
        if (Aerospike::OK == $db->get($key, $get_record, array("age"))) {
            $ages_got[$i] = $get_record["age"];
            $i++;
        }
        for ($i = 0; $i < 4; $i++) {
            if ($ages_got[$i] == ($this->ages[$i]+20)) {
                continue;
            }
        }

        $key = $db->initKey("test", "demo", "Get_key");
        $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
            =>Aerospike::POLICY_RETRY_NONE));
        for($i=1; $i<4; $i++) {
            $key=$db->initKey("test", "demo", "Get_key".$i);
            $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                =>Aerospike::POLICY_RETRY_NONE));
        }
        return($status);
    }

    /**
     * @test
     * ScanBackground - namespace and set are incorrect.
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

    function testScanBackgroundUnknownNamespaceAndSetNegative($db)
    {
        if(!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanBackground("my_udf", "mytransform", array(20), "t", "d", $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * ScanBackground - Module name is incorrect.
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

    function testScanBackgroundUnknownModuleNegative($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->scanBackground("my_udf1", "mytransform", array(20), "test", "demo", $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * ScanBackground - Unknown function passed.
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

    function testScanBackgroundUnknownFunctionNegative($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanBackground("my_udf", "mytransform1", array(20), "test", "demo", $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * ScanBackground - Empty namespace and set.
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
    function testScanBackgroundEmptyNamespaceSetNegative($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanBackground("my_udf", "mytransform", array(20), "",  "", $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * ScanBackground - Empty module and function
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

    function testScanBackgroundEmptyFunctionUDFNameNegative($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanBackground("", "", array(20),"test", "demo", $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * ScanBackground - String variable is passed as argument to UDF.
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

    function testScanBackgroundArgumentIsVariableNegative($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $arg = "args";
        $status = $db->scanBackground("my_udf", "mytransform", "age", "test", "demo", $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * ScanBackground - Empty argument passed to UDF.
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

    function testScanBackgroundArgsEmptyNegative($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $arg = "args";
        $status = $db->scanBackground("my_udf", "mytransform", "", "test", "demo", $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * ScanBackground - Extra parameter in call to lua function
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
    function testScanBackgroundExtraParameter($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanBackground("my_udf", "mytransform", array(20,30), "test", "demo", $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * ScanBackground - Lua function definition contains extra parameter
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
    function testScanBackgroundLuaExtraParameter($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $register_status =
            $db->register("my_udf_extra_parameter.lua",
                "my_udf_extra_parameter.lua");
        if ($register_status != Aerospike::OK) {
            return($db->errorno());
        }
        $status = $db->scanBackground("my_udf_extra_parameter", "mytransform", array(20), "test", "demo", $scan_id);
        $deregister_status = $db->deregister("my_udf_extra_parameter.lua");
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
     * ScanBackground - Lua function definition contains less parameters
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
    function testScanBackgroundLuaLessParameter($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $register_status =
            $db->register("my_udf_less_parameter.lua",
                "my_udf_less_parameter.lua");
        if ($register_status != Aerospike::OK) {
            return($db->errorno());
        }
        $status = $db->scanBackground("my_udf_less_parameter", "mytransform", array(20), "test", "demo", $scan_id);
        $deregister_status = $db->deregister("my_udf_less_parameter.lua");
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
     * ScanBackground - positive with all arguments
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
    function testScanBackgroundPositiveWithAllArguments($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanBackground("my_udf", "mytransform", array(20), "test", "demo", $scan_id, 100, Aerospike::SCAN_PRIORITY_AUTO, false, false, array(Aerospike::OPT_READ_TIMEOUT=>2000));
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        do {
            sleep(10);
            $status = $db->scanInfo($scan_id, $info);
            if ($status != Aerospike::OK) {
                return($db->errorno());
            }
        } while($info['status'] != Aerospike::SCAN_STATUS_COMPLETED);
        $ages_got = array();
        $i = 0;
        $key = $db->initKey("test", "demo", "Get_key");
        if (Aerospike::OK == $db->get($key, $get_record, array("age"))) {
            $ages_got[$i] = $get_record["age"];
            $i++;
        }
        $key = $db->initKey("test", "demo", "Get_key1");
        if (Aerospike::OK == $db->get($key, $get_record, array("age"))) {
            $ages_got[$i] = $get_record["age"];
            $i++;
        }
        $key = $db->initKey("test", "demo", "Get_key2");
        if (Aerospike::OK == $db->get($key, $get_record, array("age"))) {
            $ages_got[$i] = $get_record["age"];
            $i++;
        }
        $key = $db->initKey("test", "demo", "Get_key3");
        if (Aerospike::OK == $db->get($key, $get_record, array("age"))) {
            $ages_got[$i] = $get_record["age"];
            $i++;
        }
        for ($i = 0; $i < 4; $i++) {
            if ($ages_got[$i] == ($this->ages[$i]+20)) {
                continue;
            }
        }

        $key = $db->initKey("test", "demo", "Get_key");
        $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
            =>Aerospike::POLICY_RETRY_NONE));
        for($i=1; $i<4; $i++) {
            $key=$db->initKey("test", "demo", "Get_key".$i);
            $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                =>Aerospike::POLICY_RETRY_NONE));
        }
        return($status);
    }
    /**
     * @test
     * ScanBackground - null string for scanid
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
    function testScanBackgroundScanIdIsNull($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $scan_id = "";
        $status = $db->scanBackground("my_udf", "mytransform", array(20), "test", "demo", $sca_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * ScanBackground - percent is a string
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
    function testScanBackgroundPercentIsString($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        try {
            $status = $db->scanBackground("my_udf", "mytransform", array(20), "test", "demo", $scan_id, "abc");
        } catch (Exception $e) {
            return($db->errorno());
        }
    }
    /**
     * @test
     * ScanBackground - Scan priority is string
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
    function testScanBackgroundPriorityIsString($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        try {
            $status = $db->scanBackground("my_udf", "mytransform", array(20), "test", "demo", $scan_id, 100, "SCAN_PRIORITY_AUTO");
        }
        catch (Exception $e) {
            return($db->errorno());
        }
    }
    /**
     * @test
     * ScanBackground - conncurent option is integer.
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
    function testScanBackgroundConcurrentIsInt($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanBackground("my_udf", "mytransform", array(20), "test", "demo", $scan_id, 100, Aerospike::SCAN_PRIORITY_AUTO, 1);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }
}
?>
