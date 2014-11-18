<?php

require_once("Common.inc");

class AerospikeSession extends AerospikeTestCommon
{
    protected function setUp(&$dummy_db, &$dummy_key) {
        session_save_path("test|sess|" . AEROSPIKE_CONFIG_NAME . ":" .
            AEROSPIKE_CONFIG_PORT);
        session_id("test_session");
        session_start();
    }

    /**
     * @test
     * Basic Session write.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testSessionRead)
     *
     * @test_plans{1.1}
     */
    function testSessionAWrite($dummy_db)
    {
        $_SESSION["username"] = "test-student";
        $_SESSION["age"] = 20;
        $grades = array("mathematics" => "AA",
            "data-structures" => "A",
            "operating-systems" => "AA");
        $_SESSION["grades"] = $grades;
        session_write_close();
	    $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("test", "sess", "test_session");
        $status = $db->exists($key, $metadata);
        $db->close();
        return $status;
    }

    /**
     * @test
     * Basic Session read.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testSessionWrite)
     *
     * @test_plans{1.1}
     */
    function testSessionBRead($dummy_db)
    {
        $grades = array("mathematics" => "AA",
            "data-structures" => "A",
            "operating-systems" => "AA");
        if (isset($_SESSION["username"]) && strcmp($_SESSION["username"], "test-student") == 0 &&
            isset($_SESSION["age"]) && $_SESSION["age"] == 20 &&
            isset($_SESSION["grades"]) &&
            empty(array_diff_assoc_recursive($_SESSION["grades"], $grades))) {
            return Aerospike::OK;
        } else {
            return Aerospike::ERR_CLIENT;
        }
    }

    /**
     * @test
     * Basic Session destroy.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testSessionWrite)
     *
     * @test_plans{1.1}
     */
    function testSessionZDestroy($dummy_db)
    {
        session_destroy();
	    $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("test", "sess", "test_session");
        $status = $db->exists($key, $metadata);
        $db->close();
        return $status;
    }

    /**
     * @test
     * Session read after destroy.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testSessionRead)
     *
     * @test_plans{1.1}
     */
    function testSessionZReadAfterDestroy($dummy_db)
    {
        if (empty($_SESSION)) {
            return Aerospike::OK;
        } else {
            return Aerospike::ERR_CLIENT;
        }
    }

}
?>
