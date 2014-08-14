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
    function testConnect($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $db->close();
        return ($db->errorno()); 
    }
    /**
     * @test
     * Connection parameter array
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCheckParameterArray)
     *
     * @test_plans{1.1}
     */
    function testCheckParameterArray($dummydb, $dummykey) {
        try {
	        $config = "";
            $db = new Aerospike($config);
        } catch (ErrorException $e) {
            return (self::$status_ok);
        }
        return (self::$status_ok);
    }

    /**
     * @test
     * Empty array for config
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
     * Missing Host Key
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
     * Missing Name Key
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testMissingNameKeyFromConfigArray)
     *
     * @test_plans{1.1}
     */
    function testMissingNameKeyFromConfigArray($dummydb, $dummykey) {
	    $config = array("hosts"=>array(array("0"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        return ($db->errorno()); 
    }
    /**
     * @test
     * Missing Port Key
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
        $db->close();
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
	    $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,"port"=>AEROSPIKE_CONFIG_PORT),
                  array("addr"=>AEROSPIKE_CONFIG_NAME,"port"=>AEROSPIKE_CONFIG_PORT2)));
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
	    $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>"3000")));
        $db = new Aerospike($config);
        return ($db->errorno()); 
    }
    /**
     * @test
     * Port and name sequence changed in array
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
    function testNamePortSequenceInConfig($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("port"=>AEROSPIKE_CONFIG_PORT,"addr"=>AEROSPIKE_CONFIG_NAME)));
        $db = new Aerospike($config, NULL, 
            array(Aerospike::OPT_WRITE_TIMEOUT =>1500));
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
        $port = 3000;
        for($i=0 ;$i<10;$i++,$port++)
        {
            $var = "\n172.20.25.".$i;
            $config = array("hosts"=>array(array("addr"=>$var,"port"=>$port)));
            $db = new Aerospike($config);
            $db->close();
        }
        if ($i == 10) {
            return (self::$status_ok);
        } else {
            return (self::$status_err);
        }
    }
    /**
     * @test
     * Alias not passed and different config with multiple ip
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
    
    function testNoAliasDifferentConfigCase2($dummydb, $dummykey) {
        $config1 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT),array("addr"=>AEROSPIKE_CONFIG_NAME3, "port"=>AEROSPIKE_CONFIG_PORT)));
        $config2 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME2,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config1);
        if (!$db->isConnected()) {
            $db->close();
            return (self::$status_err);
        }
        $db1 = new Aerospike($config2);
        if (!$db1->isConnected()) {
            $db->close();
            return (self::$status_err);
        }
        $db1->close();
        $db->close();
        return (self::$status_ok);
    }
    /**
     * @test
     * two same config with no alias
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
    function testTwoConfigWithNoAlias($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,"port"=>AEROSPIKE_CONFIG_PORT)));
        $config1 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,"port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        if (!$db->isConnected()) {
            $db->close();
            return (self::$status_err);
        }
        $db1 = new Aerospike($config1);
        if (!$db1->isConnected()) {
            $db1->close();
            return (self::$status_err);
        }
        $db1->close();
        $db->close();
        return (self::$status_ok);
    }
    /**
     * @test
     * Two same config multi host no alias
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

    function testTwoConfigMultihostNoAlias($dummydb, $dummykey) {
        $config1 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,"port"=>AEROSPIKE_CONFIG_PORT),
                   array("addr"=>AEROSPIKE_CONFIG_NAME2, "port"=>AEROSPIKE_CONFIG_PORT)));
        $config2 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,"port"=>AEROSPIKE_CONFIG_PORT),
                   array("addr"=>AEROSPIKE_CONFIG_NAME2, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config1);
        if (!$db->isConnected()) {
            $db->close();
            return (self::$status_err);
        }
        $db1 = new Aerospike($config2);
        if (!$db1->isConnected()) {
            $db1->close();
            return (self::$status_err);
        }
        $db1->close();
        $db->close();
        return (self::$status_ok);
    }
    /**
     * @test
     * two different config set with no alias passed in
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
    
    function testTwoConfigWithNoMatchNoAlias($dummydb, $dummykey) {
        $config1 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT),array("addr"=>AEROSPIKE_CONFIG_NAME3, "port"=>AEROSPIKE_CONFIG_PORT)));
        $config2 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME2,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config1);
        if (!$db->isConnected()) {
            $db->close();
            return (self::$status_err);
        }
        $db1 = new Aerospike($config2);
        if (!$db1->isConnected()) {
            $db1->close();
            return (self::$status_err);
        }
        $db1->close();
        $db->close();
        return (self::$status_ok);
    }
    /**
     * @test
     * Alias Empty string test
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
    
    function testWithAliasEmptyString($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $config1 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME2,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $config2 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config, "\n");
        if (!$db->isConnected()) {
            $db->close();
            return (self::$status_err);
        }
        $db1 = new Aerospike($config1, "");
        if (!$db1->isConnected()) {
            $db1->close();
            return (self::$status_err);
        }
        $db2 = new Aerospike($config2, "\n");
        if (!$db2->isConnected()) {
            $db2->close();
            return (self::$status_err);
        }
        $db2->close();
        $db1->close();
        $db->close();
        return (self::$status_ok);
    }
    /**
     * @test
     * check limit of list with alias
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

    function testCheckLimitList($dummydb, $dummykey) {
        for($i=0; $i<10; $i++)
        {
            $var = "\n172.20.25.".$i;
            $config = array("hosts"=>array(array("addr"=>$var,
            "port"=>4000)));
            $db = new Aerospike($config, $var);
            $db->close();
        }

        if ($i==10) {
            return (self::$status_ok);
        } else {
            return (self::$status_err);
        }
    }
    /**
     * @test
     * Smae config and same alias are passed
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

    function testSameConfigSameAlias($dummydb, $dummykey) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $config1 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config, "alias1");
        if (!$db->isConnected()) {
            $db->close();
            return (self::$status_err);
        }
        $db1 = new Aerospike($config1, "alias1");
        if (!$db1->isConnected()) {
            $db1->close();
            return (self::$status_ok);
        }
        $db1->close();
        $db->close();
        return (self::$status_ok);
    }
    /**
     * @test
     * Different config same alias 
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
    
    function testDifferentConfigSameAlias($dummydb, $dummykey){
        $config1 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT),array("addr"=>AEROSPIKE_CONFIG_NAME3, "port"=>AEROSPIKE_CONFIG_PORT)));
        $config2 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME2,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config1, "alias1");
        if (!$db->isConnected()) {
            $db->close();
            return (self::$status_err);
        }
        $db1 = new Aerospike($config2, "alias1");
        if (!$db1->isConnected()) {
            $db1->close();
            return (self::$status_err);
        }
        $db1->close();
        $db->close();
        return (self::$status_ok);
    }
    /**
     * @test
     * Both config and alais are different
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

    function testDiffrentConfigDifferentAlias($dummydb, $dummykey) {
        $config1 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT),array("addr"=>AEROSPIKE_CONFIG_NAME3, "port"=>AEROSPIKE_CONFIG_PORT)));
        $config2 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME2,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config1, "aliaseee1");
        if (!$db->isConnected()) {
            $db->close();
            return (self::$status_err);
        }
        $db1 = new Aerospike($config2, "alias2");
        if (!$db1->isConnected()) {
            $db1->close();
            return (self::$status_err);
        }
        $db1->close();
        $db->close();
        return (self::$status_ok);
    }
    /**
     * @test
     * Partial different in config with different alias
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

    function testDiffAliasPartiallyDiffConfig($dummydb, $dummykey) {
        $config1 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT),array("addr"=>AEROSPIKE_CONFIG_NAME3, "port"=>AEROSPIKE_CONFIG_PORT)));
        $config2 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME2,
            "port"=>AEROSPIKE_CONFIG_PORT), array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config1, "alias1");
        if (!$db->isConnected()) {
            $db->close();
            return (self::$status_err);
        }
        $db1 = new Aerospike($config2, "alias2");
        if (!$db1->isConnected()) {
            $db1->close();
            $db->close();
            return (self::$status_err);
        }
        $db->close();
        $db1->close();
        return (self::$status_ok);
    }
    /**
     * @test
     * Alias is passed as blank
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

    function testAliasPasssedasBlank($dummydb, $dummykey) {
        $config1 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT),array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $opts = array(Aerospike::OPT_CONNECT_TIMEOUT => 1250);
        $db = new Aerospike($config1, "alias", $opts);
        if (!$db->isConnected()) {
            $db->close();
            return (self::$status_err);
        }
        $db1 = new Aerospike($config1, "");
        if (!$db1->isConnected()) {
            $db1->close();
            return (self::$status_err);
        }
        $db1->close();
        $db->close();
        return (self::$status_ok);
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
        $db->close();
        return ($db->errorno());
    }
} 
?>
