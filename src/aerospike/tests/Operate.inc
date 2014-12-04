<?php
require_once 'Common.inc';

/**
 *Basic Append  opeartion tests
*/

class Operate  extends AerospikeTestCommon
{
    protected function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("test", "demo", "operate_key");
        $db->put($key,array("first_name"=>"John"));
        $db->put($key,array("last_name"=>"Smith"));
        $db->put($key,array("age"=>25));
    }

    function tearDown(&$db, &$key) {
        if ($db->isConnected()) {
            $db->close();
        }
   }
    
    /**
     * @test
     * Basic Operate on a single record.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testOperatePositive)
     *
     * @test_plans{1.1}
     */
    function testOperatePositive($db, $key) {
        if ($db->isConnected()) {
            $check = array("first_name" => "Mr John");
            $operations = array(
                array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
                array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name",
                "val" => "."),
                array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
            );
            $returned = array();
            $status = $db->operate($key, $operations, $returned);
            if ($status != AEROSPIKE::OK) {
                return $db->errorno();
            }
            $comp_res = array_diff_assoc_recursive($returned, $check);
                if (!empty($comp_res)) {
                    return Aerospike::ERR;
                }
                else {
                    return AEROSPIKE::OK;
                }
 
        } else {
            return($db->errorno());
        }
        $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
             Aerospike::POLICY_RETRY_NONE));
        return $status;

    }
    /**
     * @test
     * Basic Operate with options. 
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testOperateWithOptionsPositive)
     *
     * @test_plans{1.1}
     */
    function testOperateWithOptionsPositive($db, $key) {
        if ($db->isConnected()) {
            $check = array("first_name" => "Mr John");
            $operations = array(
                array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
                array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name",
                "val" => "."),
                array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
            );
            $returned = array();
            $status = $db->operate($key, $operations, $returned,
                array(Aerospike::OPT_WRITE_TIMEOUT=>2000,
                    Aerospike::OPT_POLICY_COMMIT_LEVEL=>Aerospike::POLICY_COMMIT_LEVEL_MASTER));
            if ($status != AEROSPIKE::OK) {
                return $db->errorno();
            }
            $comp_res = array_diff_assoc_recursive($returned, $check);
                if (!empty($comp_res)) {
                    return Aerospike::ERR;
                }
                else {
                    return AEROSPIKE::OK;
                }
             
        } else {
            return($db->errorno());
        }
        $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
            Aerospike::POLICY_RETRY_NONE));
        return $status;
    }
    /**
     * @test
     * Basic Operate with multiple operations on same bin
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testOperatePositive)
     *
     * @test_plans{1.1}
     */
    function testOperateSameBinNegative($db, $key) {
        if ($db->isConnected()) {
            $check = array("first_name" => "Mr John");
            $operations = array(
                array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
                array("op" => Aerospike::OPERATOR_APPEND, "bin" => "first_name",
                "val" => ".")
            );
            $returned = array();
            $status = $db->operate($key, $operations, $returned);
            if ($status != AEROSPIKE::OK) {
                return $db->errorno();
            }
            $comp_res = array_diff_assoc_recursive($returned, $check);
            if (!empty($comp_res)) {
                    return Aerospike::ERR;
                }
                else {
                    return AEROSPIKE::OK;
                }
 
        } else {
            return($db->errorno());
        }
        $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
             Aerospike::POLICY_RETRY_NONE));
        return $status;
    }
    /**
     * @test
     * Basic Operate with empty arguments.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testOperateEmptyArgumentsNegative)
     *
     * @test_plans{1.1}
     */
    function testOperateEmptyArgumentsNegative($db, $key) {
        if ($db->isConnected()) {
            try {
                $status = $db->operate();
            } catch (ErrorException $e) {
                return Aerospike::ERR;
                $db->close();
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic bin value append when append value not string
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testOperateKeyIsEmptyStringNegative)
     *
     * @test_plans{1.1}
     */
    function testOperateKeyIsEmptyStringNegative($db, $key) {
        if ($db->isConnected()) {
            $check = array("first_name" => "Mr John");
            $operations = array(
                array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
                array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name",
                "val" => "."),
                array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
            );
            $returned = array();
            $status = $db->operate("", $operations, $returned,
                array(Aerospike::OPT_WRITE_TIMEOUT=>2000,
                        Aerospike::OPT_POLICY_COMMIT_LEVEL=>Aerospike::POLICY_COMMIT_LEVEL_ALL));
            if ($status != AEROSPIKE::OK) {
                return $db->errorno();
            }
            $comp_res = array_diff_assoc_recursive($returned, $check);
                if (!empty($comp_res)) {
                    return Aerospike::ERR;
                }
                else {
                    return AEROSPIKE::OK;
                }
             
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic return value is string
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testOperateReturnIsStringNegative)
     *
     * @test_plans{1.1}
     */
    function testOperateReturnIsStringPositive($db, $key) {
        if ($db->isConnected()) {
            $check = array("first_name" => "Mr John");
            $operations = array(
                array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
                array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name",
                "val" => "."),
                array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
            );

            $returned = "abc";
            $status = array($key, $operations, $returned,
                array(Aerospike::OPT_WRITE_TIMEOUT=>2000));
            if ($status != AEROSPIKE::OK) {
                return $db->errorno();
            }
             
            $comp_res = array_diff_assoc_recursive($returned, $check);
                if (!empty($comp_res)) {
                    return Aerospike::ERR;
                }
                else {
                    return AEROSPIKE::OK;
                }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic bin value append when append value not string
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testOperateReturnIsStringNegative)
     *
     * @test_plans{1.1}
     */
    function testOperateReturnNotPassedPositive($db, $key) {
        if ($db->isConnected()) {
            $check = array("first_name" => "Mr John");
            $operations = array(
                array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
                array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name",
                "val" => "."),
                array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
            );

            $returned = "abc";
            $status = array($key, $operations, NULL,
                array(Aerospike::OPT_WRITE_TIMEOUT=>2000));
            if ($status != AEROSPIKE::OK) {
                return $db->errorno();
            }
            else {
                return Aerospike::OK;
            } 
        } else {
            return($db->errorno());
        }
    }

    /**
     * @test
     * Basic Operate on on increment without previous record
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testOperatePositive)
     *
     * @test_plans{1.1}
     */
    function testOperatePositiveIncrement($db, $key) {
        if ($db->isConnected()) {
            $db->removeBin($key, array("age"));
            $check = array("first_name" => "John", "last_name" => "Smith", "age" => 1);
            $operations = array(
                array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1)
            );
            $returned = array();
            $status = $db->operate($key, $operations, $returned);
            if ($status != AEROSPIKE::OK) {
                return $db->errorno();
            }
            $status = $db->get( $key, $bin_arr);
            $comp_res = array_diff_assoc_recursive($bin_arr['bins'], $check);
            if (!empty($comp_res)) {
                return Aerospike::ERR;
            }
            else {
                return AEROSPIKE::OK;
            } 
        } else {
            return($db->errorno());
        }
        $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
             Aerospike::POLICY_RETRY_NONE));
        return $status;

    }
}
?>
