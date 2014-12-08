<?php
class ChangePassword extends AerospikeTestCommon
{
    protected function setUp() {
        $host = array("addr"=>AEROSPIKE_CONFIG_SECURE, "port"=>AEROSPIKE_CONFIG_SECURE_PORT);
        $config = array("hosts"=>array($host), "user"=>"admin", "pass"=>"admin");
        $this->db = new Aerospike($config);
        if (!$this->db->isConnected()) {
            return $this->db->errorno();
        }
        $res = $this->db->createUser("example", "example@123", array("read"));
    }

    protected function tearDown() {
        $res = $this->db->dropUser("example");
        $this->db->close();
    }
    /**
     * @test
     * Basic Change Password without parameter
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
    function testChangePasswordNoParameter()
    {
        try {
            return($this->db->changePassword());
        } catch(ErrorException $e) {
            return($this->db->errorno());
        }
    }
    /**
     * @test
     * Basic Change Password positive
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
    function testChangePasswordPositive()
    {
	    $res = $this->db->changePassword("example", "john@123");
	    if ($res == Aerospike::OK) {
    	    return Aerospike::OK;
        } else {
    	    return $this->db->error();
	    }
    }
    /**
     * @test
     * Basic Change Password correct timeout
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
    function testChangePasswordCorrectTimeout()
    {
	    $res = $this->db->changePassword("example", "john@123", array(Aerospike::OPT_WRITE_TIMEOUT => 20000));
	    if ($res == Aerospike::OK) {
	        return Aerospike::OK;
        } else {
    	    return $this->db->errorno();
	    }
    }
    /**
     * @test
     * Basic Change Password incorrect timeout
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
    function testChangePasswordIncorrectTimeout()
    {
        $res = $this->db->changePassword("example", "john@123", array(Aerospike::OPT_WRITE_TIMEOUT => 0.2));
        if ($res == Aerospike::OK) {
	        return Aerospike::OK;
        } else {
		    return $this->db->errorno();
        }
    }
    /**
     * @test
     * Basic Change Password non-existent user
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
    function testChangePasswordNonExistentUser()
    {
        $res = $this->db->changePassword("abcdefgh", "john@123");
        if ($res == Aerospike::OK) {
	        return Aerospike::OK;
        } else {
		    return $this->db->errorno();
        }
    }
    /**
     * @test
     * Basic Change Password user as NULL
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
    function testChangePasswordNullUser()
    {
        $res = $this->db->changePassword(NULL, "example@123");
        if ($res == Aerospike::OK) {
	        return Aerospike::OK;
        } else {
		    return $this->db->errorno();
        }
    }
    /**
     * @test
     * Basic Change Password password as NULL
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
    function testChangePasswordNullPassword()
    {
        $res = $this->db->changePassword("example", NULL);
        if ($res == Aerospike::OK) {
	        return Aerospike::OK;
        } else {
		    return $this->db->errorno();
        }
    }
    /**
     * @test
     * Basic Change Password password is very long
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
    function testChangePasswordVeryLong()
    {
	    $pass = "pass";
	    for ($i=1; $i<100; $i++) {
		    $pass = $pass + "pass";
	    }
        $res = $this->db->changePassword("example", $pass);
        if ($res == Aerospike::OK) {
	        return Aerospike::OK;
        } else {
		    return $this->db->errorno();
        }
    }
}
