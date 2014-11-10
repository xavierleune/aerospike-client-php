<?php
class CreateUser extends AerospikeTestCommon
{
    protected function setUp(&$db, &$config) {
        $host = array("addr"=>AEROSPIKE_CONFIG_SECURE, "port"=>AEROSPIKE_CONFIG_SECURE_PORT);
        $config = array("hosts"=>array($host), "user"=>"admin", "pass"=>"admin");
        $db = new Aerospike($config);
    }

    protected function tearDown(&$db, &$config) {
        $db->dropUser("createuser-example");
        $db->dropUser("createuser2");
        $db->close();
    }
    /**
     * @test
     * Basic CreateUser without parameter
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
    function testCreateUserNoParameter($db)
    {
        if ($db->isConnected()) {
            try {
                return($db->createUser());
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
     * Basic CreateUser positive
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
    function testCreateUserPositive($db)
    {
        if ($db->isConnected()) {
            $test = array("read", "read-write");
            $db->createUser("createuser-example", "example123", array("read", "read-write"));
            sleep(2);
            $db->queryUser("createuser-example", $roles);
            $db->dropUser("createuser-example");
            $result = array_diff_assoc_recursive($test, $roles);
            if(empty($result)) 
                return Aerospike::OK;
            else
                return Aerospike::ERR;
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic CreateUser positive with policy
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
    function testCreateUserPositivePolicy($db)
    {
        if ($db->isConnected()) {
            $test = array("read", "read-write");
            $db->createUser("createuser-example", "example123", array("read", "read-write"), array(Aerospike::OPT_WRITE_TIMEOUT => 2000));
            sleep(2);
            $db->queryUser("createuser-example", $roles);
            $db->dropUser("createuser-example");
            $result = array_diff_assoc_recursive($test, $roles);
            if(empty($result)) 
                return Aerospike::OK;
            else
                return Aerospike::ERR;
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic CreateUser positive with policy incorrect
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
    function testCreateUserPolicyIncorrect($db)
    {
        if ($db->isConnected()) {
            $test = array("read", "read-write");
            return($db->createUser("createuser-example", "example123", array("read", "read-write"), array(Aerospike::OPT_WRITE_TIMEOUT => 0.5)));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic CreateUser with password as NULL
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
    function testCreateUserPasswordNull($db)
    {
        $pass = NULL;
        if ($db->isConnected()) {
            return ($db->createUser("createuser-example", $pass, array("read", "read-write")));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic CreateUser with username as NULL
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
    function testCreateUserNull($db)
    {
        $user = NULL;
        if ($db->isConnected()) {
            return ($db->createUser($user, "example123", array("read", "read-write")));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic CreateUser with roles as NULL
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
    function testCreateUserRolesNull($db)
    {
        $roles = NULL;
        if ($db->isConnected()) {
            try {
                return ($db->createUser("createuser-example", "example123", $roles));
            } catch(ErrorException $e) {
                return $db->errorno();
            }
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic CreateUser with roles as empty
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
    function testCreateUserRolesEmpty($db)
    {
        if ($db->isConnected()) {
            return $db->createUser("createuser-example", "example123", array());
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic CreateUser already existing user
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
    function testCreateUserExisting($db)
    {
        if ($db->isConnected()) {
            return ($db->createUser("john", "example123", array("read", "read-write")));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic CreateUser already invalid role
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
    function testCreateUserInvalidRole($db)
    {
        if ($db->isConnected()) {
            return ($db->createUser("createuser-example", "example123", array("invalid")));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic CreateUser non-held role
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
    function testCreateUserUnderPrivileged($db)    
    {
        if ($db->isConnected()) {
            $db->createUser("createuser-example", "example123", array("read"));
            sleep(2);
            $key = $db->initKey("test", "demo", 1);
            return ($db->put($key, array("age" => 1)));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic CreateUser multiple users with same password
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
    function testCreateUserMultiple($db)
    {
        if ($db->isConnected()) {
            $test = array("read", "read-write");
            $db->createUser("createuser-example", "example123", array("read", "read-write"));
            sleep(2);
            $db->queryUser("createuser-example", $roles);
            $result = $db->createUser("createuser2", "example123", array("read", "read-write"));
            $db->dropUser("createuser2");
            $db->dropUser("createuser-example");
            return $result;
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic CreateUser with very long username
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
    function testCreateUserLongUsername($db)
    {
        if ($db->isConnected()) {
            $user = "user";
            for ($i = 0; $i <1000; $i++) {
                $user = $user + "user";
            }
            $test = array("read", "read-write");
            $db->createUser($user, "example123", array("read", "read-write"));
            if ($db->errorno() != 0) {
                sleep(2);
                $db->queryUser($user, $roles);
                if ($db->errorno() != 0) {
                    $db->dropUser($user);
                    $result = array_diff_assoc_recursive($test, $roles);
                    if(empty($result)) 
                        return Aerospike::OK;
                    else
                        return Aerospike::ERR;
                }
            }
        }
        else {
            return($db->errorno());
        }
        return $db->errorno();
    }
    /**
     * @test
     * Basic CreateUser with very special characters in username
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
    function testCreateUserSpecialCharacterUsername($db)
    {
        if ($db->isConnected()) {
            $user = "!#Q#AEQ@#$%&^*((^&*~~~````";
            $test = array("read", "read-write");
            $db->createUser($user, "example123", array("read", "read-write"));
            if ($db->errorno() != 0) {
                sleep(2);
                $db->queryUser($user, $roles);
                if ($db->errorno() != 0) {
                    $db->dropUser($user);
                    $result = array_diff_assoc_recursive($test, $roles);
                    if(empty($result)) 
                        return Aerospike::OK;
                    else
                        return Aerospike::ERR;
                }
            }
        }
        else {
            return($db->errorno());
        }
        return $db->errorno();
    }
}
?>
