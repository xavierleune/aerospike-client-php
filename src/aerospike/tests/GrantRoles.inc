<?php
class GrantRoles extends AerospikeTestCommon
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
     * Basic GrantRoles without parameter
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
    function testGrantRolesNoParameter($db)
    {
        if ($db->isConnected()) {
            try {
                return($db->grantRoles());
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
     * Basic GrantRoles positive
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
    function testGrantRolesPositive($db)
    {
        if ($db->isConnected()) {
            return($db->grantRoles("example", array("read", "read-write")));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic GrantRoles positive with policy
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
    function testGrantRolesPositivePolicy($db)
    {
        if ($db->isConnected()) {
            return($db->grantRoles("example", array("read", "read-write"), array(Aerospike::OPT_WRITE_TIMEOUT => 1000)));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic GrantRoles positive with policy incorrect
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
    function testGrantRolesPolicyIncorrect($db)
    {
        if ($db->isConnected()) {
            return($db->grantRoles("example", array("read", "read-write"), array(Aerospike::OPT_WRITE_TIMEOUT => 0.5)));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic GrantRoles with NULL username
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
    function testGrantRolesUserIsNull($db)
    {
        if ($db->isConnected()) {
            return($db->grantRoles(NULL, array("read", "read-write")));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic GrantRoles non-existent user
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
    function testGrantRolesNonExistentUser($db)
    {
        if ($db->isConnected()) {
            return($db->grantRoles("abcdefg", array("read", "read-write")));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic GrantRoles empty roles
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
    function testGrantRolesEmptyRoles($db)
    {
        if ($db->isConnected()) {
            return($db->grantRoles("example", array()));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic GrantRoles invalid role list
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
    function testGrantRolesInvalidRoleList($db)
    {
        if ($db->isConnected()) {
            return($db->grantRoles("example", array("viewer")));
        }
        else {
            return($db->errorno());
        }
    }
}
?>
