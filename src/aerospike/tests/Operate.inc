<?php
require_once 'Common.inc';

/**
 *Basic Append  opeartion tests
*/

class Operate  extends AerospikeTestCommon
{
    protected function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        if (!$this->db->isConnected()) {
            return $this->db->errorno();
        }
        $key = $this->db->initKey("test", "demo", "operate_key");
        $this->db->put($key,array("first_name"=>"John","last_name"=>"Smith","age"=>25));
        $this->keys[] = $key;
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
    function testOperatePositive() {
        $check = array("first_name" => "Mr John");
        $operations = array(
            array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
            array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name",
            "val" => "."),
            array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
            array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
        );
        $returned = array();
        $status = $this->db->operate($this->keys[0], $operations, $returned);
        if ($status !== AEROSPIKE::OK) {
            return $this->db->errorno();
        }
        $comp_res = array_diff_assoc_recursive($returned, $check);
        if (!empty($comp_res)) {
            return Aerospike::ERR;
        }
        else {
            return AEROSPIKE::OK;
        }
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
    function testOperateWithOptionsPositive() {
        $check = array("first_name" => "Mr John");
        $operations = array(
            array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
            array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name",
            "val" => "."),
            array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
            array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
        );
        $returned = array();
        $status = $this->db->operate($this->keys[0], $operations, $returned, array(Aerospike::OPT_WRITE_TIMEOUT=>2000));
        if ($status !== AEROSPIKE::OK) {
            return $this->db->errorno();
        }
        $comp_res = array_diff_assoc_recursive($returned, $check);
        if (!empty($comp_res)) {
            return Aerospike::ERR;
        }
        else {
            return AEROSPIKE::OK;
        }
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
    function testOperateSameBinNegative() {
        $check = array("first_name" => "Mr John");
        $operations = array(
            array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
            array("op" => Aerospike::OPERATOR_APPEND, "bin" => "first_name",
            "val" => ".")
        );
        $returned = array();
        $status = $this->db->operate($this->keys[0], $operations, $returned);
        if ($status !== AEROSPIKE::OK) {
            return $this->db->errorno();
        }
        $comp_res = array_diff_assoc_recursive($returned, $check);
        if (!empty($comp_res)) {
            return Aerospike::ERR;
        }
        else {
            return AEROSPIKE::OK;
        }
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
    function testOperateEmptyArgumentsNegative() {
        try {
            $status = $this->db->operate();
        } catch (ErrorException $e) {
            return Aerospike::ERR_CLIENT;
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
    function testOperateKeyIsEmptyStringNegative() {
        $check = array("first_name" => "Mr John");
        $operations = array(
            array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
            array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name",
            "val" => "."),
            array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
            array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
        );
        $returned = array();
        $status = $this->db->operate("", $operations, $returned,
            array(Aerospike::OPT_WRITE_TIMEOUT=>2000));
        if ($status !== AEROSPIKE::OK) {
            return $this->db->errorno();
        }
        $comp_res = array_diff_assoc_recursive($returned, $check);
        if (!empty($comp_res)) {
            return Aerospike::ERR;
        }
        else {
            return AEROSPIKE::OK;
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
    function testOperateReturnIsStringPositive() {
        $check = array("first_name" => "Mr John");
        $operations = array(
            array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
            array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name",
            "val" => "."),
            array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
            array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
        );

        $returned = "abc";
        $status = array($this->keys[0], $operations, $returned,
            array(Aerospike::OPT_WRITE_TIMEOUT=>2000));
        if ($status !== AEROSPIKE::OK) {
            return $this->db->errorno();
        }
        $comp_res = array_diff_assoc_recursive($returned, $check);
        if (!empty($comp_res)) {
            return Aerospike::ERR;
        }
        else {
            return AEROSPIKE::OK;
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
    function testOperateReturnNotPassedPositive() {
        $check = array("first_name" => "Mr John");
        $operations = array(
            array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
            array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name",
            "val" => "."),
            array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
            array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
        );

        $returned = "abc";
        $status = array($this->keys[0], $operations, NULL, array(Aerospike::OPT_WRITE_TIMEOUT=>2000));
        if ($status !== AEROSPIKE::OK) {
            return $this->db->errorno();
        }
        else {
            return Aerospike::OK;
        }
    }
}
?>
