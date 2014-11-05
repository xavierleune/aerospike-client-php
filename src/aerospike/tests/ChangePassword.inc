<?php
class ChangePassword extends AerospikeTestCommon
{
    protected function setUp(&$db, &$config) {
        $host = array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT);
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
    function testChangePasswordNoParameter($db)
    {
        if ($db->isConnected()) {
            try {
                return($db->changePassword());
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
    function testChangePasswordPositive($db)
    {
        if ($db->isConnected()) {
	   $res = $db->changePassword("example", "john@123");
	   if ($res == Aerospike::OK) {
    		return Aerospike::OK;
           } else {
    		return $db->error();
	   }
        }
        else {
            return($db->errorno());
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
    function testChangePasswordCorrectTimeout($db)
    {
        if ($db->isConnected()) {
	   $res = $db->changePassword("example", "example@123", array(Aerospike::OPT_WRITE_TIMEOUT => 20000));
	   if ($res == Aerospike::OK) {
		return Aerospike::OK;
           } else {
    		return $db->errorno();
	   }
        }
        else {
            return($db->errorno());
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
    function testChangePasswordIncorrectTimeout($db)
    {
        if ($db->isConnected()) {
	   $res = $db->changePassword("example", "example@123", array(Aerospike::OPT_WRITE_TIMEOUT => 0.2));
	   if ($res == Aerospike::OK) {
		return Aerospike::OK;
           } else {
    		return $db->errorno();
	   }
        }
        else {
            return($db->errorno());
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
    function testChangePasswordNonExistentUser($db)
    {
        if ($db->isConnected()) {
	   try {
	   $res = $db->changePassword("john", "john@123");
	   } catch(ErrorException $e) {
	   if ($res == Aerospike::OK) {
		return Aerospike::OK;
           } else {
    		echo $db->errorno();
	   }
	  }
        }
        else {
            return($db->errorno());
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
    function testChangePasswordNullUser($db)
    {
        if ($db->isConnected()) {
	   $res = $db->changePassword(NULL, "example@123");
	   if ($res == Aerospike::OK) {
		return Aerospike::OK;
           } else {
    		echo $db->errorno();
	   }
        }
        else {
            return($db->errorno());
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
    function testChangePasswordNullPassword($db)
    {
        if ($db->isConnected()) {
	   $res = $db->changePassword("example", NULL);
	   if ($res == Aerospike::OK) {
		return Aerospike::OK;
           } else {
    		echo $db->errorno();
	   }
        }
        else {
            return($db->errorno());
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
    function testChangePasswordVeryLong($db)
    {
	$pass = "pass";
	for ($i=1; $i<100; $i++) {
		$pass = $pass + "pass";
	}
        if ($db->isConnected()) {
	   $res = $db->changePassword("example", $pass);
	   if ($res == Aerospike::OK) {
		return Aerospike::OK;
           } else {
    		echo $db->errorno();
	   }
        }
        else {
            return($db->errorno());
        }
    }
}
