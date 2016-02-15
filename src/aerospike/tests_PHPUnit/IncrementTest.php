<?php
require_once 'Util.inc';

class Increment extends PHPUnit_Framework_TestCase
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
        self::assertTrue(self::$db->isConnected());
    }
    
    function setUp() {
        if(isset($this->objKey)){
            self::$db->remove($this->objKey);
        }
        $this->objKey = self::$db->initKey("test", "demo", "increment_key");
    }
    function teardown(){
        self::$db->remove($this->objKey);
    }

    /**
     * @test
     * @dataProvider  dpNormalIncrement
     */
    function normalIncrement($valPut, $valIncr, $valExpected, $arrPolicy=array()) {
        self::$db->put($this->objKey, array("bin1"=>$valPut));
        self::$db->increment($this->objKey, 'bin1', $valIncr, $arrPolicy);
        $status = self::$db->get($this->objKey, $get_record, array('bin1'));
        $this->assertEquals($get_record["bins"]["bin1"], $valExpected);
    }
    
    function dpNormalIncrement(){
        return array(
            "Check with positive integer"=>
            array(
                5,1,6
            ),
            
            "Check with positive integer with policy"=>
            array(
                5,1,6,
                array(Aerospike::OPT_WRITE_TIMEOUT=>1000,
                Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST,
                Aerospike::OPT_POLICY_COMMIT_LEVEL=>Aerospike::POLICY_COMMIT_LEVEL_MASTER)
            )
            
        );
        
    }
    
    /**
     * @test
     * @dataProvider        dpNormalIncrementNoPut
     */
    function normalIncrementNoPut($valIncrement, $expResult) {
        self::$db->increment($this->objKey, "bin2", $valIncrement, array(Aerospike::OPT_WRITE_TIMEOUT=>1000));
        $status = self::$db->get($this->objKey, $get_record, array('bin2'));
        if($expResult=="PASS"){
            $this->assertEquals($get_record['bins']['bin2'], $valIncrement); 
        }else{
            $this->assertTrue(isset($get_record));
        }
    }
    
    function dpNormalIncrementNoPut(){
        return array(
            "Test increment with positive integer"=>
            array(
                3,
                "PASS"
            ),
            
            "Test increment with positive integer passed as string"=>
            array(
                "5",
                "PASS"
            ),
            
            "Test increment with string value passed as offset"=>
            array(
                "abcdef",
                "FAIL"
            ),
            
            "Test increment with negative offset number passed as string"=>
            array(
                "-10",
                "FAIL"
            ),
        );
        
    }
    
    
    /**
     * @test
     * @dataProvider            dpNormalKeyNotExist
     */
    function normalKeyNotExist($keyArray, $binValue="bin1") {
        $this->objKey = $keyArray;
        self::$db->increment($this->objKey, $binValue, 4,
            array(Aerospike::OPT_POLICY_RETRY=>Aerospike::POLICY_RETRY_ONCE,
            Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND));
        $status = self::$db->get($this->objKey, $get_record, array($binValue));
        $this->assertEquals($get_record['bins'][$binValue], 4);
        
    }
    
    
    function dpNormalKeyNotExist(){
        return array(
            "key does not exists"=>
            array(
                array("ns" => "test", "set" => "demo", "key" => "----ssss----"),
            ),
            
            "set does not exists"=>
            array(
                array("ns" => "test", "set" => "--ssss--", "key" => "----ssss----")
            ),
            
            "bin value does not exists"=>
            array(
                array("ns" => "test", "set" => "--ssss--", "key" => "----ssss----"),
                "binNotExists"
            )
        );
    }
    
    
    /**
     * @test
     * @dataProvider    dpNormalIncrGenPolicy
     */
    function normalIncrGenPolicy($genPolicy, $expResult) {
        $statusPut = self::$db->put($this->objKey,array("bin1"=>1));
        $exists_status = self::$db->exists($this->objKey, $metadata);
        $this->gen_value = $metadata["generation"];
        self::$db->increment($this->objKey, 'bin1', 4, $genPolicy);
        $status = self::$db->get($this->objKey, $get_record, array('bin1'));
        if($expResult=="PASS"){
            $this->assertEquals($get_record['bins']['bin1'], 5);
        }else{
            $this->assertNotEquals($get_record['bins']['bin1'], 5);
        }
    }
    
    function dpNormalIncrGenPolicy(){
        return array(
            "POLICY_GEN_GT, negative test case"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT, $this->gen_value)),
                "FAIL"
            ),
            
            "POLICY_GEN_GT, positive test case"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT, $this->gen_value+2)),
                "PASS"
            ),
            
            "POLICY_GEN_EQ, negative test case"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_EQ, $this->gen_value)),
                "FAIL"
            ),
            
            "POLICY_GEN_EQ, positive test case"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_EQ, $this->gen_value+1)),
                "PASS"
            ),
            
            "POLICY_GEN_IGNORE, positive test case"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_IGNORE)),
                "PASS"
            )
        );
    }
    
    /**
     * @test
     */
    function normalPolicyKeyDigest() {
        $gen_value = 10;
        $key = self::$db->initKey("test", "demo", base64_decode("increment_policy_key_digest"), true);
        $put_status = self::$db->put($key, array("bin1"=>1), NULL, array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
        self::$db->increment($key, 'bin1', 4, array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
        $status = self::$db->get($key, $get_record, array('bin1'));
        $this->assertEquals($get_record['bins']['bin1'], 5);
    }
    
    /**
     * @test
     */
    function normalPolicyKeySend() {
        $gen_value = 10;
        $key = self::$db->initKey("test", "demo", 1);
        $put_status = self::$db->put($key, array("bin1"=>1), NULL, array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND));
        self::$db->increment($key, 'bin1', 4, array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND));
        $status = self::$db->get($key, $get_record, array('bin1'));
        $this->assertEquals($get_record['bins']['bin1'], 5);
    }
    
}
?>
