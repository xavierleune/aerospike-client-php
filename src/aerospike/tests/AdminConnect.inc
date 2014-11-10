<?php
class AdminConnect extends AerospikeTestCommon
{
    protected function setUp(&$db, &$config) {
        /*$host = array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT);
        $config = array("hosts"=>array($host), "user"=>"admin", "pass"=>"admin");
        $db = new Aerospike($config);
        $res = $db->createUser("example", "example@123", array("read"));*/
    }

    protected function tearDown(&$db, &$config) {
       /* $db->dropUser("createuser-example");
        $db->close();*/
    }

    function testConnectPositive($db, $config) {
        $host = array("addr"=>AEROSPIKE_CONFIG_SECURE, "port"=>AEROSPIKE_CONFIG_SECURE_PORT);
        $config = array("hosts"=>array($host), "user"=>"admin", "pass"=>"admin");
        $db = new Aerospike($config);
        if ($db->isConnected()) {
            $db->close();
            return Aerospike::OK;
        } else 
        return Aerospike::ERR;
    }

    function testConnectSecurityNotSupported($db, $config) {
        $host = array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT);
        $config = array("hosts"=>array($host), "user"=>"admin", "pass"=>"admin");
        $db = new Aerospike($config);
        if ($db->isConnected()) {
            return Aerospike::OK;
        } else {
            return $db->errorno();
        }
    }

    function testConnectSecurityNotEnabled($db, $config) {
        $host = array("addr"=>AEROSPIKE_CONFIG_SECURITY_DISABLED, "port"=>AEROSPIKE_CONFIG_SECURITY_DISABLED_PORT);
        $config = array("hosts"=>array($host), "user"=>"admin", "pass"=>"admin");
        $db = new Aerospike($config);
        if ($db->isConnected()) {
            return Aerospike::OK;
        } else {
            return $db->errorno();
        }
    }

    function testConnectInvalidPassword($db, $config) {
        $host = array("addr"=>AEROSPIKE_CONFIG_SECURE, "port"=>AEROSPIKE_CONFIG_SECURE_PORT);
        $config = array("hosts"=>array($host), "user"=>"admin", "pass"=>"admin1");
        $db = new Aerospike($config);
        if ($db->isConnected()) {
            return Aerospike::OK;
        } else 
        return $db->errorno();
    }
}
?>
