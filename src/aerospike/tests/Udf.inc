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

    function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $udf_register_status =
            $db->register("bin_transform.lua", "bin_transform");
        if ($udf_register_status != Aerospike::OK) {
            return ($db->error());
        }
    }

    function tearDown(&$db, &$key) {
        if ($db->isConnected()) {
            $db->deregister("bin_transform", array(Aerospike::OPT_WRITE_TIMEOUT
            => 3000));
            $db->close();
        } else {
            $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
                "port"=>AEROSPIKE_CONFIG_PORT)));
            $db = new Aerospike($config);
            $db->deregister("bin_transform");
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
     * Variants: OO (testUdfRegisterModule)
     *
     * @test_plans{1.1}
     */
    function testUdfRegisterModule($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $udf_register_status = $db->register("hello.lua", "hello", Aerospike::UDF_TYPE_LUA,
            array(Aerospike::OPT_WRITE_TIMEOUT => 5000));
        if ($udf_register_status != Aerospike::OK) {
            return ($db->error());
        }
        return($udf_register_status);
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
     * Variants: OO (testUdfOnInt)
     *
     * @test_plans{1.1}
     */
    function testUdfOnInt($db, $key) 
    {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $key = array("ns" => "test", "set" => "demo", "key" => "udf_integer");
        if ($db->isConnected()) {
            $status = $db->put($key,array("bin1"=>10));
        } else {
            $db->close();
            return($db->errorno());
        }
        $returned = "key";
        $udf_apply_status = $db->apply($key, "bin_transform",
            "bin_udf_operation_integer", array("bin1", 2, 20), $returned,
            array(Aerospike::OPT_WRITE_TIMEOUT => 2000));
        if ($status != Aerospike::OK) {
            return ($db->error());
        }
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
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
     * Variants: OO (testUdfOnString)
     *
     * @test_plans{1.1}
     */
    function testUdfOnString($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $key = array("ns" => "test", "set" => "demo", "key" => "test_string");
        if ($db->isConnected()) {
            $status = $db->put($key,array("bin1"=>"hello"));
        } else{
            $db->close();
            return($db->errorno());
        }
        $status = $db->apply($key, "bin_transform", "bin_udf_operation_string", array("bin1",
            "world"), $returned);
        if ($status != Aerospike::OK) {
            return ($db->error());
        }
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
        return ($status);
    }

    /**
     * @test
     * Basic Udf operation on bool.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testUdfOnBool)
     *
     * @test_plans{1.1}
     */
    function testUdfOnBool($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $key = array("ns" => "test", "set" => "demo", "key" => "test_bool");
        if ($db->isConnected()) {
            $status = $db->put($key,array("bin1"=>true), NULL,
                array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
        } else{
            $db->close();
            return($db->errorno());
        }
        $status = $db->apply($key, "bin_transform", "bin_udf_operation_bool", array("bin1"), $returned);
        if ($status != Aerospike::OK) {
            return ($db->error());
        }
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
        return ($status);
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
     * Variants: OO (testUdfOnList)
     *
     * @test_plans{1.1}
     */
    function testUdfOnList($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $returned = array();
        $key = array("ns" => "test", "set" => "demo", "key" => "test_list");
        $list = array(94, 5, 25);
        if ($db->isConnected()) {
            $status = $db->put($key, array("bin1"=>$list), NULL);
        } else{
            $db->close();
            return($db->errorno());
        }
        $status = $db->apply($key, "bin_transform", "list_iterate", array("bin1", 2), $returned);
        if ($status != Aerospike::OK) {
            return ($db->error());
        }

        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
        return ($status);
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
     * Variants: OO (testUdfOnList)
     *
     * @test_plans{1.1}
     */
    function testUdfOnListReturnsList($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $returned = array();
        $key = array("ns" => "test", "set" => "demo", "key" => "list_iterate");
        $list = array(94, 5, 25);
        if ($db->isConnected()) {
            $status = $db->put($key, array("bin1"=>$list), NULL);
        } else{
            $db->close();
            return($db->errorno());
        }
        $returned = array();
        $status = $db->apply($key, "bin_transform", "list_iterate_returns_list", array("bin1", 2), $returned);
        if ($status != Aerospike::OK) {
            return ($db->error());
        }
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
        return ($status);
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
     * Variants: OO (testUdfOnMap)
     *
     * @test_plans{1.1}
     */
    function testUdfOnMap($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $returned = array();
        $key = array("ns" => "test", "set" => "demo", "key" => "test_map");
        $map = array("k30"=>6, "k20"=>5, "k10"=>1);
        if ($db->isConnected()) {
            $status = $db->put($key, array("bin1"=>$map), NULL);
        } else{
            $db->close();
            return($db->errorno());
        }
        $status = $db->apply($key, "bin_transform", "map_iterate",
            array("bin1", 55555), $returned);
        if ($status != Aerospike::OK) {
            return ($db->error());
        }
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));

        return ($status);
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
    function testUdfRegisterUnknownModule($db, $key) {
        if ($db->isConnected()) {
            $udf_register_status = $db->register("unknown.lua","unknown_udf");
        } else{
            $db->close();
            return($db->errorno());
        }
        if ($udf_register_status != Aerospike::OK) {
            return ($db->error());
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
    function testUdfRegisterEmptyStringAsModule($db, $key) {
        if ($db->isConnected()) {
            $udf_register_status = $db->register("","");
        } else{
            $db->close();
            return($db->errorno());
        }
        if ($udf_register_status != Aerospike::OK) {
            return ($db->error());
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
     * Variants: OO (testUdfDeregisterUnknownModule)
     *
     * @test_plans{1.1}
     */
    function testUdfDeregisterUnknownModule($db, $key) {
        if ($db->isConnected()) {
            $udf_deregister_status = $db->deregister("unknown_udf");
        } else{
            $db->close();
            return($db->errorno());
        }
        if ($udf_deregister_status != Aerospike::OK) {
            return ($db->error());
        }
        return ($udf_deregister_status);
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
     * Variants: OO (testUdfApplyReturnsMap)
     *
     * @test_plans{1.1}
     */
    function testUdfApplyReturnsMap($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
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
        $status = $db->apply($key, "bin_transform", "map_iterate_returns_map",
            array("bin1", 85), $returned);
        if ($status != Aerospike::OK) {
            return ($db->error());
        }
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
        return ($status);
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
     * Variants: OO (testUdfApplyReturnsMapNeg)
     *
     * @test_plans{1.1}
     */
    function testUdfApplyReturnsMapNeg($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $key = array("ns" => "test", "set" => "demo", "key" => "test_map");
        $map = array("k30"=>6, "k20"=>5, "k10"=>1);
        if ($db->isConnected()) {
            $status = $db->put($key, array("bin1"=>$map), NULL);
        } else{
            $db->close();
            return($db->errorno());
        }
        $ret_val = "aa";

        $status = $db->apply($key, "bin_transform", "map_iterate_returns_map",
            array("bin1", 5), $ret_val);
        if ($status != Aerospike::OK) {
            return ($db->error());
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
     * Variants: OO (testUdfListRegisteredLUAModules)
     *
     * @test_plans{1.1}
     */
    function testUdfListRegisteredLUAModules($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        else {
            $modules = array();
            $status = $db->listRegistered($modules, Aerospike::UDF_TYPE_LUA,
                array(Aerospike::OPT_READ_TIMEOUT => 5000));
            if ($status != Aerospike::OK) {
                return($db->error());
            } 
            var_dump($modules);
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
     * Variants: OO (testUdfListRegisteredModules)
     *
     * @test_plans{1.1}
     */
    function testUdfListRegisteredModules($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        else {
            $modules = array();
            $status = $db->listRegistered($modules);
            if ($status != Aerospike::OK) {
                return($db->error());
            }
            var_dump($modules);
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
     * Variants: OO (testUdfListRegisteredModulesNeg)
     *
     * @test_plans{1.1}
     */
    function testUdfListRegisteredModulesNeg($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        else {
            $modules = 5;
            $status = $db->listRegistered($modules);
            if ($status != Aerospike::OK) {
                return($db->error());
            }
            var_dump($modules);
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
     * Variants: OO (testUdfGetRegisteredModule)
     *
     * @test_plans{1.1}
     */
    function testUdfGetRegisteredModule($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        else {
            $code = array();
            $status = $db->getRegistered("bin_transform", $code,
                array(Aerospike::OPT_READ_TIMEOUT => 2000));
            if ($status != Aerospike::OK) {
                return($db->error());
            }
            var_dump($code);
        } 
        return($status);
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
     * Variants: OO (testUdfGetNotRegisteredModule)
     *
     * @test_plans{1.1}
     */
    function testUdfGetNotRegisteredModule($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        else {
            $code = array();
            $status = $db->getRegistered("unknown", $code);
            if ($status != Aerospike::OK) {
                return($db->error());
            }
            var_dump($code);
        } 
        return($status);
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
     * Variants: OO (testUdfReturnsRecord)
     *
     * @test_plans{1.1}
     */
    function testUdfReturnsRecord($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $key = array("ns" => "test", "set" => "demo", "key" =>
            "udf_returns_record");
        $map = array("k30"=>6, "k20"=>5, "k10"=>1);
        $list = array(1, 2, 89);
        if ($db->isConnected()) {
            $status = $db->put($key, array("bin1"=>$map, "bin2"=>$list));
        } else{
            $db->close();
            return($db->errorno());
        }
        $returned = array();
        $status = $db->apply($key, "bin_transform", "udf_returns_record", NULL, $returned);
        if ($status != Aerospike::OK) {
            return ($db->error());
        }
        return ($status);
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
     * Variants: OO (testUdfGetRegisteredModule)
     *
     * @test_plans{1.1}
     */
    function testUdfRegisterWrongModuleandPath($db, $key) {
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
     * Variants: OO (testUdfAcceptsNothingAndReturnsNothing)
     *
     * @test_plans{1.1}
     */
    function testUdfAcceptsNothingAndReturnsNothing($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $key = array("ns" => "test", "set" => "demo", "key" =>
            "udf_returns_nothing");
        $status = $db->apply($key, "bin_transform", "udf_without_arg_and_return");
        if ($status != Aerospike::OK) {
            return ($db->error());
        }
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
     * Variants: OO (testUdfApplyPutByteArray)
     *
     * @test_plans{1.1}
     */
    function testUdfApplyPutByteArray($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $key = array("ns" => "test", "set" => "demo", "key" =>
            "udf_put_bytes");
        $status = $db->apply($key, "bin_transform", "udf_put_bytes", array("bin1"));
        if ($status != Aerospike::OK) {
            return ($db->error());
        }
        return ($status);
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
     * Variants: OO (testUdfApplyModuleIsEmptyString)
     *
     * @test_plans{1.1}
     */
    function testUdfApplyModuleIsEmptyString($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $key = array("ns" => "test", "set" => "demo", "key" =>
            "udf_apply_modules_empty_string");
        $status = $db->apply($key, "", "udf_put_bytes", array("bin1"));
        if ($status != Aerospike::OK) {
            return ($db->error());
        }
        return ($status);
    }
}
?>
