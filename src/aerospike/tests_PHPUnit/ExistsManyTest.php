<?php
require_once 'Util.inc';

class ExistsMany extends PHPUnit_Framework_TestCase
{   
    static $config = array("hosts"=>array(array("addr"=>"127.0.0.1", "port"=>"3000")));
    public $objKey = null;
    static $db = null;
    
    /**
    * @beforeClass
    */
    public static function getConnection(){
        self::$db = new Aerospike(self::$config);
        self::assertTrue(self::$db->isConnected());
    }
    
    function teardown(){
        self::$db->remove($this->objKey);
        if(isset($this->key2)){
            self::$db->remove($this->key2);
        }
        if(isset($this->key1)){
            self::$db->remove($this->key1);
        }
    }
    
    /**
     * @before
     */
    public function insertRecord(){
        $this->objKey = self::$db->initKey("test", "demo", "existsMany1");
        self::$db->put($this->objKey, array("binA"=>10));
        $this->objKeyArray[] = $this->objKey;
        $this->objKey = self::$db->initKey("test", "demo", "existsMany2");
        self::$db->put($this->objKey, array("binB"=>20));
        $this->objKeyArray[] = $this->objKey;
        $this->objKey = self::$db->initKey("test", "demo", "existsMany3");
        self::$db->put($this->objKey, array("binC"=>30));
        $this->objKeyArray[] = $this->objKey;
    }
    
    /**
     * @test
     * @dataProvider  dpNormalExistMany
     */
    function normalExistMany($policy=array()) {
        $status = self::$db->existsMany($this->objKeyArray, $metadata, array(Aerospike::OPT_READ_TIMEOUT=>3000));
        foreach($metadata as $key=>$value) {
            $this->assertEquals($this->objKeyArray[$key]["key"], $value["key"]["key"]);
            $this->assertTrue(array_key_exists("ttl", $value["metadata"]));
            $this->assertTrue(array_key_exists("generation", $value["metadata"]));
        }
    }
    
    function dpNormalExistMany(){
        return array(
            "Exists many with policy array" => 
            array(
                array(Aerospike::OPT_READ_TIMEOUT=>3000)
            )
        );
    }
       
    /**
     * @test
     * @dataProvider   
     */
    function normal_002() {
        $my_keys = $this->objKeyArray;
        $key5 = array(self::$db->initKey("test", "demo", "existsMany5"));
        array_splice($my_keys, 1, 0, $key5);
        $status = self::$db->existsMany($my_keys, $metadata);
        $this->assertEquals($status, Aerospike::OK);
        $i = 0;
        foreach ($metadata as $metadata_key=>$value) {
            if (strcmp($my_keys[$i]["key"], $metadata_key) || ($i == 1 && $value)) {
                return Aerospike::ERR_CLIENT;
            } else if ($i != 1) {
                if (!(array_key_exists("ttl", $value) && array_key_exists("generation", $value))) {
                    return AEROSPIKE::ERR_CLIENT;
                }
            }
            $i++;
        }
        return $status;
    }

    /**
     * @test
     */
    function edge_001() {
        $status = self::$db->existsMany(array(), $metadata);
        $this->assertEquals($status, AEROSPIKE::OK);
    }
    
    /**
     * @test
     */
    function edge_002() {
        try{
            $status = self::$db->existsMany("", $metadata);
        }catch(Exception $e){
            $status= AEROSPIKE::ERR_PARAM;
        }
        $this->assertEquals($status,AEROSPIKE::ERR_PARAM);
    }
    /**
     * @test
     */
    function edge_003() {
        try {
            $status = self::$db->existsMany();
        } catch (Exception $e) {
            $status = Aerospike::ERR_PARAM;
        } 
        $this->assertEquals($status, Aerospike::ERR_PARAM);
    }
    /**
     * @test
     */
    function normal_003() {
        $metadata = "abc";
        $status = self::$db->existsMany($this->objKeyArray, $metadata, array(Aerospike::OPT_READ_TIMEOUT=>3000));
        foreach($metadata as $key=>$value) {
            $this->assertEquals($this->objKeyArray[$key]["key"], $value["key"]["key"]);
            $this->assertTrue(array_key_exists("ttl", $value["metadata"]));
            $this->assertTrue(array_key_exists("generation", $value["metadata"]));
        }
    }
    /**
     * @test
     *
     */
    function edge_004() {
        try {
            $status = self::$db->existsMany($this->objKeyArray, $metadata, 20);
        } catch (Exception $e) {
            $status = Aerospike::ERR_PARAM;
        }
        $this->assertEquals($status, Aerospike::ERR_PARAM);
    }
    /**
     * @test
     * 
     */
    function normalMultipleArray() {
        $this->key1 = self::$db->initKey("test", "demo", "existsMany1", true);
        self::$db->put($this->key1, array("binA"=>10));
        $this->keys[] = $this->key1;
        
        $this->key2 = self::$db->initKey("test", "demo", "existsMany2", true);
        self::$db->put($this->key2, array("binB"=>20));
        $this->keys[] = $this->key2;
        
        $keys = array($this->key1, $this->key2);
        $status = self::$db->existsMany($keys, $metadata, array(Aerospike::OPT_READ_TIMEOUT=>3000));
        
        
        $this->assertEquals($status, AEROSPIKE::OK);
        $this->assertFalse(empty($metadata));
        
        foreach($metadata as $key=>$value) {
            $this->assertTrue(array_key_exists("ttl", $value["metadata"]));
            $this->assertTrue(array_key_exists("generation", $value["metadata"]));
        }
    }
}
