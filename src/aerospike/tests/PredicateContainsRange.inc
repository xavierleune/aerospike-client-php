<?php

require_once 'Common.inc';
/**
 *Basic Get tests
 */

class PredicateContainsRange extends AerospikeTestCommon
{
    private $bin;

    protected function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        if (!$this->db->isConnected()) {
            return $this->db->errorno();
		}
		$key = $this->db->initKey("test", "demo", "predicate_contains");
		$this->db->put($key,array("numeric_list"=>array(1, 2, 3, 4, 5)));
		$this->keys[] = $key;
		$this->db->put($key,array("string_list"=>array('a', 'b', 'c')));
		$this->keys[] = $key;
    }

    /**
     * @test
     * Basic Predicate Contains without parameter
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateContainsRangeNoParameter)
     *
     * @test_plans{1.1}
     */
    function testPredicateContainsRangeNoParameter()
    {
        try {
            $this->db->PredicateContainsRange();
        } catch (ErrorException $e) {
            return Aerospike::ERR_PARAM;
        }
        return Aerospike::OK;
    }
    /**
     * @test
     * Basic Predicate with value parameter is of type array
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateContainsRangeMinValIsArray)
     *
     * @test_plans{1.1}
     */
    function testPredicateContainsRangeMinValIsArray()
    {
		$predicate = $this->db->PredicateContainsRange('numeric_list',
			Aerospike::INDEX_TYPE_LIST, array(), 25);
		if (!$predicate) {
			return Aerospike::ERR_PARAM;
		}
        return Aerospike::OK;
    }
    /**
     * @test
     * Basic Predicate with value parameter is NULL
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateContainsRangeMaxValIsNULL)
     *
     * @test_plans{1.1}
     */
    function testPredicateContainsRangeMaxValIsNULL()
    {
		$predicate = $this->db->PredicateContainsRange('numeric_list', Aerospike::INDEX_TYPE_LIST, 2, NULL);
		if (!$predicate) {
			return Aerospike::ERR_PARAM;
		}
        return Aerospike::OK;
	}
    /**
     * @test
     * Basic Predicate with value is of type integer.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateContainsRangeIntegerValue)
     *
     * @test_plans{1.1}
     */
    function testPredicateContainsRangeIntegerValue()
    {
		$predicate = $this->db->PredicateContainsRange('numeric_list', Aerospike::INDEX_TYPE_LIST, 1, 4);
		if (!$predicate) {
			return Aerospike::ERR_PARAM;
		}
        return Aerospike::OK;
	}
    /**
     * @test
     * Basic Predicate with value is of type string.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateContainsRangeStringValue)
     *
     * @test_plans{1.1}
     */
    function testPredicateContainsRangeStringValue()
    {
		$predicate = $this->db->PredicateContainsRange('string_list', Aerospike::INDEX_TYPE_LIST, 'b', 'd');
		if (!$predicate) {
			return Aerospike::ERR_PARAM;
		}
        return Aerospike::OK;
	}
    /**
     * @test
     * Basic Predicate with index type is string.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateContainsRangeIndexTypeString)
     *
     * @test_plans{1.1}
     */
    function testPredicateContainsRangeIndexTypeString()
	{
		try {
			$predicate = $this->db->PredicateContainsRange('string_list', 'abcde', 'c', 'd');
        } catch (ErrorException $e) {
            return Aerospike::ERR_PARAM;
        }
        return Aerospike::OK;
	}
    /**
     * @test
     * Basic Predicate with bin name is of type integer.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateContainsRangeBinNameInteger)
     *
     * @test_plans{1.1}
     */
    function testPredicateContainsRangeBinNameInteger()
	{
		try {
			$predicate = $this->db->PredicateContainsRange(12, 'abcde', 'c', 'd');
        } catch (ErrorException $e) {
            return Aerospike::ERR_PARAM;
        }
        return Aerospike::OK;
	}
}
?>
