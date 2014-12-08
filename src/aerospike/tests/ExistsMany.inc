<?php
require_once 'Common.inc';

/**
 *Basic Key Exists/getMetaData opeartion tests
*/

class ExistsMany extends AerospikeTestCommon
{

    protected function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        if (!$this->db->isConnected()) {
            return $this->db->errorno();
        }
        $key = $this->db->initKey("test", "demo", "existsMany1");
        $this->db->put($key, array("binA"=>10));
        $this->keys[] = $key;
        $key = $this->db->initKey("test", "demo", "existsMany2");
        $this->db->put($key, array("binB"=>20));
        $this->keys[] = $key;
        $key = $this->db->initKey("test", "demo", "existsMany3");
        $this->db->put($key, array("binC"=>30));
        $this->keys[] = $key;
    }

    /**
     * @test
     * Basic existsMany operation with all arguments.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testExistsManyPositive)
     *
     * @test_plans{1.1}
     */
    function testExistsManyPositive() {
        $status = $this->db->existsMany($this->keys, $metadata, array(Aerospike::OPT_READ_TIMEOUT=>3000));
        if ($status !== AEROSPIKE::OK) {
            return $this->db->errorno();
        }
        if (empty($metadata)) {
            return Aerospike::ERR;
        }
        return $status;
    }
    /**
     * @test
     * Basic existsMany operation with empty key array.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testExistsManyEmptyKeyArrayNegative)
     *
     * @test_plans{1.1}
     */
    function testExistsManyEmptyKeyArrayNegative() {
        $status = $this->db->existsMany(array(), $metadata);
        return $status;
    }
    /**
     * @test
     * Basic existsMany operation with empty string passed 
       in place of key array .
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testExistsManyKeyEmptyStringNegative)
     *
     * @test_plans{1.1}
     */
    function testExistsManyKeyEmptyStringNegative() {
        try {
            $status = $this->db->existsMany("", $metadata);
        } catch (ErrorException $e) {
            return Aerospike::ERR_PARAM;
        }
        return $status;
    }
    /**
     * @test
     * Basic existsMany operation with no arguments.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testExistsManyNoArgumentsNegative)
     *
     * @test_plans{1.1}
     */
    function testExistsManyNoArgumentsNegative() {
        try {
            $status = $this->db->existsMany();
        } catch (ErrorException $e) {
            return Aerospike::ERR_PARAM;
        }
        return $status;
    }
    /**
     * @test
     * Basic existsMany, Metadata is empty string variable.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testExistsManyMetadataEmptyStringPositive)
     *
     * @test_plans{1.1}
     */
    function testExistsManyMetadataEmptyStringPositive() {
        $metadta = "";
        $status = $this->db->existsMany($this->keys, $metadata);
        return $status;
    }
    /**
     * @test
     * Basic existsMany, Metadata is of type string.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testExistsManyMetadataStringVariablePositive)
     *
     * @test_plans{1.1}
     */
    function testExistsManyMetadataStringVariablePositive() {
        $metadata = "abc";
        $status = $this->db->existsMany($this->keys, $metadata);
        return $status;
    }
    /**
     * @test
     * Basic existsMany, Options is of type integer.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testExistsManyOptionsIntegerNegative)
     *
     * @test_plans{1.1}
     */
    function testExistsManyOptionsIntegerNegative() {
        try {
            $status = $this->db->existsMany($this->keys, $metadata, 20);
        } catch (ErrorException $e) {
            return Aerospike::ERR_PARAM;
        }
        return $status;
    }
    /**
     * @test
     * Basic existsMany, initKey with digest.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testExistsManyInitKeyWithDigest)
     *
     * @test_plans{1.1}
     */
    function testExistsManyInitKeyWithDigest() {
        $key1 = $this->db->initKey("test", "demo", "existsMany1", true);
        $this->db->put($key1, array("binA"=>10));
        $this->keys[] = $key1;
        $key2 = $this->db->initKey("test", "demo", "existsMany2", true);
        $this->db->put($key2, array("binB"=>20));
        $this->keys[] = $key2;

        $keys = array($key1, $key2);
        $status = $this->db->existsMany($keys, $metadata, array(Aerospike::OPT_READ_TIMEOUT=>3000));
        if ($status !== AEROSPIKE::OK) {
            return $this->db->errorno();
        }
        if (empty($metadata)) {
            return Aerospike::ERR;
        }
        return $status;
    }
}
