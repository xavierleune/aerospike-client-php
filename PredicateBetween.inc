<?php

require_once 'Common.inc';
/**
 *Basic Get tests
 */

class PredicateBetween extends AerospikeTestCommon
{
    /*
     * TODO: Remove these constants and map with actual Aerospike class
     * constants.
     */
    static $status_ok = 2000;
    static $status_err = 2001;
    static $status_get_record_fail = 2002;

    protected function setUp(&$db, &$bin) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $bin = "age";
    }

    protected function tearDown(&$db, &$bin) {
        $db->close();
    }
    /**
     * @test
     * Basic Predicate Between without parameter
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
    function testPredicateBetweenNoParameter($db,$bin)
    {
        if ($db->isConnected()) {
            try {
                $db->predicateBetween();
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
     * Basic Predicate value is Bool
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
    function testPredicateBetweenValueIsBool($db,$bin)
    {
        if ($db->isConnected()) {
            try {
                $db->predicateBetween($bin,true,true);
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
     * Basic Predicate Value is Array
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
    function testPredicateBetweenValueIsArray($db,$bin)
    {
        if ($db->isConnected()) {
            try {
                $db->predicateBetween($bin,array("age","email"),array(2,3));
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
     * Basic PredicateBetween has only one argument
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
    function testPredicateBetweenValueHasOneArgument($db,$bin)
    {
        if ($db->isConnected()) {
            try {
                $db->predicateBetween($bin);
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
     * Basic Predicate with correct arguments
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
    function testPredicateBetween($db,$bin)
    {
        if ($db->isConnected()) {
            try {
                $ans=$db->predicateBetween($bin,30,40);
            } catch (ErrorException $e) {
                return self::$status_ok;
            }
            echo "Ans is".var_dump($ans);
            $op = array_diff_assoc_recursive($ans, array("bin"=>"age", "op"=>"BETWEEN", "val"=>array(30,40)));
            if(!empty($op)) {
                return Aerospike::ERR;
            }
            return self::$status_ok;
        } else {
            return($db->errorno());
        }
    }
}
?>
