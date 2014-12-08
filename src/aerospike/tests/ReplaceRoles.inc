<?php
class ReplaceRoles extends AerospikeTestCommon
{
    protected function setUp() {
        $host = array("addr"=>AEROSPIKE_CONFIG_SECURE, "port"=>AEROSPIKE_CONFIG_SECURE_PORT);
        $config = array("hosts"=>array($host), "user"=>"admin", "pass"=>"admin");
        $this->db = new Aerospike($config);
        $res = $this->db->createUser("example", "example@123", array("read","read-write","sys-admin"));
        if (!$this->db->isConnected()) {
            return $this->db->errorno();
        }
    }

    protected function tearDown() {
        $res = $this->db->dropUser("example");
        $this->db->close();
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
    function testReplaceRolesNoParameter()
    {
       try {
            return($this->db->replaceRoles());
        } catch(ErrorException $e) {
            return($this->db->errorno());
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
    function testReplaceRolesPositive()
    {
        $status = $this->db->replaceRoles("example", array("read", "read-write"));
        sleep(4);
        $status = $this->db->queryUser("example", $roles);
        sleep(2);
        $result = array_diff_assoc_recursive($roles, array("read","read-write"));
        if(!empty($result)) {
            return Aerospike::ERR_CLIENT;
        } else {
            return Aerospike::OK;
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
    function testReplaceRolesPositivePolicy()
    {
        $this->db->replaceRoles("example", array("read", "read-write"), array(Aerospike::OPT_WRITE_TIMEOUT => 1000));
        sleep(4);
        $this->db->queryUser("example", $roles);
        $result = array_diff_assoc_recursive($roles, array("read","read-write"));
        if(!empty($result)) {
            return Aerospike::ERR_CLIENT;
        } else {
            return Aerospike::OK;
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
    function testReplaceRolesPolicyIncorrect()
    {
        return($this->db->replaceRoles("example", array("read", "read-write"), array(Aerospike::OPT_WRITE_TIMEOUT => 0.5)));
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
    function testReplaceRolesUserIsNull()
    {
        return($this->db->replaceRoles(NULL, array("read", "read-write")));
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
    function testReplaceRolesNonExistentUser()
    {
        return($this->db->replaceRoles("abcdefgh", array("read", "read-write")));
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
    function testReplaceRolesEmptyRoles()
    {
        return($this->db->replaceRoles("example", array()));
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
    function testReplaceRolesInvalidRoleList()
    {
        return($this->db->replaceRoles("example", array("viewer")));
    }
}
?>
