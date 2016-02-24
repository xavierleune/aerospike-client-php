<?php
require_once 'Util.inc';require_once 'Util.inc';

/**
 *Basic getMany opeartion tests
*/
class GetMany extends PHPUnit_Framework_TestCase
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
    
    function setUp() {
        
        $this->objKey = self::$db->initKey("test", "demo", "getMany1");
        $put_record1 = array("binA"=>10, "binB"=>20, "binC"=>30);
        self::$db->put($this->objKey, $put_record1);
        $this->keys[] = $this->objKey;
        
        $this->objKey = self::$db->initKey("test", "demo", "getMany2");
        $put_record2 = array("binA"=>40, "binB"=>50, "binC"=>60);
        self::$db->put($this->objKey, $put_record2);
        $this->keys[] = $this->objKey;
        
        $this->objKey = self::$db->initKey("test", "demo", "getMany3");
        $put_record3 = array("binA"=>70, "binB"=>80, "binC"=>90);
        self::$db->put($this->objKey, $put_record3);
        $this->keys[] = $this->objKey;
        
        $this->put_records = array($put_record1, $put_record2, $put_record3);
            
    }
    
    function teardown(){
        self::$db->remove($this->keys);
       
    }
    
    /**
     * @afterClass
     */
    function removeKeyArray(){
        $statusRemove = self::$db->remove($this->put_records);
        self::$db->assertEquals($statusRemove, AEROSPIKE::OK);
        $statusRemove = self::$db->remove($this->keyArray);
        self::$db->assertEquals($statusRemove, AEROSPIKE::OK);
        
    }
    
    /**
     * @test
     * @dataProvider  dpNormal001
     */
    function normal_001($keyArray, $putArray, $ttl, $option=array()) {
        
        for($count=0; $count < count($putArray); $count++){
            
            self::$db->put($keyArray[$count], $putArray[$count]);
            $this->keyArray[] = $keyArray[$count];
        }
        
        $statusGetMany = self::$db->getMany($this->keyArray, $recGetMany, $ttl, $option);
        $this->assertEquals($statusGetMany, AEROSPIKE::OK);
        $i= 0;
        
        foreach($recGetMany as $records_key=>$value){
                       
            $this->assertEquals($keyArray[$i]["key"], $value["key"]["key"]);
            $this->assertEquals($keyArray[$i]["set"], $value["key"]["set"]);
            $this->assertEquals($keyArray[$i]["ns"], $value["key"]["ns"]);
            $this->assertEquals($putArray[$i], $value["bins"]);
            $i++;
        }
        
    }
    
    function dpNormal001(){
        return array(
        "Verify getMany with NULL ttl and Timeout policy"=>
            array(
                array(
                    array("ns"=>"test", "set"=>"demo", "key"=>"myKey1"),
                    array("ns"=>"test", "set"=>"demo", "key"=>"myKey2"),
                    array("ns"=>"test", "set"=>"demo", "key"=>"myKey3"),
                    array("ns"=>"test", "set"=>"demo", "key"=>"myKey4"),
                    array("ns"=>"test", "set"=>"demo", "key"=>"myKey5"),
                ),
                array(
                    array("bin1"=>array("speaking", "reading", "writing")),
                    array("bin1"=>array("k1"=>10, "k2"=>5, "k3"=>6, "k4"=>7, "k5"=>8)),
                    array("array_bin"=>array()),
                    array("bin1"=>array(10, 20, "whatsup", array(1,2,"facebook", array("twitter", 100)), array("name"=>"aero", "age"=>23, "edu"=>array("twitter", 100), "skills"=>array("python", "c", "java",array("speaking", "reading", "writing"))))),
                    array("bin1"=>array("k1", new Employee(), array(array(12, new Employee(), new Employee()), new Employee(), " ")))
                ),
                NULL,
                array(Aerospike::OPT_READ_TIMEOUT=>3000)
            )
        );    
    }
    
     /**
     * @test
     * 
     */
    function normal_002() {
        $key4 = self::$db->initKey("test", "demo", "getMany4");
        $this->keys[] = $key4;
        $my_put_records = $this->put_records;
        $my_put_records[3] = NULL;
        $status = self::$db->getMany($this->keys, $records);
        $this->assertEquals($status, Aerospike::OK);
        $i = 0;
        foreach ($records as $records_key=>$value) {
            if ($i != 3) {
                $this->assertEquals($this->keys[$i]["key"], $value["key"]["key"]);
                $this->assertEquals($this->keys[$i]["set"], $value["key"]["set"]);
                $this->assertEquals($this->keys[$i]["ns"], $value["key"]["ns"]);
                $this->assertEquals($my_put_records[$i], $value["bins"]);
                $i++;
            }else{
                $this->assertNull($value["bins"]);
            }
        }
    }
    
    /**
     * @test
     */
    function normal_003() {
        $key4 = self::$db->initKey("test", "demo", "getMany4", true);
        $put_record = array("binD"=>50);
        self::$db->put($key4, $put_record);
        $this->keys[] = $key4;
        
        $my_keys = $this->keys + array($key4);
        $my_put_records = $this->put_records;
        $my_put_records[3] = $put_record;
        $status = self::$db->getMany($my_keys, $records);
        $this->assertEquals($status, AEROSPIKE::OK);
        
        $i = 0;
        
        foreach ($records as $records_key=>$value) {
            if ($i == 3){
                $this->assertNull($value["key"]["key"], NULL);
                $this->assertNotEquals($value["key"]["digest"], NULL);
                $this->assertEquals($my_keys[$i]["ns"], $value["key"]["ns"]);
                $this->assertEquals($my_keys[$i]["set"], $value["key"]["set"]);
                $this->assertEquals($my_put_records[$i], $value["bins"]);
            }
            
             else{
                $this->assertEquals($value["key"]["key"], $value["key"]["key"]);
                $this->assertEquals($my_keys[$i]["ns"], $value["key"]["ns"]);
                $this->assertEquals($my_keys[$i]["set"], $value["key"]["set"]);
                $this->assertEquals($my_put_records[$i], $value["bins"]);
                
            }
            
            $i++;
        }
        
    }

     /**
     * @test
     */
    function normal_004() {
        $status = self::$db->getMany($this->keys, $records, array("binA", "binC"));
        $this->assertEquals($status, AEROSPIKE::OK);
        $i=0;
        foreach ($records as $records_key=>$value) {
            $this->assertEquals($this->keys[$i]["key"], $value["key"]["key"]);
            $this->assertEquals($this->keys[$i]["ns"], $value["key"]["ns"]);
            $this->assertEquals($this->keys[$i]["set"], $value["key"]["set"]);
            $this->assertEquals(count($value["bins"]), 2);
            $this->assertFalse(array_key_exists("binB", $value["bins"]));
            $i++;
        }
    }
    
     /**
     * @test
     */
    function normal_005() {
        $status = self::$db->getMany($this->keys, $records, array("binA", "binC"), array(Aerospike::OPT_READ_TIMEOUT=>3000));
        $this->assertEquals($status, AEROSPIKE::OK);
        $i=0;
        foreach ($records as $records_key=>$value) {
            $this->assertEquals($this->keys[$i]["key"], $value["key"]["key"]);
            $this->assertEquals($this->keys[$i]["ns"], $value["key"]["ns"]);
            $this->assertEquals($this->keys[$i]["set"], $value["key"]["set"]);
            $this->assertEquals(count($value["bins"]), 2);
            $this->assertFalse(array_key_exists("binB", $value["bins"]));
            $i++;
        }
    }

    /**
     * @test
     */
    function normal_006() {
        $status = self::$db->getMany($this->keys, $records, array());
        $this->assertEquals($status, AEROSPIKE::OK);
        $i=0;
        foreach ($records as $records_key=>$value) {
            $this->assertEquals($this->keys[$i]["key"], $value["key"]["key"]);
            $this->assertEquals($this->keys[$i]["ns"], $value["key"]["ns"]);
            $this->assertEquals($this->keys[$i]["set"], $value["key"]["set"]);
            $this->assertEquals($this->put_records[$i], $value["bins"]);
            $i++;
        }
    }

    /**
     * @test
     */
    function normal_007() {
        $status = self::$db->getMany($this->keys, $records, array("random_bin"));
        $this->assertEquals($status, AEROSPIKE::OK);
        $i=0;
        foreach ($records as $records_key=>$value) {
            $this->assertEquals($this->keys[$i]["key"], $value["key"]["key"]);
            $this->assertEquals($this->keys[$i]["ns"], $value["key"]["ns"]);
            $this->assertEquals($this->keys[$i]["set"], $value["key"]["set"]);
            $this->assertEquals(count($value["bins"]), 0);
            $i++;
        }
        return $status;
    }
    
    /**
     * @test
     */
    function normal_008() {
        $records = "";
        $status = self::$db->getMany($this->keys, $records);
        $this->assertEquals($status, AEROSPIKE::OK);
        $i = 0;
        foreach ($records as $records_key=>$value) {
            $this->assertEquals($this->keys[$i]["key"], $value["key"]["key"]);
            $this->assertEquals($this->keys[$i]["ns"], $value["key"]["ns"]);
            $this->assertEquals($this->keys[$i]["set"], $value["key"]["set"]);
            $this->assertEquals($this->put_records[$i], $value["bins"]);
            $i++;
        }
    }

    /**
     * @test
     */
    function edge_001() {
        $keys = array();
        $status = self::$db->getMany($keys, $records, NULL, array(Aerospike::OPT_READ_TIMEOUT=>3000));
        $this->assertEquals($status, AEROSPIKE::OK);
        $this->assertEquals($records, array());
    }

    /**
     * @test
     */
    function edge_002() {
        try {
            $status = self::$db->getMany("", $records);
        } catch (Exception $e) {
            $status = Aerospike::ERR_PARAM;
        }
        $this->assertEquals($status, Aerospike::ERR_PARAM);
    }
    
    /**
     * @test
     */
    function edge_003() {
        try {
            $status = self::$db->getMany();
        } catch (Exception $e) {
            $status =  Aerospike::ERR_PARAM;
        }
        $this->assertEquals($status, Aerospike::ERR_PARAM);
    }
    
    /**
     * @test
     */
    function edge_004() {
        try {
            $status = self::$db->getMany($this->keys, $records, NULL, 12);
        } catch (Exception $e) {
            $status = Aerospike::ERR_PARAM;
        }
        $this->assertEquals($status, Aerospike::ERR_PARAM);
    }
    
    /**
     * @test
     */
    function bug_CLIENT_140() {
        $malformed_key = ["ns"=>"test", "set"=>"demo", "key"];
        $this->keys[] = $malformed_key;
        $my_keys = $this->keys;
        $status = self::$db->getMany($my_keys, $records);
        $this->assertEquals($status, AEROSPIKE::ERR_PARAM);
    }
}
