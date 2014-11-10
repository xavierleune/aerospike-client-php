<?php
class ReplaceRoles extends AerospikeTestCommon
{
    protected function setUp(&$db, &$config) {
        $host = array("addr"=>AEROSPIKE_CONFIG_SECURE, "port"=>AEROSPIKE_CONFIG_SECURE_PORT);
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
     * Basic ReplaceRoles without parameter
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
    function testReplaceRolesNoParameter($db)
    {
        if ($db->isConnected()) {
            try {
                return($db->replaceRoles());
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
     * Basic ReplaceRoles positive
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
    function testReplaceRolesPositive($db)
    {
        if ($db->isConnected()) {
            $status = $db->replaceRoles("example", array("read", "read-write"));
            sleep(4);
            $status = $db->queryUser("example", $roles);
            sleep(2);
            $result = array_diff_assoc_recursive($roles, array("read","read-write"));
            if(!empty($result)) 
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
     * Basic ReplaceRoles positive with policy
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
    function testReplaceRolesPositivePolicy($db)
    {
        if ($db->isConnected()) {
            $db->replaceRoles("example", array("read", "read-write"), array(Aerospike::OPT_WRITE_TIMEOUT => 1000));
            sleep(4);
            $db->queryUser("example", $roles);
            $result = array_diff_assoc_recursive($roles, array("read","read-write"));
            if(!empty($result)) 
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
     * Basic ReplaceeRoles positive with policy incorrect
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
    function testReplaceRolesPolicyIncorrect($db)
    {
        if ($db->isConnected()) {
            return($db->replaceRoles("example", array("read", "read-write"), array(Aerospike::OPT_WRITE_TIMEOUT => 0.5)));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic ReplaceRoles with NULL username
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
    function testReplaceRolesUserIsNull($db)
    {
        if ($db->isConnected()) {
            return($db->replaceRoles(NULL, array("read", "read-write")));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic ReplaceRoles non-existent user
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
    function testReplaceRolesNonExistentUser($db)
    {
        if ($db->isConnected()) {
            return($db->replaceRoles("abcdefgh", array("read", "read-write")));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic ReplaceRoles empty roles
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
    function testReplaceRolesEmptyRoles($db)
    {
        if ($db->isConnected()) {
            return($db->replaceRoles("example", array()));
        }
        else {
            return($db->errorno());
        }
    }
    /**
     * @test
     * Basic ReplaceRoles invalid role list
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
    function testReplaceRolesInvalidRoleList($db)
    {
        if ($db->isConnected()) {
            return($db->replaceRoles("example", array("viewer")));
        }
        else {
            return($db->errorno());
        }
    }
}
?>
