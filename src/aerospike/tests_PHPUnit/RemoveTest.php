<?php
require_once 'Util.inc';

class Remove extends PHPUnit_Framework_TestCase
{
    static $config = array("hosts"=>array(array("addr"=>"127.0.0.1", "port"=>"3000")));
    public $objKey = null;
    public $gen_value = null;
    static $db = null;
    
    /**
    * @beforeClass
    */
    public static function getConnection(){
        self::$db = new Aerospike(self::$config);
    }
    
    function setUp() {
        self::$db = new Aerospike(self::$config);
        $this->objKey = self::$db->initKey("test", "demo", "RemoveBin_key");
        self::$db->put($this->objKey, array("bin"=>"123123"));
    }
    
    function teardown(){
        self::$db->remove($this->objKey);
    }
    
    /**
     * @test
     */
    function normalRemove() {
        $statusRem = self::$db->remove($this->objKey);
        $this->assertEquals($statusRem, AEROSPIKE::OK);
    }
    
    /**
     * @test
     */
    function normalRemoveByDigest() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        if (!$db->isConnected()) {
            return $db->errorno();
        }

        for ($i = 0; $i < 1000; $i++) {
            $this->objKey = $db->initKey("test", "demo", $i);
            $db->put($this->objKey, array("email"=>$i));
        }
        $digests = array();
        $status = $db->scan('test','demo', function ($record) use (&$digests, &$db) {
            $digests[] = $record["key"]["digest"];
        });
        $keys = array();
        $i = 0;
        foreach ($digests as $digest) {
            $keys[] = $db->initKey ('test', 'demo', $digest, true);
            $db->remove($keys[$i]);
            $i++;
        }
        foreach ($keys as $key) {
            if (Aerospike::OK == $db->exists($key, $metadata)) {
                $db->close();
                $this->assertTrue(False);
            }
        }
        $db->close();
        $this->assertTrue(True);
    }

    /**
     * @test
     * @dataProvider  dpNormalRemovePolicy
     */
    function normalRemovePolicy($arrayPolicy=array()) {
        self::$db->put($this->objKey, array("bin"=>"Hello! World"));
        $statusRem = self::$db->remove($this->objKey, $arrayPolicy);
        $this->assertEquals($statusRem, AEROSPIKE::OK);
    }
    
    function dpNormalRemovePolicy(){
        return array(
            "Test remove with array"=>
            array(
                array(Aerospike::OPT_POLICY_RETRY =>Aerospike::POLICY_RETRY_NONE,
                Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND,
                Aerospike::OPT_POLICY_COMMIT_LEVEL=>Aerospike::POLICY_COMMIT_LEVEL_MASTER)
            )
        );
    }
    
    /**
     * @test
     * @dataProvider  dpNormalRemoveGenPolicy
     * 
     */
    function normalRemoveGenPolicy($genPolicy, $expectedResult){
        self::$db->put($this->objKey, array("bin"=>"Hello! World"));
        $exists_status = self::$db->exists($this->objKey, $metadata);
        $this->gen_value = $metadata["generation"];
        $statusRemove= self::$db->remove($this->objKey, $genPolicy);
        if($expectedResult=="PASS"){
            $this->assertEquals($statusRemove, AEROSPIKE::OK);
        }else{
            $this->assertNotEquals($statusRemove, AEROSPIKE::OK);
        }
        
    }
    
    function dpNormalRemoveGenPolicy(){
        return array(
            "POLICY_GEN_IGNORE, positive"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_IGNORE)),
                "PASS"
            ),

            "POLICY_GEN_EQ, positive"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_EQ, ($this->gen_value+2))),
                "PASS"
            ),
            
            "POLICY_GEN_GT, positive"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT, ($this->gen_value+5))),
                "PASS"
            ),
            
            "POLICY_GEN_GT, negative"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT, ($this->gen_value))),
                "FAIL"
            ),
                        
            "POLICY_GEN_EQ, negative"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_EQ, ($this->gen_value))),
                "FAIL"
            )
        );
    }
    
    /**
     *  @test 
     *  @dataProvider    dpErrorKeyNotExists
     */
    function errorKeyNotExists($keyArray){
        $this->objKey = array("ns"=>"test", "set"=>"demo", "key"=>"keyNotExists");
        self::$db->put($this->objKey, array("bin"=>"Hello! World"));
        $this->objKey = $keyArray;
        self::$db->remove($this->objKey, array(Aerospike::OPT_POLICY_RETRY => Aerospike::POLICY_RETRY_NONE, Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
    }
    
    function dpErrorKeyNotExists(){
        return array(
            "non existing key"=>
            array(
                array("ns" => "test", "set" => "demo", "key" => "-----ss-----")
            ),
            
            "non existing set"=>
            array(
                array("ns" => "test", "set" => "----notExistingSet---", "key" => "keyNotExists")
            ),
            
            "non existing ns"=>
            array(
                array("ns" => "---nsNotExists---", "set" => "demo", "key" => "keyNotExists")
            )
        );
    }
    
    function normalPolicyKeySend() {
        $put_status = self::$db->put($this->objKey, array("bin1"=>"1"), NULL, array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND));
        self::$db->remove($this->objKey, array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND));
        $status = self::$db->get($this->objKey, $get_record, array('bin1'));
    }
    
    function normalPolicyKeyDigest() {
        $this->objKey = self::$db->initKey("test", "demo", base64_decode("removebin_policy_key_digest"), true);
        $put_status = self::$db->put($this->objKey, array("bin1"=>"1"), NULL, array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
        self::$db->remove($this->objKey, array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
        $status = self::$db->get($this->objKey, $get_record, array('bin1'));
        
    }
}
?>
