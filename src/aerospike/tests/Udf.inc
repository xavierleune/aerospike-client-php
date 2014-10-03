<?php
require_once 'Common.inc';

/**
 *Basic Increment opeartion tests
 */

class Udf extends AerospikeTestCommon
{
    /*
     * TODO: Remove these constants and map with actual Aerospike class
     * constants.
     */

    protected function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $udf_register_status =
            $db->register("tests/lua/bin_transform.lua", "module.lua");
        if ($udf_register_status != Aerospike::OK) {
            return ($db->errorno());
        }
    }

    protected function tearDown(&$db, &$key) {
        if ($db->isConnected()) {
            $db->deregister("module.lua", array(Aerospike::OPT_WRITE_TIMEOUT=> 3000));
            $db->close();
        } else {
            $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
                "port"=>AEROSPIKE_CONFIG_PORT)));
            $db = new Aerospike($config);
            $udf_deregister_status = $db->deregister("module.lua",
                array(Aerospike::OPT_WRITE_TIMEOUT => 3000));
            if ($udf_deregister_status != Aerospike::OK) {
                return ($db->errorno());
            }
            $db->close();
        }
    }

    /**
     * @test
     * Registers UDF Module.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfPositiveRegisterModule)
     *
     * @test_plans{1.1}
     */
    function testUdfPositiveRegisterModule($db, $key) {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        $udf_register_status = $db->register("tests/lua/bin_transform.lua", "module.lua", Aerospike::UDF_TYPE_LUA,
            array(Aerospike::OPT_WRITE_TIMEOUT => 5000));
        if ($udf_register_status != Aerospike::OK) {
            return ($db->errorno());
        }
        return($udf_register_status);
    }
    /**
     * @test
     * Register UDF module which is not present.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfRegisterUnknownModule)
     *
     * @test_plans{1.1}
     */
    function testUdfNegativeRegisterUnknownModule($db, $key) {
        if ($db->isConnected()) {
            $udf_register_status = $db->register("unknown.lua","unknown_udf");
        } else{
            $db->close();
            return($db->errorno());
        }
        if ($udf_register_status != Aerospike::OK) {
            return ($db->errorno());
        }
        return ($udf_register_status);
    }

    /**
     * @test
     * Register empty string as a UDF module.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfRegisterEmptyStringAsModule)
     *
     * @test_plans{1.1}
     */
    function testUdfNegativeRegisterEmptyStringAsModule($db, $key) {
        if ($db->isConnected()) {
            $udf_register_status = $db->register("","");
        } else{
            $db->close();
            return($db->errorno());
        }
        if ($udf_register_status != Aerospike::OK) {
            return ($db->errorno());
        }
        return ($udf_register_status);
    }

    /**
     * @test
     * Deregisters a UDF module which is not registered.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfNegativeDeregisterUnknownModule)
     *
     * @test_plans{1.1}
     */
    function testUdfNegativeDeregisterUnknownModule($db, $key) {
        if ($db->isConnected()) {
            $udf_deregister_status = $db->deregister("unknown_udf");
        } else{
            $db->close();
            return($db->errorno());
        }
        if ($udf_deregister_status != Aerospike::OK) {
            return ($db->errorno());
        }
        return ($udf_deregister_status);
    }
    /**
     * @test
     * Basic Udf operation on int.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfPositiveApplyOnInteger)
     *
     * @test_plans{1.1}
     */
    function testUdfPositiveApplyOnInteger($db, $key) 
    {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        $key = $db->initKey("test","demo", "udf_integer");
        if ($db->isConnected()) {
            $status = $db->put($key,array("bin1"=>10));
        } else {
            $db->close();
            return($db->errorno());
        }
        $returned = "aa";
        $udf_apply_status = $db->apply($key, "module",
            "bin_udf_operation_integer", array("bin1", 2, 20), $returned,
            array(Aerospike::OPT_WRITE_TIMEOUT => 2000));
        if ($status != Aerospike::OK) {
            return ($db->errorno());
        }
        if ($returned !== 32) {
            return "Value returned from apply() is not int(32)";
        }
        return ($status);
    }

    /**
     * @test
     * Basic Udf operation on String.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfPositiveApplyOnString)
     *
     * @test_plans{1.1}
     */
    function testUdfPositiveApplyOnString($db, $key) {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        $key = $db->initKey("test","demo", "udf_for_string");
        if ($db->isConnected()) {
            $status = $db->put($key,array("bin1"=>"hello"));
        } else {
            $db->close();
            return($db->errorno());
        }
        $status = $db->apply($key, "module", "bin_udf_operation_string", array("bin1",
            "world"), $returned);
        if ($status != Aerospike::OK) {
            return ($db->errorno());
        }
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
        if($returned == "helloworld") {
            return Aerospike::OK;
        }
        return Aerospike::ERR;
    }

    /**
     * @test
     * Apply UDF on a record containing List.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfPositiveApplyOnList)
     *
     * @test_plans{1.1}
     */
    function testUdfPositiveApplyOnList($db, $key) {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        $key = $db->initKey("test", "demo", "list_iterate");
        $list = array(94, 5, 25);
        if ($db->isConnected()) {
            $status = $db->put($key, array("bin1"=>$list), NULL);
        } else{
            $db->close();
            return($db->errorno());
        }
        $returned = array();
        $status = $db->apply($key, "module", "list_iterate", array("bin1", 2),
            $returned);
        if ($status != Aerospike::OK) {
            return ($db->errorno());
        }
        $mystatus = $db->get($key, $returned);
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
        $testList = array("bin1" => array(94, 222, 25, 58));
        $result = array_diff_assoc_recursive($testList, $returned["bins"]);
        if(!empty($result)) {
            return Aerospike::ERR;
        }
        return Aerospike::OK;
    }

    /**
     * @test
     * Apply UDF on a record containing List and UDF returns a List.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfPositiveApplyOnListReturnsList)
     *
     * @test_plans{1.1}
     */
    function testUdfPositiveApplyOnListReturnsList($db, $key) {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        $returned = array();
        $key = $db->initKey("test", "demo", "list_iterate");
        $list = array(94, 5, 25);
        if ($db->isConnected()) {
            $status = $db->put($key, array("bin1"=>$list), NULL);
        } else{
            $db->close();
            return($db->errorno());
        }
        $returned = array();
        $status = $db->apply($key, "module", "list_iterate_returns_list",
            array("bin1", 2), $returned);
        if ($status != Aerospike::OK) {
            return ($db->errorno());
        }
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
        $testList = array(94, 222, 25, 58);
        $result = array_diff_assoc_recursive($testList, $returned);
        if(!empty($result)) {
            return Aerospike::ERR;
        }
        return Aerospike::OK;
    }
    /**
     * @test
     * Apply UDF on record containing Map.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfPositiveApplyOnMap)
     *
     * @test_plans{1.1}
     */
    function testUdfPositiveApplyOnMap($db, $key) {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        $returned = array();
        $key = $db->initKey("test", "demo", "test_map");
        $map = array("k30"=>6, "k20"=>5, "k10"=>1);
        if ($db->isConnected()) {
            $status = $db->put($key, array("bin1"=>$map), NULL);
        } else{
            $db->close();
            return($db->errorno());
        }
        $status = $db->apply($key, "module", "map_iterate",
            array("bin1", 55555), $returned);
        if ($status != Aerospike::OK) {
            return ($db->errorno());
        }
        $mystatus = $db->get($key, $returned);
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
        $testMap = array("bin1"=>array("k30"=>55555, "k20"=>55555, "k10"=>55555));
        $result = array_diff_assoc_recursive($returned["bins"], $testMap);
        if (!empty($result)) {
            return Aerospike::ERR;
        }
        return Aerospike::OK;
    }

    /**
     * @test
     * Apply UDF on a record and UDF returns a Map.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfPositiveApplyReturnsMap)
     *
     * @test_plans{1.1}
     */
    function testUdfPositiveApplyReturnsMap($db, $key) {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        $key = array("ns" => "test", "set" => "demo", "key" => "test_map");
        $map = array("k30"=>6, "k20"=>5, "k10"=>1);
        if ($db->isConnected()) {
            $status = $db->put($key, array("bin1"=>$map), NULL);
        } else{
            $db->close();
            return($db->errorno());
        }
        $returned = array();
        $status = $db->apply($key, "module", "map_iterate_returns_map",
            array("bin1", 85), $returned);
        if ($status != Aerospike::OK) {
            return ($db->errorno());
        }
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
        $testMap = array("k30"=>85, "k20"=>85, "k10"=>85);
        $result = array_diff_assoc_recursive($testMap, $returned);
        if(!empty($result)) {
            return Aerospike::ERR;
        }
        return Aerospike::OK;
    }

    /**
     * @test
     * Apply UDF on a record and UDF returns a Map(Array) but we are passing variable.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfPositiveApplyReturnsMapPassingVariable)
     *
     * @test_plans{1.1}
     */
    function testUdfPositiveApplyReturnsMapPassingVariable($db, $key) {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        $key = $db->initKey("test", "demo", "test_map");
        $map = array("k30"=>6, "k20"=>5, "k10"=>1);
        if ($db->isConnected()) {
            $status = $db->put($key, array("bin1"=>$map), NULL);
        } else{
            $db->close();
            return($db->errorno());
        }
        $ret_val = "aa";

        $status = $db->apply($key, "module", "map_iterate_returns_map",
            array("bin1", 5), $ret_val);
        if ($status != Aerospike::OK) {
            return ($db->errorno());
        }
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
        $testMap = array("k30"=>5, "k20"=>5, "k10"=>5);
        $result = array_diff_assoc_recursive($testMap, $ret_val);
        if(!empty($result)) {
            return Aerospike::ERR;
        }
        return Aerospike::OK;
    }

    /**
     * @test
     * Apply UDF on a record which returns record.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfPositiveApplyReturnsRecord)
     *
     * @test_plans{1.1}
     */
    function testUdfPositiveApplyReturnsRecord($db, $key) {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        $key = $db->initKey("test", "demo", "udf_returns_record");
        $map = array("k30"=>6, "k20"=>5, "k10"=>1);
        $list = array(1, 2, 89);
        if ($db->isConnected()) {
            $status = $db->put($key, array("bin1"=>$map, "bin2"=>$list));
        } else{
            $db->close();
            return($db->errorno());
        }
        $status = $db->apply($key, "module", "udf_returns_record", NULL, $returned);
        if ($status != Aerospike::OK) {
            return ($db->errorno());
        }
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
        if ($returned != "") {
            return Aerospike::ERR;
        }
        return Aerospike::OK;
    }

    /**
     * @test
     * Apply UDF on a record which accepts nothing and returns nothing.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfPositiveApplyAcceptsNothingAndReturnsNothing)
     *
     * @test_plans{1.1}
     */
    function testUdfPositiveApplyAcceptsNothingAndReturnsNothing($db, $key) {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        $key = $db->initKey("test", "demo", "udf_put");

        if ($db->isConnected()) {
            $status = $db->put($key, array("bin1"=>"aa"));
        } else{
            $db->close();
            return($db->errorno());
        }
        $return_val = NULL;
        $status = $db->apply($key, "module", "udf_without_arg_and_return",
            NULL, $return_val, array(Aerospike::OPT_WRITE_TIMEOUT => 2000));
        if ($status != Aerospike::OK) {
            return ($db->errorno());
        }
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
        return ($status);
    }

    /**
     * @test
     * Apply UDF on record, Where UDF puts bytes array in DB.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfPositiveApplyPutByteArray)
     *
     * @test_plans{1.1}
     */
    function testUdfPositiveApplyPutByteArray($db, $key) {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        $key = $db->initKey("test", "demo", "udf_put_bytes");
        $list = array(94, 5, 25);
        if ($db->isConnected()) {
            $status = $db->put($key, array("bin1"=>$list), NULL);
        } else {
            $db->close();
            return($db->errorno());
        }
        $status = $db->apply($key, "module", "udf_put_bytes", array("bin1"),
            $returned);
        if ($status != Aerospike::OK) {
            return ($db->errorno());
        }
        if (!empty($returned)) {
             return Aerospike::ERR;
        }
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
        return $status;
    }

    /**
     * @test
     * Apply UDF on record, Where Module name is empty string.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfApplyNegativeModuleIsEmptyString)
     *
     * @test_plans{1.1}
     */
    function testUdfApplyNegativeModuleIsEmptyString($db, $key) {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        $key = $db->initKey("test", "demo", "udf_apply_modules_empty_string");
        $status = $db->apply($key, "", "udf_put_bytes", array("bin1"));
        if ($status != Aerospike::OK) {
            return ($db->errorno());
        }
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
        return ($status);
    }
    /**
     * @test
     * List registered LUA(UDF_TYPE_LUA) modules.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfPositiveListRegisteredLUAModules)
     *
     * @test_plans{1.1}
     */
    function testUdfPositiveListRegisteredLUAModules($db, $key) {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        else {
            $modules = array();
            $status = $db->listRegistered($modules, Aerospike::UDF_TYPE_LUA,
                array(Aerospike::OPT_READ_TIMEOUT => 5000));
            if ($status != Aerospike::OK) {
                return($db->errorno());
            }
            foreach ($modules as $moduleeach) {
                if ($moduleeach['name'] == 'module.lua') {
                    return Aerospike::OK;
                }
            }
            return Aerospike::ERR;
        }
        return($status);
    }

    /**
     * @test
     * List registered all type(UDF_TYPE_*) modules.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfPositiveListRegisteredModules)
     *
     * @test_plans{1.1}
     */
    function testUdfPositiveListRegisteredModules($db, $key) {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        else {
            $modules = array();
            $status = $db->listRegistered($modules);
            if ($status != Aerospike::OK) {
                return($db->errorno());
            }
            foreach ($modules as $moduleeach) {
                if ($moduleeach['name']=='module.lua') {
                    return Aerospike::OK;
                }
            }
            return Aerospike::ERR;
        }
        return($status);
    }

    /**
     * @test
     * List registered UDF modules and PHP script passing variable.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfPositiveListRegisteredModulesPassingVariable)
     *
     * @test_plans{1.1}
     */
    function testUdfPositiveListRegisteredModulesPassingVariable($db, $key) {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        else {
            $modules = 5;
            $status = $db->listRegistered($modules);
            if ($status != Aerospike::OK) {
                return($db->errorno());
            }
            foreach ($modules as $moduleeach) {
                if ($moduleeach['name'] == 'module.lua') {
                    return Aerospike::OK;
                }
            }
            return Aerospike::ERR;
        }
        return($status);
    }

    /**
     * @test
     * Get registered UDF's code.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfPositiveGetRegisteredModule)
     *
     * @test_plans{1.1}
     */
    function testUdfPositiveGetRegisteredModule($db, $key) {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        else {
            $code = array();
            $status = $db->getRegistered("module.lua", $code,
                Aerospike::UDF_TYPE_LUA,
                array(Aerospike::OPT_READ_TIMEOUT => 2000));
            if ($status != Aerospike::OK) {
                return($db->errorno());
            }
        }
        $handle = fopen("tests/lua/bin_transform.lua","r");
        $mycode = fread($handle,filesize("tests/lua/bin_transform.lua"));
        $result = strcmp($code, $mycode);
        if(!$result) {
            return Aerospike::OK;
        }
        return Aerospike::ERR;
    }

    /**
     * @test
     * Get contents of a module which is not registered.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfNegativeGetNotRegisteredModule)
     *
     * @test_plans{1.1}
     */
    function testUdfNegativeGetNotRegisteredModule($db, $key) {
        if (!$db->isConnected()) {
            return ($db->errorno());
        }
        else {
            $code = array();
            $status = $db->getRegistered("unknown", $code,
                Aerospike::UDF_TYPE_LUA);
            if ($status != Aerospike::OK) {
                return($db->errorno());
            }
        } 
        return($status);
    }
    /**
     * @test
     * apply Udf on a record and returned parameter of udf is initialized to
     * NULL.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfPositiveReturnvalueInitializedNULL)
     *
     * @test_plans{1.1}
     */
     function testUdfPositiveReturnvalueInitializedNULL($db, $key) {
         if (!$db->isConnected()) {
             return ($db->errorno());
         }
         $key = $db->initKey("test","demo", "udf_list_no_return_value");
         $list = array(94, 5, 25);
         if ($db->isConnected()) {
             $status = $db->put($key, array("bin1"=>$list), NULL);
         } else {
             $db->close();
             return($db->errorno());
         }
         $returned = NULL;
         $udf_apply_status = $db->apply($key, "module",
             "list_iterate", array("bin1", 2, 20), $returned,
             array(Aerospike::OPT_WRITE_TIMEOUT => 2000));
         if ($udf_apply_status != Aerospike::OK) {
             return ($db->errorno());
         }
         if (!empty($returned)) {
             return Aerospike::ERR;
         }
         $status = $db->remove($key,
             array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
         return ($status);
     }
}
?>
