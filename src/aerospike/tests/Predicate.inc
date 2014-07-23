<?php
require_once 'Common.inc';

/**
 * predicate helper functions tests
*/

class Predicate extends AerospikeTestCommon
{
    /**
     * @test
     * predicateEquals helper should return a specific associative array
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateEqualsHelper)
     *
     * @test_plans{1.1}
     */
    function testPredicateEqualsHelper() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $predicate = $db->predicateEquals("a", "test");
        $this->assertArrayHasKey('op', $predicate, $msg = "Predicate missing 'op' key.");
        $this->assertArrayHasKey('bin', $predicate, $msg = "Predicate missing 'bin' key.");
        $this->assertArrayHasKey('val', $predicate, $msg = "Predicate missing 'val' key.");
    }

    /**
     * @test
     * Test predicateEquals with integer value
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateEqualsValueInt)
     *
     * @test_plans{1.1}
     */
    function testPredicateEqualsValueInt() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $predicate = $db->predicateEquals("a", 1);
        $this->assertInternalType('integer', $predicate['val'], "Expected integer value");
        $this->assertEquals(1, $predicate['val'], "Expected a value of 1");
    }

    /**
     * @test
     * Test predicateEquals with string value
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateEqualsValueString)
     *
     * @test_plans{1.1}
     */
    function testPredicateEqualsValueString() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $predicate = $db->predicateEquals("a", "1");
        $this->assertInternalType('string', $predicate['val'], "Expected integer value");
        $this->assertEquals("1", $predicate['val'], "Expected a value of '1'");
    }

    /**
     * @test
     * predicateBetween helper should return a specific associative array
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateBetweenHelper)
     *
     * @test_plans{1.1}
     */
    function testPredicateBetweenHelper() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $predicate = $db->predicateBetween("c", 1, 10);
        $this->assertArrayHasKey('op', $predicate, $msg = "Predicate missing 'op' key.");
        $this->assertArrayHasKey('bin', $predicate, $msg = "Predicate missing 'bin' key.");
        $this->assertArrayHasKey('val', $predicate, $msg = "Predicate missing 'val' key.");
    }

    /**
     * @test
     * Test the predicateBetween value to be an array of (min, max) integers
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateBetweenArrayValueWithInts)
     *
     * @test_plans{1.1}
     */
    function testPredicateBetweenArrayValueWithInts() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $predicate = $db->predicateBetween("c", 1, 10);
        $this->assertInternalType('array', $predicate['val'], "Expected array value.");
        $this->assertEquals(array(1, 10), $predicate['val'], "Expected an array value of (1, 10)");
    }

    /**
     * @test
     * Test the predicateBetween value to be an array of (min, max) integers
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateBetweenArrayValueWithStrings)
     *
     * @test_plans{1.1}
     */
    function testPredicateBetweenArrayValueWithStrings() {
        $config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $predicate = $db->predicateBetween("c", "2", "24");
        $this->assertInternalType('array', $predicate['val'], "Expected array value.");
        $this->assertEquals(array(2, 24), $predicate['val'], "Expected an array value of (2, 24)");
    }

}
?>
