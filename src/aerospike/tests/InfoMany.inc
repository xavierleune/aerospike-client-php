<?php
class InfoMany extends AerospikeTestCommon
{
    /*
     * TODO: Remove these constants and map with actual Aerospike class
     * constants.
     */
    static $status_ok = 2000;
    static $status_err = 2001;
    static $status_get_record_fail = 2002;

    protected function setUp(&$db, &$config) {
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

    protected function tearDown(&$db, &$config) {
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
     * Basic INFOMANY without parameter
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Error
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoManyNoParameter($db)
    {
        if ($db->isConnected()) {
            try {
                return($db->infoMany());
            } catch(ErrorException $e) {
                return($db->errorno());    
            }
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic InfoMany positive for bins
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info for bins of namespace
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoManyPositiveForBins($db, $config)
    {
        if ($db->isConnected()) {
            try {
                $response = $db->infoMany('bins/test');
                if ($response == NULL)
                    return ($db->errorno()); 
                else
                    return (Aerospike::OK);
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic InfoMany positive for bins with config array
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info for bins of namespace
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoManyPositiveForBinsWithConfig($db, $config)
    {
        if ($db->isConnected()) {
            try {
                $response = $db->infoMany('bins/test', $config);
                if ($response == NULL)
                    return ($db->errorno()); 
                else
                    return (Aerospike::OK);
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic InfoMany positive for bins with multiple hosts
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info for bins of namespace
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoManyPositiveForBinsWithMultipleHosts($db, $config)
    {
        $host = array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT);
        $hostnew = array("addr"=>AEROSPIKE_CONFIG_NAME2, "port"=>AEROSPIKE_CONFIG_PORT2);
        $config = array("hosts"=>array($host, $hostnew));
        if ($db->isConnected()) {
            try {
                $response = $db->infoMany('bins/test', $config);
                if ($response == NULL)
                    return ($db->errorno()); 
                else
                    return (Aerospike::OK);
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic InfoMany positive for bins with config array and policy argument
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info for bins of namespace
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoManyPositiveForBinsWithPolicy($db, $config)
    {
        if ($db->isConnected()) {
            try {
                $response = $db->infoMany('bins/test', $config, array(Aerospike::OPT_READ_TIMEOUT => 2000));
                if ($response == NULL)
                    return ($db->errorno); 
                else
                    return (Aerospike::OK);
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic InfoMany negative incorrect command
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info for bins of namespace
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoManyNegativeIncorrectCommand($db, $config)
    {
        if ($db->isConnected()) {
            try {
                $response = $db->infoMany('abcd', $config);
                if ($response == NULL)
                    return ($db->errorno()); 
                else
                    return (Aerospike::OK);
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic InfoMany negative incorrect config structure
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info for bins of namespace
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoManyNegativeIncorrectConfigStructure($db, $config)
    {
        if ($db->isConnected()) {
            try {
                $response = $db->infoMany('bins/test', array("addr"=>"localhost", "port"=>AEROSPIKE_CONFIG_PORT));
                if ($response == NULL)
                    return ($db->errorno()) ;
                else
                    return (Aerospike::OK);
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic InfoMany negative incorrect hostname
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info for bins of namespace
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoManyNegativeIncorrectHostname($db, $config)
    {
        $host = array("addr"=>"localhost2", "port"=>AEROSPIKE_CONFIG_PORT);
        $config = array("hosts"=>array($host));
        if ($db->isConnected()) {
            try {
                $response = $db->infoMany('bins/test', $config);
                if ($response == NULL) {
                    return ($db->errorno());
                }
                else {
                    return (Aerospike::OK);
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
     * Basic InfoMany negative incorrect port
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info for bins of namespace
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoManyNegativeIncorrectPort($db, $config)
    {
        $host = array("addr"=>"localhost", "port"=>3002);
        $config = array("hosts"=>array($host));
        if ($db->isConnected()) {
            try {
                $response = $db->infoMany('bins/test', $config);
                if ($response == NULL) {
                    return ($db->errorno());
                }
                else {
                    return (Aerospike::OK);
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
     * Basic InfoMany positive for build
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info for bins of namespace
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoManyPositiveForBuild($db, $config)
    {
        if ($db->isConnected()) {
            try {
                $response = $db->infoMany('build');
                if ($response == NULL)
                    return ($db->errorno()); 
                else
                    return (Aerospike::OK);
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
}
?>
