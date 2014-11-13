<?php
class InfoMany extends AerospikeTestCommon
{
    protected function setUp(&$db, &$config) {
        $host = array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT);
        $config = array("hosts"=>array($host));
        $db = new Aerospike($config);
    }

    protected function tearDown(&$db, &$config) {
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
                $response = $db->infoMany('bins/test', $config, array(Aerospike::OPT_READ_TIMEOUT => 20000));
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
                $response = $db->infoMany('sadfsdfasdfasfdbcd', $config, array(Aerospike::OPT_READ_TIMEOUT => 20000));
                if ($response == NULL) {
                    return ($db->errorno());
                } else {
                    return (Aerospike::OK);
                }
            } catch (Exception $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic InfoMany negative empty command
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info for bins of namespace
     *
     * @remark Adjust after client bug is fixed
     *
     *
     * @test_plans{1.1}
     */
    function testInfoManyNegativeEmptyCommand($db, $config)
    {
        if ($db->isConnected()) {
            $response = $db->infoMany('', $config, array(Aerospike::OPT_READ_TIMEOUT => 20000));
            if ($response == NULL) {
                return ($db->errorno());
            } else {
                return (Aerospike::OK);
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
                $response = $db->infoMany('bins/test',
                    array("addr"=>"localhost", "port"=>AEROSPIKE_CONFIG_PORT),
                    array(Aerospike::OPT_READ_TIMEOUT => 20000));
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
        $myconfig = array("hosts"=>array($host));
        if ($db->isConnected()) {
            try {
                $response = $db->infoMany('bins/test', $myconfig,
                    array(Aerospike::OPT_READ_TIMEOUT => 20000));
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
                $response = $db->infoMany('bins/test', $config,
                    array(Aerospike::OPT_READ_TIMEOUT => 20000));
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
