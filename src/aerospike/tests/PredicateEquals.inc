<?php

require_once 'Common.inc';
/**
 *Basic Get tests
 */

class PredicateEquals extends AerospikeTestCommon
{
    /*
     * TODO: Remove these constants and map with actual Aerospike class
     * constants.
     */
    static $status_ok = 2000;
    static $status_err = 2001;
    static $status_get_record_fail = 2002;
    static $put_rec = array("Greet"=>"World_end");

    protected function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $bin = "age";
    }

    protected function tearDown(&$db, &$key) {
        $db->close();
    }
    /**
     * @test
     * Basic Predicate Equals without parameter
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
    function testPredicateEqualsNoParameter($db)
    {
        if ($db->isConnected()) {
            try {
                $status = $db->predicateEquals();
            } catch (ErrorException $e) {
                return self::$status_ok;
            }
            return($status);
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
    function testPredicateEqualsValueIsBool($db)
    {
        if ($db->isConnected()) {
            try {
                $db->predicateEquals("age",true);
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
    function testPredicateEqualsValueIsArray($db)
    {
        if ($db->isConnected()) {
            try {
                $db->predicateEquals("age",array("age","email"));
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
     * Basic Predicate Value is same as bin
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
    function testPredicateEqualsValueIsSame($db)
    {
        if ($db->isConnected()) {
            try {
                $db->predicateEquals(1,1);
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
     * Basic PredicateEquals has only one argument
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
    function testPredicateEqualsValueHasOneArgument($db)
    {
        if ($db->isConnected()) {
            try {
                $db->predicateEquals("age");
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
    function testPredicateEquals($db)
    {
        if ($db->isConnected()) {
            try {
                $ans=$db->predicateEquals("age",30);
            } catch (ErrorException $e) {
                return self::$status_ok;
            }
            echo "ans is".var_dump($ans);
            $op = array_diff_assoc_recursive($ans,array("bin"=>"age", "op"=>"=", "val"=>30));
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
