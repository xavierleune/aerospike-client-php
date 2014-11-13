<?php
class Info extends AerospikeTestCommon
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
     * Basic INFO without parameter
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     *
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoNoParameter($db)
    {
        if ($db->isConnected()) {
            try {
                return($db->info());
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
     * Basic Info positive for bins
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
    function testInfoPositiveForBins($db, $host)
    {
        if ($db->isConnected()) {
            $status = $db->info('bins/test', $response, $host);
        } else {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * Basic Info positive for build
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info for build
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoPositiveForBuild($db, $host)
    {
        if ($db->isConnected()) {
            try {
                return($db->info('build', $response, $host));
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Info positive for config with response an array
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info for config
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoPositiveForConfig($db, $host)
    {
        $response = array();
        if ($db->isConnected()) {
            try {
                return($db->info('get-config:context=namespace;id=test', $response, $host));
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Info positive for histogram
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info for histogram
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoPositiveForHistogram($db, $host)
    {
        if ($db->isConnected()) {
            try {
                return($db->info('hist-dump:ns=test;hist=ttl', $response, $host));
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Info positive for jobs
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info for all jobs
     *
     * @remark
     * 
     *
     * @test_plans{1.1}
     */
    function testInfoPositiveForJobs($db, $host)
    {
        if ($db->isConnected()) {
            try {
                return($db->info('jobs:', $response, $host));
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Info positive for jobs with module scan and response is string
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info for scan job
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoPositiveForJobsScan($db, $host)
    {
        $response = "";
        if ($db->isConnected()) {
            try {
                return($db->info('jobs:module=scan', $response, $host));
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Info positive for latency
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * info for latency
     *
     * @remark
     * 
     *
     * @test_plans{1.1}
     */
    function testInfoPositiveForLatency($db, $host)
    {
        if ($db->isConnected()) {
            try {
                return($db->info('latency:hist=reads;slice=30;back=300;duration:120', $response, $host));
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Info positive for log and response is null
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * info for particular log
     *
     * @remark
     * 
     *
     * @test_plans{1.1}
     */
    function testInfoPositiveForLog($db, $host)
    {
        $response = NULL;
        if ($db->isConnected()) {
            try {
                return($db->info('log/0', $response, $host));
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Info positive for all logs
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * info for all log files
     *
     * @remark
     * 
     *
     * @test_plans{1.1}
     */
    function testInfoPositiveForAllLogs($db, $host)
    {
        if ($db->isConnected()) {
            try {
                return($db->info('logs', $response, $host));
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Info positive for mcast
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * info for mcast
     *
     * @remark
     * 
     *
     * @test_plans{1.1}
     */
    function testInfoPositiveForMcast($db, $host)
    {
        if ($db->isConnected()) {
            try {
                return($db->info('mcast', $response, $host));
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Info positive for mesh
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * info for mesh
     *
     * @remark
     * 
     *
     * @test_plans{1.1}
     */
    function testInfoPositiveForMesh($db, $host)
    {
        if ($db->isConnected()) {
            try {
                return($db->info('mesh', $response, $host));
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Info positive for namespaces
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Information about a particular namespace
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoPositiveForNamespace($db, $host)
    {
        if ($db->isConnected()) {
            try {
                return($db->info('namespace/test', $response, $host));
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Info positive for nodes
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info about all nodes in system
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoPositiveForNode($db, $host)
    {
        if ($db->isConnected()) {
            try {
                return($db->info('node', $response, $host));
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Info negative with incorrect host name
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
    function testInfoNegativeForIncorrectHost($db, $host)
    {
        $host = array("addr"=>"localhost2", "port"=>3000);
        if ($db->isConnected()) {
            try {
                return($db->info('node', $response, $host));
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Info negative with incorrect command
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
    function testInfoNegativeForIncorrectCommand($db, $host)
    {
        if ($db->isConnected()) {
            try {
                $status = $db->info('abcd', $response, $host);
                if($status == Aerospike::OK) {
                    if($response==NULL) {
                        return Aerospike::OK;
                    }
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
     * Basic Info negative with empty command
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
    function testInfoNegativeForEmptyCommand($db, $host)
    {
        if ($db->isConnected()) {
            $status = $db->info('', $response, $host);
            return $status;
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Info negative with incorrect port no
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
    function testInfoNegativeForIncorrectPort($db, $host)
    {
        $host = array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>3001);
        if ($db->isConnected()) {
            try {
                return($db->info('node', $response, $host));
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Info positive with 2 arguments
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
    function testInfoPositiveForTwoArguments($db, $host)
    {
        if ($db->isConnected()) {
            try {
                return($db->info('build', $response));
                }  catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Info positive with policy argument
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * Info about build
     *
     * @remark
     *
     *
     * @test_plans{1.1}
     */
    function testInfoPositiveForPolicyArgument($db, $host)
    {
        if ($db->isConnected()) {
            try {
                return($db->info('build', $response, $host, array(Aerospike::OPT_READ_TIMEOUT => 2000)));
                }
            catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
}
?>
