<?php
class GetNodes extends AerospikeTestCommon
{
    /*
     * TODO: Remove these constants and map with actual Aerospike class
     * constants.
     */
    static $status_ok = 2000;
    static $status_err = 2001;
    static $status_get_record_fail = 2002;

    protected function setUp(&$db, &$host) {
        $host = array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT);
        $config = array("hosts"=>array($host));
        $db = new Aerospike($config);
        $ns = "test";
        $set = "demo";
        $key = $db->initKey("test", "demo", "Get_key");
        $db->put($key, array("email"=>"raunak"));
        $key = $db->initKey("test", "demo", "Get_key1");
        $db->put($key, array("email"=>"pavan"));
    }

    protected function tearDown(&$db, &$host) {
        $key = $db->initKey("test", "demo", "Get_key");
        $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
            =>Aerospike::POLICY_RETRY_NONE));
        $key = $db->initKey("test", "demo", "Get_key1");
        $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
            =>Aerospike::POLICY_RETRY_NONE));
        $db->close();
    }
    /**
     * @test
     * Basic GetNodes positive
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Get information of nodes
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testGetNodesPositive($db)
    {
        if ($db->isConnected()) {
            try {
                return($db->getNodes());
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic GetNodes with parameters
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Test should fail
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testGetNodesWithParameter($db)
    {
        if ($db->isConnected()) {
            try {
                return($db->getNodes($request));
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic GetNodes positive with policy argument
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Get information of nodes
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testGetNodesPositive($db)
    {
        if ($db->isConnected()) {
            try {
                return($db->getNodes(array(Aerospike::OPT_READ_TIMEOUT => 1500)));
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
?>
