<?php
class QueryUser extends AerospikeTestCommon
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
     * Basic Query User without parameter
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
    function testQueryUserNoParameter($db)
    {
        if ($db->isConnected()) {
            try {
                return($db->queryUser());
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
     * Basic Query User positive
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
    function testQueryUserPositive($db)
    {
        $test = array("read");
        if ($db->isConnected()) {
            sleep(2);
            $db->queryUser("example", $roles);	
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
     * Basic Query User positive with policy
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
    function testQueryUserPositiveWithPolicy($db)
    {
        $test = array("read");
        if ($db->isConnected()) {
            sleep(2);
            $db->queryUser("example", $roles, array(Aerospike::OPT_WRITE_TIMEOUT => 2000));
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
     * Basic Query User with incorrect policy
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
    function testQueryUserIncorrectPolicy($db)
    {
        $test = array("read");
        if ($db->isConnected()) {
            $result = $db->queryUser("example", $roles, array("OPT_WRITE_TIMEOUT" => 0.5));	
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
     * Basic Query User username is null
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
    function testQueryUserUserIsNull($db)
    {
        $test = array("read");
        if ($db->isConnected()) {
            $db->queryUser(NULL, $roles);	
            if ($db->errorno() != 0) {
                return $db->errorno();
            } else {
                $result = array_diff_assoc_recursive($test, $roles);
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
     * Basic Query User non existent user
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
    function testQueryUserNonexistentUser($db)
    {
        $test = array("read");
        if ($db->isConnected()) {
            $db->queryUser("abcasfdgasddas", $roles);
            if ($db->errorno() != 0) {
                return $db->errorno();
            }
            return Aerospike::OK;
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic Query User with no roles
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
    function testQueryUserNoRoles($db)
    {
        $test = array();
        if ($db->isConnected()) {
            $db->revokeRoles("example", array("read"));
            sleep(2);
            $db->queryUser("example", $roles);	
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
     * Basic Query User with policy as string
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
    function testQueryUserStringPolicy($db)
    {
        $test = array("read");
        if ($db->isConnected()) {
            try {
                $db->queryUser("example", $roles, array("OPT_READ_TIMEOUT" => "incorrect"));	
                $result = array_diff_assoc_recursive($test, $roles);
                if(!empty($result)) {
                    return Aerospike::ERR;
                } else {
                    return Aerospike::OK;
                }
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
     * Basic Query User with extra parameter
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
    function testQueryUsesExtraParameter($db)
    {
        $test = array("read");
        if ($db->isConnected()) {
            try {
                $db->queryUser("example", $roles, array("OPT_READ_TIMEOUT" => 1000), "");
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
