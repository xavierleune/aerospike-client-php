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
            $key = array("test", "demo", "operate_key");

            $operations = array(
                array("op" => Aerospike::OPERATOR_WRITE, "bin" => "name", "val" => "Ph.D"),
                array("op" => Aerospike::OPERATOR_APPEND, "bin" => "name", "val"
                => "Engineering"),
                array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                array("op" => Aerospike::OPERATOR_READ, "bin" => "name"),
                array("op" => Aerospike::OPERATOR_TOUCH)
            );

            $status = $db->operate($key, $operations, $returned);
            if ($status != AEROSPIKE_OK) {
                return $db->errorno();
            }
            if (strcmp($returned, "Ph.DEngineering") == 0) {
                return Aerospike::OK;
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
            $key = array("test", "demo", "operate_key");

            $operations = array(
                array("op" => Aerospike::OPERATOR_WRITE, "bin" => "name", "val" => "Ph.D"),
                array("op" => Aerospike::OPERATOR_APPEND, "bin" => "name", "val"
                => "Engineering"),
                array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                array("op" => Aerospike::OPERATOR_READ, "bin" => "name"),
                array("op" => Aerospike::OPERATOR_TOUCH)
            );

            $status = $db->operate($key, $operations, $returned,
                array(Aerospike::OPT_WRITE_TIMEOUT=>2000));
            if ($status != AEROSPIKE_OK) {
                return $db->errorno();
            }
            if (strcmp($returned, "Ph.DEngineering") == 0) {
                return Aerospike::OK;
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
            $operations = array(
                array("op" => Aerospike::OPERATOR_WRITE, "bin" => "name", "val" => "Ph.D"),
                array("op" => Aerospike::OPERATOR_APPEND, "bin" => "name", "val"
                => "Engineering"),
                array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                array("op" => Aerospike::OPERATOR_READ, "bin" => "name"),
                array("op" => Aerospike::OPERATOR_TOUCH)
            );

            $status = array("", $operations, $returned,
                array(Aerospike::OPT_WRITE_TIMEOUT=>2000));
            if ($status != AEROSPIKE_OK) {
                return $db->errorno();
            }
            if (strcmp($returned, "Ph.DEngineering") == 0) {
                return Aerospike::OK;
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
            $operations = array(
                array("op" => Aerospike::OPERATOR_WRITE, "bin" => "name", "val" => "Ph.D"),
                array("op" => Aerospike::OPERATOR_APPEND, "bin" => "name", "val"
                => "Engineering"),
                array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                array("op" => Aerospike::OPERATOR_READ, "bin" => "name"),
                array("op" => Aerospike::OPERATOR_TOUCH)
            );

            $status = array("", $operations, $returned,
                array(Aerospike::OPT_WRITE_TIMEOUT=>2000));
            if ($status != AEROSPIKE_OK) {
                return $db->errorno();
            }
            if (strcmp($returned, "Ph.DEngineering") == 0) {
                return Aerospike::OK;
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
    function testOperateReturnIsStringNegative($db, $key) {
        if ($db->isConnected()) {
            $key = array("test", "demo", "operate_key");
            $operations = array(
                array("op" => Aerospike::OPERATOR_WRITE, "bin" => "name", "val" => "Ph.D"),
                array("op" => Aerospike::OPERATOR_APPEND, "bin" => "name", "val"
                => "Engineering"),
                array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                array("op" => Aerospike::OPERATOR_READ, "bin" => "name"),
                array("op" => Aerospike::OPERATOR_TOUCH)
            );

            $returned = "abc";
            $status = array($key, $operations, $returned,
                array(Aerospike::OPT_WRITE_TIMEOUT=>2000));
            if ($status != AEROSPIKE_OK) {
                return $db->errorno();
            }
            if (strcmp($returned, "Ph.DEngineering") == 0) {
                return Aerospike::OK;
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
            $key = array("test", "demo", "operate_key");
            $operations = array(
                array("op" => Aerospike::OPERATOR_WRITE, "bin" => "name", "val" => "Ph.D"),
                array("op" => Aerospike::OPERATOR_APPEND, "bin" => "name", "val"
                => "Engineering"),
                array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                array("op" => Aerospike::OPERATOR_READ, "bin" => "name"),
                array("op" => Aerospike::OPERATOR_TOUCH)
            );

            $returned = "abc";
            $status = array($key, $operations, NULL,
                array(Aerospike::OPT_WRITE_TIMEOUT=>2000));
            if ($status != AEROSPIKE_OK) {
                return $db->errorno();
            }
            if (strcmp($returned, "Ph.DEngineering") == 0) {
                return Aerospike::OK;
            }
             
        } else {
            return($db->errorno());
        }
    }
}
?>
