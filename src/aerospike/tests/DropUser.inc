<?php
class DropUser extends AerospikeTestCommon
{
    protected function setUp(&$db, &$config) {
        $host = array("addr"=>AEROSPIKE_CONFIG_SECURE, "port"=>AEROSPIKE_CONFIG_SECURE_PORT);
        $config = array("hosts"=>array($host), "user"=>"admin", "pass"=>"admin");
        $db = new Aerospike($config);
    }

    protected function tearDown(&$db, &$config) {
        $db->dropUser("examplephp");
        $db->close();
    }
    /**
     * @test
     * Basic DropUser without parameter
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
    function testDropUserNoParameter($db)
    {
        if ($db->isConnected()) {
            try {
                return($db->dropUser());
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
     * Basic DropUser with user NULL
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
    function testDropUserUserIsNull($db)
    {
        $user = NULL;
        if ($db->isConnected()) {
            return ($db->dropUser($user));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic DropUser positive
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
    function testDropUserUserPositive($db)
    {
        $user = "examplephp";
        if ($db->isConnected()) {
            $db->createUser($user, "example123", array("read"));
            sleep(2);
            $db->dropUser($user);
            sleep(2);
            $db->queryUser($user, $roles);
            if(empty($roles))
                return Aerospike::OK;
            else
                return Aerospike::ERR;
            return($db->errorno());
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic DropUser with non-existent user
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
    function testDropUserNonExistentUser($db)
    {
        $user = "abcdefgh";
        if ($db->isConnected()) {
            try {
                return($db->dropUser($user));
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
     * Basic DropUser positive with policy
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
    function testDropUserUserPositiveWithPolicy($db)
    {
        $user = "examplephp";
        if ($db->isConnected()) {
            $db->createUser($user, "example123", array("read"));
            sleep(2);
            $db->dropUser($user, array(Aerospike::OPT_WRITE_TIMEOUT => 2000));
            sleep(2);
            $db->queryUser($user, $roles);
            if(empty($roles))
                return Aerospike::OK;
            else
                return Aerospike::ERR;
            return($db->errorno());
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic DropUser positive with policy
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
    function testDropUserUserPositiveWithIncorrectPolicy($db)
    {
        $user = "examplephp";
        if ($db->isConnected()) {
            $db->createUser($user, "example123", array("read"));
            sleep(2);
            return ($db->dropUser($user, array(Aerospike::OPT_WRITE_TIMEOUT => 0.5)));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic DropUser negative with extra argument
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
    function testDropUserUserExtraArgument($db)
    {
        $user = "examplephp";
        if ($db->isConnected()) {
            $db->createUser($user, "example123", array("read"));
            sleep(2);
            try {
                $db->dropUser($user, array(Aerospike::OPT_WRITE_TIMEOUT => 2000), "");
            } catch (ErrorException $e) {
                return $db->errorno();
            }
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic DropUser negative with policy null
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
    function testDropUserPolicyNull($db)
    {
        $user = "examplephp";
        if ($db->isConnected()) {
            $db->createUser($user, "example123", array("read"));
            sleep(2);
            try {
                $db->dropUser($user, NULL);
            } catch(ErrorException $e) {
                return $db->errorno();
            }
        }
        else {
            return($db->errorno());
        }
    }
}
?>
