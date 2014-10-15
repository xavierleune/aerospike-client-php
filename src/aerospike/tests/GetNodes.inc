<?php
class GetNodes extends AerospikeTestCommon
{
    protected function setUp(&$db, &$host) {
        $host = array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT);
        $config = array("hosts"=>array($host));
        $db = new Aerospike($config);
    }

    protected function tearDown(&$db, &$host) {
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
                if ($db->getNodes()) {
                    return Aerospike::OK;
                } else {
                    return Aerospike::ERR_CLIENT;
                }
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
                if ($db->getNodes($request)) {
                    return Aerospike::OK;
                } else {
                    return Aerospike::ERR_CLIENT;
                }
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
}
?>
