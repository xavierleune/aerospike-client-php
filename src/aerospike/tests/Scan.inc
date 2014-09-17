<?php

require_once("Common.inc");

class Scan extends AerospikeTestCommon
{
    /*
     * TODO: Remove these constants and map with actual Aerospike class
     * constants.
     */
    static $status_ok = 2000;
    static $status_err = 2001;
    static $status_get_record_fail = 2002;

    protected function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $ns = "test";
        $set = "demo";
        $key = $db->initKey("test", "demo", "Get_key");
        $db->put($key, array("email"=>"raunak"));
        $key = $db->initKey("test", "demo", "Get_key1");
        $db->put($key, array("email"=>"pavan"));
    }

    protected function tearDown(&$db, &$key) {
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
     * Basic SCAN without parameter
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGETNoParameter)
     *
     * @test_plans{1.1}
     */
    function testScanNoParameter($db)
    {
        if ($db->isConnected()) {
            try {
                $db->scan();
            } catch (ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic SCAN Check set pnd ns parameter value non exist in database
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCheckSetValueNotExistInDB)
     *
     * @test_plans{1.1}
     */
    function testCheckSetAndNsValueNotExistInDB($db)
    {
        $processed = 0;
        if ($db->isConnected()) {
            return($db->scan("r1", "rr", function ($record) {
                if (array_key_exists('email', $record) && !is_null($record['email']))
                    echo $record['email']."\n";
                if ($processed++ > 19) return false;
            }, array("email")));
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic SCAN Check set pnd ns parameter value empty string
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCheckSetValueNotExistInDB)
     *
     * @test_plans{1.1}
     */
    function testCheckSetAndNsValueNull($db)
    {
        $processed = 0;
        if ($db->isConnected()) {
            try {
                return($db->scan("", "", function ($record) {
                if (array_key_exists('email', $record) && !is_null($record['email']))
                    echo $record['email']."\n";
                if ($processed++ > 19) return false;
                }, array("email")));
            } catch(Exception $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic SCAN Check incorrect name of bins
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCheckSetValueNotExistInDB)
     *
     * @test_plans{1.1}
     */
    function testCheckIncorrectNameOfBins($db)
    {
        $processed = 0;
        if ($db->isConnected()) {
            return($db->scan("test", "demo", function ($record) {
                if (array_key_exists('account', $record) && !is_null($record['account']))
                    echo $record['email']."\n";
                if ($processed++ > 19) return false;
            }, array("account")));
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic SCAN Check wrong bin name in callback
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCheckSetValueNotExistInDB)
     *
     * @test_plans{1.1}
     */
    function testCheckIncorrectBinNameInCallback($db)
    {
        $processed = 0;
        if ($db->isConnected()) {
            return($db->scan("test", "demo", function ($record) {
                if (array_key_exists('account', $record) && !is_null($record['account']))
                    echo $record['email']."\n";
                global $processed;
                if ($processed++ > 19) return false;
            }, array("email")));
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic SCAN Check for empty bin value
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCheckSetValueNotExistInDB)
     *
     * @test_plans{1.1}
     */
    function testCheckForEmptyBinValue($db, $key)
    {
        $processed = 0;
        if ($db->isConnected()) {
            try {
                $status =  $db->scan("test", "demo", function ($record) {
                    if (array_key_exists('email', $record) && !is_null($record['email']))
                        echo $record['email']."\n";
                    if ($processed++ > 19) return false;
                }, "");
            } catch (exception $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic SCAN Check incorrect value for options
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCheckSetValueNotExistInDB)
     *
     * @test_plans{1.1}
     */
    function testCheckIncorrectValueForOptions($db)
    {
        if ($db->isConnected()) {
            return($db->scan( "test", "demo", function ($record) {
                global $processed;
                if (array_key_exists('account', $record) && !is_null($record['account']))
                    echo $record['email']."\n";
                if ($processed++ > 19) return false;
            }, array("email"), array(Aerospike::OPT_SCAN_NOBINS=>false, Aerospike::OPT_READ_TIMEOUT=>"pqr")));
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic SCAN Check for callback parameter missing
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCheckSetValueNotExistInDB)
     *
     * @test_plans{1.1}
     */
    function testCheckForCallbackParameterMissing($db)
    {
        $processed = 0;
        if ($db->isConnected()) {
            try {
                $db->scan( "test", "demo", array("email"));
            } catch(ErrorException $e) {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic SCAN Check with correct arguments
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCheckSetValueNotExistInDB)
     *
     * @test_plans{1.1}
     */
    function testCheckForCorrectArguments($db)
    {
        $processed = 0;
        $mystatus = 0;
        if ($db->isConnected()) {
            $status = $db->scan( "test", "demo", function ($record) {
                global $processed;
                if ($processed < 2) {
                    if (array_key_exists('email', $record) && !is_null($record['email'])) {
                        if ((!strcmp($record['email'], "pavan")) ||
                            (!strcmp($record['email'], "raunak"))) {
                            $processed++;
                            }
                        }
                    }
                if ($processed == 2) {
                    return Aerospike::OK;
                }
            }, array("email"), array(Aerospike::OPT_READ_TIMEOUT=>2000));
            if ($status != AEROSPIKE::OK) {
                echo $db->errorno();
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * Scan - positive with all arguments
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAggregatePositive)
     *
     * @test_plans{1.1}
     */
    function testScanPositiveWithAllArguments($db)
    {
        $processed = 0;
        $mystatus = 0;
        if ($db->isConnected()) {
            $status = $db->scan( "test", "demo", function ($record) {
                global $processed;
                if ($processed < 2) {
                    if (array_key_exists('email', $record) && !is_null($record['email'])) {
                        if ((!strcmp($record['email'], "pavan")) ||
                            (!strcmp($record['email'], "raunak"))) {
                            $processed++;
                            }
                        }
                    }
                if ($processed == 2) {
                    return Aerospike::OK;
                }
            }, array("email"), array(Aerospike::OPT_READ_TIMEOUT=>2000, Aerospike::OPT_SCAN_CONCURRENTLY=>true));
            if ($status != AEROSPIKE::OK) {
                echo $db->errorno();
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * Basic SCAN Check without optional arguments
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCheckSetValueNotExistInDB)
     *
     * @test_plans{1.1}
     */
    function testCheckForCorrectArgumentsWithoutOptionals($db)
    {
        $processed = 0;
        $mystatus = 0;
        if ($db->isConnected()) {
            $status = $db->scan( "test", "demo", function ($record) {
                global $processed;
                if ($processed < 2) {
                    if (array_key_exists('email', $record) && !is_null($record['email'])) {
                        if ((!strcmp($record['email'], "pavan")) ||
                            (!strcmp($record['email'], "raunak"))) {
                            $processed++;
                            }
                        }
                    }
                if ($processed == 2) {
                    return Aerospike::OK;
                }
            });
            if ($status != AEROSPIKE::OK) {
                echo $db->errorno();
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * Basic SCAN Check with correct arguments and one optional argument
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCheckSetValueNotExistInDB)
     *
     * @test_plans{1.1}
     */
    function testCheckForOneOptionalArgument($db)
    {
        $processed = 0;
        $mystatus = 0;
        if ($db->isConnected()) {
            $status = $db->scan( "test", "demo", function ($record) {
                global $processed;
                if ($processed < 2) {
                    if (array_key_exists('email', $record) && !is_null($record['email'])) {
                        if ((!strcmp($record['email'], "pavan")) ||
                            (!strcmp($record['email'], "raunak"))) {
                            $processed++;
                            }
                        }
                    }
                if ($processed == 2) {
                    return Aerospike::OK;
                }
            }, array("email"));
            if ($status != AEROSPIKE::OK) {
                echo $db->errorno();
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
        return($status);
    }
    /**
     * @test
     * Basic SCAN Check with correct arguments and empty callback function
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCheckSetValueNotExistInDB)
     *
     * @test_plans{1.1}
     */
    function testCheckForEmptyCallbackFunction($db)
    {
        $processed = 0;
        if ($db->isConnected()) {
            $status = $db->scan("test", "demo", function ($record) {
            },array("email"), array(Aerospike::OPT_SCAN_PRIORITY=>Aerospike::SCAN_PRIORITY_AUTO,
                Aerospike::OPT_READ_TIMEOUT=>2000));
            if ($status != AEROSPIKE::OK) {
                return($db->errorno());
            }
            return($status);
        }
    }
}
?>
