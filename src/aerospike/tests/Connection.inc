<?php

require_once 'Common.inc';
/**
 *Basic connection tests
*/

class Connection extends AerospikeTestCommon
{
    static $status_ok = 0;
    static $status_err = 201;

    /**
     * @test
     * Basic Connection Test
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testConnect)
     *
     * @test_plans{1.1}
     */
    function testConnect($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $db->close();
        return ($db->errorno());
    }
    /**
     * @test
     * Config Argument Not An Array
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testConfigNotArray)
     *
     * @test_plans{1.1}
     */
    function testConfigNotArray($dummydb, $dummykey) {
        try {
            $config = "";
            $db = new Aerospike($config);
            return($db->errorno());
        } catch (ErrorException $e) {
            return self::$status_err;
        }
    }
    /**
     * @test
     * Config Argument Is An Empty Array
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testEmptyArray)
     *
     * @test_plans{1.1}
     */
    function testEmptyArray($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("addr"=>"", "port"=>"")));
        $db = new Aerospike($config);
        return($db->errorno());
    }
    /**
     * @test
     * Config Array Missing The Host Key
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testMissingHostKeyFromConfigArray)
     *
     * @test_plans{1.1}
     */
    function testMissingHostsKeyFromConfigArray($dummydb, $dummykey) {
        $config = array(array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        return ($db->errorno());
    }
    /**
     * @test
     * Config Array Missing Addr Key
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testMissingAddrKeyFromConfigArray)
     *
     * @test_plans{1.1}
     */
    function testMissingAddrKeyFromConfigArray($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("0"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        return ($db->errorno());
    }
    /**
     * @test
     * Config Array Missing Port Key
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testMissingPortKeyFromConfigArray)
     *
     * @test_plans{1.1}
     */
    function testMissingPortKeyFromConfigArray($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "qw"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        return ($db->errorno());
    }
    /**
     * @test
     * Multiple Hosts
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testMultipleHostsArray)
     *
     * @test_plans{1.1}
     */
    function testMultipleHostsArray($dummydb, $dummykey) {
        $config = array("hosts"=>array(
                    array("addr"=>AEROSPIKE_CONFIG_NAME,"port"=>AEROSPIKE_CONFIG_PORT),
                    array("addr"=>AEROSPIKE_CONFIG_NAME2,"port"=>AEROSPIKE_CONFIG_PORT2)));
        $db = new Aerospike($config);
        $db->close();
        return ($db->errorno());
    }
    /**
     * @test
     * Port value is string
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPortValueIsString)
     *
     * @test_plans{1.1}
     */
    function testPortValueIsString($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>"3000")));
        $db = new Aerospike($config);
        return ($db->errorno());
    }
    /**
     * @test
     * Test With Null Persistence Alias
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testNullAlias)
     *
     * @test_plans{1.1}
     */
    function testNullAlias($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("port"=>AEROSPIKE_CONFIG_PORT,"addr"=>AEROSPIKE_CONFIG_NAME)));
        $db = new Aerospike($config, NULL);
        $db->close();
        return ($db->errorno());
    }
    /**
     * @test
     * Alias not passed and different config
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testNamePortSequenceInConfig)
     *
     * @test_plans{1.1}
     */
    function testNoAliasDiffrentConfig($dummydb, $dummykey) {
        $test1 = false;
        $test2 = false;
        $config1 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,"port"=>AEROSPIKE_CONFIG_PORT)));
        $db1 = new Aerospike($config1);
        $config2 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME2,"port"=>AEROSPIKE_CONFIG_PORT2)));
        $db2 = new Aerospike($config2);
        if ($db1->isConnected()) {
            $test1 = true;
            $db1->close();
        }
        if ($db2->isConnected()) {
            $test2 = true;
            $db2->close();
        }
        if ($test1 && $test2) {
            return (self::$status_ok);
        } elseif (!$test1) {
            return $db1->errorno();
        } else {
            return $db2->errorno();
        }
    }


    /**
     * @test
     * Basic Connection
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testConnect)
     *
     * @test_plans{1.1}
     */
    function testConnectWithOption($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $opts = array(Aerospike::OPT_CONNECT_TIMEOUT => 1250,
            Aerospike::OPT_WRITE_TIMEOUT => 1500);
        $db = new Aerospike($config, NULL, $opts);
        $status = $db->errorno();
        $db->close();
        return ($status);
    }

    function testConnectWithPersistFlagFalse($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config, false);
        $status = $db->errorno();
        $db->close();
        return ($status);
    }

    function testConnectWithPersistFlagPassedAsStr($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config, "persist");
        $status = $db->errorno();
        $db->close();
        return ($status);
    }

    function testConnectWithPersistFlagPassedAsInt($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config, 11111222);
        $status = $db->errorno();
        $db->close();
        return ($status);
    }

    function testConnectWithPersistFlagTrue($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config, true);
        $status = $db->errorno();
        $db->close();
        return ($status);
    }

    function testConnectWithSameConfigPersistAsTrue($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $config1 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config, true);
        if (!$db->isConnected()) {
            $db->close();
            return (self::$status_err);
        }
        $db1 = new Aerospike($config1, true);
        if (!$db1->isConnected()) {
            $db1->close();
            return (self::$status_ok);
        }
        $db1->close();
        $db->close();
        return (self::$status_ok);
        
    }
} 
?>
