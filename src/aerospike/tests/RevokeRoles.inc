<?php
class RevokeRoles extends AerospikeTestCommon
{
    protected function setUp(&$db, &$config) {
        $host = array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT);
        $config = array("hosts"=>array($host), "user"=>"admin", "pass"=>"admin");
        $db = new Aerospike($config);
        $res = $db->createUser("example", "example@123", array("read","read-write","sys-admin"));
    }

    protected function tearDown(&$db, &$config) {
        $res = $db->dropUser("example");
        $db->close();
    }
    /**
     * @test
     * Basic RevokeRoles without parameter
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
    function testRevokeRolesNoParameter($db)
    {
        if ($db->isConnected()) {
            try {
                return($db->revokeRoles());
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
     * Basic RevokeRoles positive
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
    function testRevokeRolesPositive($db)
    {
        if ($db->isConnected()) {
             $status = $db->revokeRoles("example", array("read", "read-write"));
	     sleep(2);
	     $status = $db->queryUser("example", $roles);
	     sleep(2);
	     var_dump($roles);
             $result = array_diff_assoc_recursive($roles, array("sys-admin"));
	     if(!$result) 
		return Aerospike::ERR;
	     else
		return Aerospike::OK;
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic RevokeRoles positive with policy
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
    function testRevokeRolesPositivePolicy($db)
    {
        if ($db->isConnected()) {
             $db->revokeRoles("example", array("read", "read-write"), array("OPT_WRITE_TIMEOUT" => 1000));
	     sleep(2);
             $db->queryUser("example", $roles);
             var_dump($roles);
             $result = array_diff_assoc_recursive($roles, array("sys-admin"));
	     if(!$result) 
		return Aerospike::ERR;
	     else
		return Aerospike::OK;
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic RevokeRoles positive with policy incorrect
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
    function testRevokeRolesPolicyIncorrect($db)
    {
        if ($db->isConnected()) {
            return($db->revokeRoles("example", array("read", "read-write"), array("OPT_WRITE_TIMEOUT" => 0.5)));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic RevokeRoles with NULL username
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
    function testRevokeRolesUserIsNull($db)
    {
        if ($db->isConnected()) {
                return($db->revokeRoles(NULL, array("read", "read-write")));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic RevokeRoles non-existent user
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
    function testRevokeRolesNonExistentUser($db)
    {
        if ($db->isConnected()) {
                return($db->revokeRoles("john", array("read", "read-write")));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic RevokeRoles empty roles
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
    function testRevokeRolesEmptyRoles($db)
    {
        if ($db->isConnected()) {
             return($db->revokeRoles("example", array()));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic RevokeRoles all roles
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
    function testRevokeRolesAllRoles($db)
    {
        if ($db->isConnected()) {
             $db->revokeRoles("example", array("read", "read-write", "sys-admin"));
	     sleep(2);
             $db->queryUser("example", $roles);
             $result = array_diff_assoc_recursive($roles, array());
	     if(!$result) 
		return Aerospike::ERR;
	     else
		return Aerospike::OK;
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic RevokeRoles non-possessed role
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
    function testRevokeRolesNonpossessedRoles($db)
    {
        if ($db->isConnected()) {
             $res = $db->createUser("john", "john@123", array("read","read-write"));
	     sleep(2);
             $db->revokeRoles("john", array("sys-admin"));
	     sleep(2);
             $db->queryUser("john", $roles);
             $result = array_diff_assoc_recursive($roles, array("read", "read-write"));
	     if(!$result) 
		return Aerospike::ERR;
	     else
		return Aerospike::OK;
             $res = $db->dropUser("john");
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic RevokeRoles invalid role list
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
    function testRevokeRolesInvalidRoleList($db)
    {
        if ($db->isConnected()) {
            return($db->revokeRoles("example", array("viewer")));
        }
        else {
            return($db->errorno());
        }
    }
}
?>
