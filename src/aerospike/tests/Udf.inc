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
            $db->deregister("bin_transform");
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
     * Basic Udf operation on int.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinIncrementByOffsetValue)
     *
     * @test_plans{1.1}
     */
    function testUdfOnInt($db, $key) 
    {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $returned = array();
        $key = array("ns" => "test", "set" => "demo", "key" => "udf_integer");
        if ($db->isConnected()) {
            $status = $db->put($key,array("bin1"=>10));
        } else {
            $db->close();
            return($db->errorno());
        }
        $udf_apply_status = $db->apply($key, "bin_transform",
            "bin_udf_operation_integer", array("bin1", 2, 20), $returned);
        if ($status != Aerospike::OK) {
            return ($db->error());
        }
        $status = $db->remove($key,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_NONE));
        return ($status);
    }

    /**
     * @test
     * Basic Udf operation on Float.
     *
     * @pre
     * Udf using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinIncrementByOffsetValue)
     *
     * @test_plans{1.1}
     */
    function testUdfOnFloat($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $returned = array();
        $key = array("ns" => "test", "set" => "demo", "key" => "test_float");
        if ($db->isConnected()) {
            $status = $db->put($key,array("bin1"=>10.20));
        } else{
            $db->close();
            return($db->errorno());
        }
        $status = $db->apply($key, "bin_transform", "bin_udf_operation_float", array("bin1",
            2.5, 20.5), $returned);
        if ($status != Aerospike::OK) {
            return ($db->error());
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
     * Variants: OO (testBinIncrementByOffsetValue)
     *
     * @test_plans{1.1}
     */
    function testUdfOnString($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $returned = array();
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
     * Variants: OO (testBinIncrementByOffsetValue)
     *
     * @test_plans{1.1}
     */
    function testUdfOnBool($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $returned = array();
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
        return ($status);
    }

    function testUdfOnMap($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $returned = array();
        $key = array("ns" => "test", "set" => "demo", "key" => "test_map");
        $map1= array("k1"=>10,"k2"=>5,"k3"=>6,"k4"=>7,"k5"=>8);
        if ($db->isConnected()) {
            $status = $db->put($key,array("bin1"=>$map1));
        } else{
            $db->close();
            return($db->errorno());
        }
        $status = $db->apply($key, "bin_transform", "access_map_udf", array("bin1"), $returned);
        if ($status != Aerospike::OK) {
            return ($db->error());
        }
        return ($status);
    }

    function testUdfOnList($db, $key) {
        if (!$db->isConnected()) {
            return ($db->error());
        }
        $returned = array();
        $key = array("ns" => "test", "set" => "demo", "key" => "test_list");
        $list = array("twitter", 100);
        if ($db->isConnected()) {
            $status = $db->put($key,array("bin1"=>$list), NULL,
                array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
        } else{
            $db->close();
            return($db->errorno());
        }
        $status = $db->apply($key, "bin_transform", "access_list_udf", array("bin1"), $returned);
        if ($status != Aerospike::OK) {
            return ($db->error());
        }
        return ($status);
    }
}
?>
