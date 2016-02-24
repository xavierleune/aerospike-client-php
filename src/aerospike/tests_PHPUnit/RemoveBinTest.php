<?php
require_once 'Util.inc';

class RemoveBin extends PHPUnit_Framework_TestCase
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
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        self::$db = new Aerospike($config);
        $this->assertTrue(self::$db->isConnected());
        $this->objKey = self::$db->initKey("test", "demo", "removeBin_test");
        self::$db->put($this->objKey, array("bin1"=>"1"));
    }
    
    function teardown(){
        self::$db->remove($this->objKey);
    }
     
    /**
     * @test
     * @dataProvider  dpNormalBinRemove
     */
    function normalBinRemove($policy = array()) {
        self::$db->removeBin($this->objKey, array("bin1"), $policy);
        $status = self::$db->get($this->objKey, $get_record, array('bin1'));
        $this->assertFalse(array_key_exists('bins', $get_record));
    }
    
    function dpNormalBinRemove(){
        return array(
            "Test remove bin with option: OPT_WRITE_TIMEOUT"=>
            array(
                array(Aerospike::OPT_WRITE_TIMEOUT=>1000)
            ),
            "test remove bin with option: POLICY_COMMIT_LEVEL_MASTER"=>
            array(
                array(Aerospike::OPT_POLICY_COMMIT_LEVEL=>Aerospike::POLICY_COMMIT_LEVEL_MASTER)
            ),
        );
    }
    
    /**
    *   @test
    *   @dataProvider   dpBinRemoveNotExists
    */
    function binRemoveNotExists($keyArray){
        $this->objKey = array("ns"=>"test", "set"=>"demo", "key"=>"myKey");
        self::$db->put($this->objKey, array("bin"=>"Hello! World"));
        $this->objKey = $keyArray;
        $statusRemoveBin = self::$db->removeBin($this->objKey, array(Aerospike::OPT_POLICY_RETRY => Aerospike::POLICY_RETRY_NONE, Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
        $this->assertEquals($statusRemoveBin, AEROSPIKE::ERR_CLIENT);
    }
    
    function dpBinRemoveNotExists(){
        return array(
            "non existing key"=>
            array(
                array("ns" => "test", "set" => "demo", "key" => "--ss--")
            ),
            "non existing set"=>
            array(
                array("ns" => "test", "set" => "--setNotExists--", "key" => "myKey")
            ),
            "non existing ns"=>
            array(
                array("ns" => "--nsNotExists--", "set" => "demo", "key" => "myKey")
            ),
        );
    }
    
    /**
     * @test
     * @dataProvider    dpRemoveBinGenPolicy
     */
    function removeBinGenPolicy($genArray, $expected){
        $statusPut = self::$db->put($this->objKey, array("bin"=>"myBinValue"));
        $this->assertEquals($statusPut, AEROSPIKE::OK);
        $exists_status = self::$db->exists($this->objKey, $metadata);
        $this->gen_value = $metadata["generation"];
        $statusRem = self::$db->removeBin($this->objKey, array("bin"), $genArray);
        $status = self::$db->get($this->objKey, $get_record, array('bin'));
        if($expected==="PASS"){
            $this->assertTrue(empty($get_record["bins"]));
        }else{
            $this->assertFalse(empty($get_record["bins"]));
        }
    }
    
    
    function dpRemoveBinGenPolicy(){
        return array(
            "POLICY_GEN_GT, positive tc"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT, ($this->gen_value+1))),
                "PASS"
            ),
            
            "POLICY_GEN_GT, negative tc"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT, ($this->gen_value))),
                "FAIL"
            ),
            
            "POLICY_GEN_EQ, positive tc"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_EQ, ($this->gen_value+1))),
                "PASS"
            ),
            
            "POLICY_GEN_EQ, negative tc"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_EQ, ($this->gen_value))),
                "FAIL"
            ),
            
            "POLICY_GEN_IGNORE, negative tc"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_IGNORE)),
                "PASS"
            ),
        );
    }
    
    /**
     * @test
     * 
     */
    function normalPolicyKeySend() {
        $put_status = self::$db->put($this->objKey, array("bin1"=>"1"), NULL,
        array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND));
        self::$db->removeBin($this->objKey, array("bin1"), array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND));
        $status = self::$db->get($this->objKey, $get_record, array('bin1'));
        $this->assertEquals($status, AEROSPIKE::ERR_RECORD_NOT_FOUND);
    }
    /**
     * @test
     * 
     */
    function normalPolicyKeyDigest() {
        $this->objKey = self::$db->initKey("test", "demo", base64_decode("removebin_policy_key_digest"), true);
        $put_status = self::$db->put($this->objKey, array("bin1"=>"1"), NULL,array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
        self::$db->removeBin($this->objKey, array("bin1"), array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
        $status = self::$db->get($this->objKey, $get_record, array('bin1'));
        $this->assertEquals($status, AEROSPIKE::ERR_RECORD_NOT_FOUND);
    }
}
?>
