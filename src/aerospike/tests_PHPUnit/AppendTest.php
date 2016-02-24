<?php
require_once 'Util.inc';

class Append  extends PHPUnit_Framework_TestCase
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
        $this->objKey = array("ns" => "test", "set" => "demo","key" =>"Append_key");
    }
    
    function teardown(){
        self::$db->remove($this->objKey);
    }
   
    /**
     * @test
     * @dataProvider    dpNormalAppend
     */
    function normalAppend($putValue, $appendValue, $policyArray=array()) {
        self::$db->put($this->objKey, array("Greet"=>$putValue));
        self::$db->append($this->objKey, 'Greet', $appendValue);
        $status = self::$db->get($this->objKey, $get_record, array('Greet'));
        $this->assertEquals($status, AEROSPIKE::OK);
        $this->assertEquals($get_record['bins']['Greet'], $putValue.$appendValue);
    }
    
    function dpNormalAppend(){
        return array(
            "test append with string value"=>
            array(
                "Hello!",
                "World"
            ),
            "test append with string and policy "=>
            array(
                "For The",
                "World",
                array(Aerospike::OPT_WRITE_TIMEOUT=>1000,Aerospike::OPT_POLICY_COMMIT_LEVEL=>Aerospike::POLICY_COMMIT_LEVEL_MASTER)
            )
        );
    }
    
    /**
     * @test
     * @dataProvider     dpInvalidKeyArgAppend
     */
    function invalidKeyArgAppend($keyArray, $binValue, $policyArray=array()){
        $this->objKey = $keyArray;
        $status = self::$db->append($this->objKey, $binValue, ' World', $policyArray);
        $this->assertEquals($status, AEROSPIKE::OK);
    }
    
    function dpInvalidKeyArgAppend(){
        return array(
            "Test append with non existing key value"=>
            array(
                array("ns" => "test", "set" => "demo", "key" => "asdfg"),
                "Greet"
            ),
            
            "Test append with non existing bin value"=>
            array(
                array("ns" => "test", "set" => "demo", "key" => "myKey"),
                "asdfasdf"
            ),
            
            "Test append with non existing set value"=>
            array(
                array("ns" => "test", "set" => "qwerqewr", "key" => "myKey"),
                "Greet"
            ),
            
            "Test append with numeric key value"=>
            array(
                array("ns" => "test", "set" => "qwerqewr", "key" => 888888),
                "Greet"
            ),
        );
    }
  
    
    /**
     * @test
     * @dataProvider     dpNormalppendGenPolicy
     */
    function normalppendGenPolicy($genPolicy=array(), $expected){
        self::$db->put($this->objKey, array("Greet"=>"Hello!"));
        $existStatus = self::$db->exists($this->objKey, $metadata);
        $this->gen_value = $metadata["generation"];
        self::$db->append($this->objKey, 'Greet', ' World', $genPolicy);
        $getStatus = self::$db->get($this->objKey, $getRecords, array('Greet'));
        if($expected === "PASS"){
            $this->assertEquals($getRecords["bins"]["Greet"], "Hello! World");
        }else{
            $this->assertNotEquals($getRecords["bins"]["Greet"], "Hello! World");
        }
    }
    
    function dpNormalppendGenPolicy(){
        return array(
            "Negative test append with generation policy, POLICY_GEN_GT positive"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT, $this->gen_value)),
                "FAIL"
            ),
            
            "Positive test append with generation policy, POLICY_GEN_GT positive"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT, ($this->gen_value+10))),
                "PASS"
            ),
            
            "Positive test append with generation policy, POLICY_GEN_EQ positive"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_EQ, ($this->gen_value+1))),
                "PASS"
            ),
            
            "Negative test append with generation policy, POLICY_GEN_EQ positive"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_EQ, ($this->gen_value))),
                "FAIL"
            ),
            
            "Positive test append with generation policy, POLICY_GEN_IGNORE positive"=>
            array(
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_IGNORE)),
                "PASS"
            ),
        );
    }
    
    /**
     * @test
     */
    function testAppendWithPolicyKeyDigest() {
        $gen_value = 10;
        $this->objKey = self::$db->initKey("test", "demo", base64_decode("append_option_policy_key_digest"), true);
        $put_status = self::$db->put($this->objKey, array("Greet"=>"Hello!"), NULL, array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
        self::$db->append($this->objKey, 'Greet', ' World', array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
        $status = self::$db->get($this->objKey, $get_record, array('Greet'));
        $this->assertEquals($get_record['bins']['Greet'], "Hello! World");
    }
    
    /**
     * @test
     */
    function testAppendWithPolicyKeySend() {
        $gen_value = 10;
        $key = self::$db->initKey("test", "demo", 1);
        $put_status = self::$db->put($this->objKey, array("Greet"=>"Hello!"), NULL,
         array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND));
        self::$db->append($this->objKey, 'Greet', ' World', array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND));
        $status = self::$db->get($this->objKey, $get_record, array('Greet'));
        $this->assertEquals($get_record['bins']['Greet'], "Hello! World");
    }
}
?>
