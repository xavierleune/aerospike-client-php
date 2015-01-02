<?php
class CreateUser extends AerospikeTestCommon
{
    protected function setUp() {
        $host = array("addr"=>AEROSPIKE_CONFIG_SECURE, "port"=>AEROSPIKE_CONFIG_SECURE_PORT);
        $config = array("hosts"=>array($host), "user"=>"admin", "pass"=>"admin");
        $this->db = new Aerospike($config);
        if (!$this->db->isConnected()) {
            return $this->db->errorno();
        }
    }

    protected function tearDown() {
        $this->db->dropUser("createuser-example");
        $this->db->dropUser("createuser2");
        $this->db->close();
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
    function testCreateUserNoParameter()
    {
        try {
            return($this->db->createUser());
        } catch(ErrorException $e) {
            return($this->db->errorno());
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
    function testCreateUserPositive()
    {
        $test = array("read", "read-write");
        $this->db->createUser("createuser-example", "example123", array("read", "read-write"));
        sleep(2);
        $this->db->queryUser("createuser-example", $roles);
        $this->db->dropUser("createuser-example");
        $result = array_diff_assoc_recursive($test, $roles);
        if(empty($result)) 
            return Aerospike::OK;
        else
            return Aerospike::ERR_CLIENT;
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
    function testCreateUserPositivePolicy()
    {
        $test = array("read", "read-write");
        $this->db->createUser("createuser-example", "example123", array("read", "read-write"), array(Aerospike::OPT_WRITE_TIMEOUT => 2000));
        sleep(2);
        $this->db->queryUser("createuser-example", $roles);
        $this->db->dropUser("createuser-example");
        $result = array_diff_assoc_recursive($test, $roles);
        if(empty($result)) 
            return Aerospike::OK;
        else
            return Aerospike::ERR_CLIENT;
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
    function testCreateUserPolicyIncorrect()
    {
        $test = array("read", "read-write");
        return($this->db->createUser("createuser-example", "example123", array("read", "read-write"), array(Aerospike::OPT_WRITE_TIMEOUT => 0.5)));
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
    function testCreateUserPasswordNull()
    {
        $pass = NULL;
        return ($this->db->createUser("createuser-example", $pass, array("read", "read-write")));
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
    function testCreateUserNull()
    {
        $user = NULL;
        return ($this->db->createUser($user, "example123", array("read", "read-write")));
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
    function testCreateUserRolesNull()
    {
        $roles = NULL;
        try {
            return ($this->db->createUser("createuser-example", "example123", $roles));
        } catch(ErrorException $e) {
            return($this->db->errorno());
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
    function testCreateUserRolesEmpty()
    {
        return $this->db->createUser("createuser-example", "example123", array());
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
    function testCreateUserExisting()
    {
        return ($this->db->createUser("john", "example123", array("read", "read-write")));
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
    function testCreateUserInvalidRole()
    {
        return ($this->db->createUser("createuser-example", "example123", array("invalid")));
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
    function testCreateUserUnderPrivileged()
    {
        $this->db->createUser("createuser-example", "example123", array("read"));
        sleep(2);
        $key = $this->db->initKey("test", "demo", 1);
        return ($this->db->put($key, array("age" => 1)));
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
    function testCreateUserMultiple()
    {
        $test = array("read", "read-write");
        $this->db->createUser("createuser-example", "example123", array("read", "read-write"));
        sleep(2);
        $this->db->queryUser("createuser-example", $roles);
        $result = $this->db->createUser("createuser2", "example123", array("read", "read-write"));
        sleep(2);
        $this->db->queryUser("createuser2", $roles);
        $this->db->dropUser("createuser2");
        $this->db->dropUser("createuser-example");
        $result = array_diff_assoc_recursive($test, $roles);
        if(empty($result)) 
            return Aerospike::OK;
        else
            return Aerospike::ERR_CLIENT;
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
    function testCreateUserLongUsername()
    {
        $user = "user";
        for ($i = 0; $i <1000; $i++) {
            $user = $user + "user";
        }
        $test = array("read", "read-write");
        $this->db->createUser($user, "example123", array("read", "read-write"));
        if ($this->db->errorno() != 0) {
            sleep(2);
            $this->db->queryUser($user, $roles);
            if ($this->db->errorno() != 0) {
                $this->db->dropUser($user);
                $result = array_diff_assoc_recursive($test, $roles);
                if(empty($result)) 
                    return Aerospike::OK;
                else
                    return Aerospike::ERR_CLIENT;
            }
        }
        return $this->db->errorno();
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
    function testCreateUserSpecialCharacterUsername()
    {
        $user = "!#Q#AEQ@#$%&^*((^&*~~~````";
        $test = array("read", "read-write");
        $this->db->createUser($user, "example123", array("read", "read-write"));
        if ($this->db->errorno() != 0) {
            sleep(2);
            $this->db->queryUser($user, $roles);
            if ($this->db->errorno() != 0) {
                $this->db->dropUser($user);
                $result = array_diff_assoc_recursive($test, $roles);
                if(empty($result)) 
                    return Aerospike::OK;
                else
                    return Aerospike::ERR_CLIENT;
            }
        }
        return $this->db->errorno();
    }
}
?>
