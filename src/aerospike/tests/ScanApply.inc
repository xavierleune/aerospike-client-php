<?php
require_once 'Common.inc';
/**

 *Basic Aggregate tests

 */
class ScanApply extends AerospikeTestCommon
{
    /*
     * TODO: Remove these constants and map with actual Aerospike class
     * constants.
     */
    private $ages = array(29, 27, 22, 32);
    static $lua_user_path = "tests/lua";

    public function __construct() {
        self::$lua_user_path = file_exists("/opt/aerospike/client-php/usr-lua") ? "/opt/aerospike/client-php/usr-lua" : (file_exists("/usr/local/aerospike/client-php/usr-lua") ? "/usr/local/aerospike/client-php/usr-lua" : "tests/lua");
    }

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
            $db->register(self::$lua_user_path . "/my_udf.lua", "my_udf.lua");
        if ($register_status != Aerospike::OK) {
            return($db->errorno());
        }
        sleep(2);
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
     * ScanApply - All correct arguments
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
    function testScanApplyPositive($db, $key)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanApply("test", "demo", "my_udf", "mytransform", array(20), $scan_id);
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
            $ages_got[$i] = $get_record["bins"]["age"];
            $i++;
        }
        $key = $db->initKey("test", "demo", "Get_key1");
        if (Aerospike::OK == $db->get($key, $get_record, array("age"))) {
            $ages_got[$i] = $get_record["bins"]["age"];
            $i++;
        }
        $key = $db->initKey("test", "demo", "Get_key2");
        if (Aerospike::OK == $db->get($key, $get_record, array("age"))) {
            $ages_got[$i] = $get_record["bins"]["age"];
            $i++;
        }
        $key = $db->initKey("test", "demo", "Get_key3");
        if (Aerospike::OK == $db->get($key, $get_record, array("age"))) {
            $ages_got[$i] = $get_record["bins"]["age"];
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
     * ScanApply - namespace and set are incorrect.
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

    function testScanApplyUnknownNamespaceAndSetNegative($db)
    {
        if(!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanApply("t", "d", "my_udf", "mytransform", array(20), $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * ScanApply - Module name is incorrect.
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

    function testScanApplyUnknownModuleNegative($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->scanApply("test", "demo", "my_udf1", "mytransform", array(20), $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * ScanApply - Unknown function passed.
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

    function testScanApplyUnknownFunctionNegative($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanApply("test", "demo", "my_udf", "mytransform1", array(20), $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * ScanApply - Empty namespace and set.
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
    function testScanApplyEmptyNamespaceSetNegative($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanApply("", "", "my_udf", "mytransform", array(20), $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * ScanApply - Empty module and function
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

    function testScanApplyEmptyFunctionUDFNameNegative($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanApply("test", "demo", "", "", array(20), $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * ScanApply - String variable is passed as argument to UDF.
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

    function testScanApplyArgumentIsVariableNegative($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $arg = "args";
        $status = $db->scanApply("test", "demo", "my_udf", "mytransform", "age", $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }

    /**
     * @test
     * ScanApply - Empty argument passed to UDF.
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

    function testScanApplyArgsEmptyNegative($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $arg = "args";
        $status = $db->scanApply("test", "demo", "my_udf", "mytransform", "", $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * ScanApply - Extra parameter in call to lua function
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
    function testScanApplyExtraParameter($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanApply("test", "demo", "my_udf", "mytransform", array(20,30), $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * ScanApply - Lua function definition contains extra parameter
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
    function testScanApplyLuaExtraParameter($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $register_status =
            $db->register(self::$lua_user_path . "/my_udf_extra_parameter.lua",
                "my_udf_extra_parameter.lua");
        if ($register_status != Aerospike::OK) {
            return($db->errorno());
        }
        $status = $db->scanApply("test", "demo", "my_udf_extra_parameter", "mytransform", array(20), $scan_id);
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
     * ScanApply - Lua function definition contains less parameters
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
    function testScanApplyLuaLessParameter($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $register_status =
            $db->register(self::$lua_user_path . "/my_udf_less_parameter.lua",
                "my_udf_less_parameter.lua");
        if ($register_status != Aerospike::OK) {
            return($db->errorno());
        }
        $status = $db->scanApply("test", "demo", "my_udf_less_parameter", "mytransform", array(20), $scan_id);
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
     * ScanApply - positive with all arguments
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
    function testScanApplyPositiveWithAllArguments($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanApply("test", "demo", "my_udf", "mytransform", array(20), $scan_id, array(Aerospike::OPT_SCAN_PERCENTAGE=>100, Aerospike::OPT_SCAN_NOBINS=>false));
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
            $ages_got[$i] = $get_record["bins"]["age"];
            $i++;
        }
        $key = $db->initKey("test", "demo", "Get_key1");
        if (Aerospike::OK == $db->get($key, $get_record, array("age"))) {
            $ages_got[$i] = $get_record["bins"]["age"];
            $i++;
        }
        $key = $db->initKey("test", "demo", "Get_key2");
        if (Aerospike::OK == $db->get($key, $get_record, array("age"))) {
            $ages_got[$i] = $get_record["bins"]["age"];
            $i++;
        }
        $key = $db->initKey("test", "demo", "Get_key3");
        if (Aerospike::OK == $db->get($key, $get_record, array("age"))) {
            $ages_got[$i] = $get_record["bins"]["age"];
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
     * ScanApply - null string for scanid
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
    function testScanApplyScanIdIsNull($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $scan_id = "";
        $status = $db->scanApply("test", "demo", "my_udf", "mytransform", array(20), $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * ScanApply - percent is a string
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
    function testScanApplyPercentIsString($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanApply("test", "demo", "my_udf", "mytransform", array(20), $scan_id, array(Aerospike::OPT_SCAN_PERCENTAGE=>"abc"));
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * ScanApply - Scan priority is string
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
    function testScanApplyPriorityIsString($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanApply("test", "demo", "my_udf", "mytransform", array(20), $scan_id, array(Aerospike::OPT_SCAN_PRIORITY=>"SCAN_PRIORITY_AUTO"));
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * ScanApply - conncurent option is integer.
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
    function testScanApplyConcurrentIsInt($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanApply("test", "demo", "my_udf", "mytransform", array(20), $scan_id, array(Aerospike::OPT_SCAN_CONCURRENTLY=>true));
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
        return($status);
    }
}
?>
