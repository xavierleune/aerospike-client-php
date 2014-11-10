<?php
class QueryUsers extends AerospikeTestCommon
{
    protected function setUp(&$db, &$config) {
        $host = array("addr"=>AEROSPIKE_CONFIG_SECURE, "port"=>AEROSPIKE_CONFIG_SECURE_PORT);
        $config = array("hosts"=>array($host), "user"=>"admin", "pass"=>"admin");
        $db = new Aerospike($config);
        $res = $db->createUser("example", "example@123", array("read"));
    }

    protected function tearDown(&$db, &$config) {
        $res = $db->dropUser("example");
        $db->close();
    }
    /**
     * @test
     * Basic Query Users without parameter
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
    function testQueryUsersNoParameter($db)
    {
        if ($db->isConnected()) {
            try {
                return($db->queryUsers());
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
     * Basic Query Users positive
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
    function testQueryUsersPositive($db)
    {
        $test = array("read");
        if ($db->isConnected()) {
            sleep(2);
            $db->queryUsers($roles);
            $res = $roles['example'];
            $result = array_diff_assoc_recursive($test, $res);
            if(!empty($result)) {
                return Aerospike::ERR;
            } else {
                return Aerospike::OK;
            }
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Query Users positive with policy
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
    function testQueryUsersPositiveWithPolicy($db)
    {
        sleep(2);
        $test = array("read");
        if ($db->isConnected()) {
            $db->queryUsers($roles, array(Aerospike::OPT_WRITE_TIMEOUT => 2000));
            $result = array_diff_assoc_recursive($test, $roles['example']);
            if(!empty($result)) {
                return Aerospike::ERR;
            } else {
                return Aerospike::OK;
            }
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Query Users with incorrect policy
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
    function testQueryUsersIncorrectPolicy($db)
    {
        $test = array("read");
        if ($db->isConnected()) {
            $result = $db->queryUsers($roles, array(Aerospike::OPT_WRITE_TIMEOUT => 0.5));
            return $result;
            $result = array_diff_assoc_recursive($test, $roles);
            if(!empty($result)) {
                return Aerospike::ERR;
            } else {
                return Aerospike::OK;
            }
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Query Users roles is null
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
    function testQueryUsersRolesIsNull($db)
    {
        sleep(2);
        $test = array("read");
        $roles = NULL;
        if ($db->isConnected()) {
            $result = $db->queryUsers($roles);
            if ($db->errorno() != 0) {
                return $db->errorno();
            } else {
                $result = array_diff_assoc_recursive($test, $roles['example']);
                if(!empty($result)) {
                    return Aerospike::ERR;
                } else {
                    return Aerospike::OK;
                }
            }
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Query Users with policy as string
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
    function testQueryUsersStringPolicy($db)
    {
        $test = array("read");
        if ($db->isConnected()) {
            try {
                return($db->queryUser("example", $roles, array("OPT_READ_TIMEOUT" => "incorrect")));	
            } catch (ErrorExcpetion $e) {
                return $db->errorno();
            }
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Query Users with extra parameter
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
    function testQueryUsersExtraParameter($db)
    {
        $test = array("read");
        if ($db->isConnected()) {
            try {
                $db->queryUsers($roles, array("OPT_READ_TIMEOUT" => 1000), "");
                $result = array_diff_assoc_recursive($test, $roles);
                if(!empty($result)) {
                    return Aerospike::ERR;
                } else {
                    return Aerospike::OK;
                }
            } catch(ErrorException $e) {
                return $db->errorno();
            }
        }
        else {
            return($db->errorno());
        }
    }
}
