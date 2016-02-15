<?php
require_once 'Util.inc';

class InitKey extends PHPUnit_Framework_TestCase{
    static $config = array("hosts"=>array(array("addr"=>"127.0.0.1", "port"=>"3000")));
    public $objKey = null;
    static $db = null;
    
    /**
    * @beforeClass
    */
    public static function getConnection(){
        self::$db = new Aerospike(self::$config);
    }
    
    /**
    * @dataProvider dpInitKey
    * @test
    */
    function testInitKey($dpError, $expectedStatus)
    {
        if($expectedStatus==='PASS'){
            if(!empty($dpError[3])){
                $this->objKey = self::$db->initKey($dpError[0], $dpError[1], $dpError[2], $dpError[3]);
                $this->assertNotNull($this->objKey);
            }
            else if(!empty($dpError[2])){
                $this->objKey = self::$db->initKey($dpError[0], $dpError[1], $dpError[2]);
                $this->assertNotNull($this->objKey);
            }
            else if(!empty($dpError[1])){
                $this->objKey = self::$db->initKey($dpError[0], $dpError[1]);
                $this->assertNotNull($this->objKey);
            }
        } else {
            if(!empty($dpError[3])){
                $this->objKey = self::$db->initKey($dpError[0], $dpError[1], $dpError[2], $dpError[3]);
                $this->assertNull($this->objKey);
            }
            else if(!empty($dpError[2])){
                $this->objKey = self::$db->initKey($dpError[0], $dpError[1], $dpError[2]);
                $this->assertNull($this->objKey);
            }
            else if(!empty($dpError[1])){
                $this->objKey = self::$db->initKey($dpError[0], $dpError[1]);
                $this->assertNull($this->objKey);
            }
        }
    }

    /**
    * @after
    */
    function verifyValue(){
        if(!empty($this->objKey)){
            self::$db->remove($this->objKey);
        }
    }
    
    function dpInitKey(){
        return array(
            'TC_put_001: To verify init key with integer key. testInitKeyHelper' => 
                array(
                    array(
                        "demo", "test", 10000
                    ),
                    'PASS'
                ),
            
            'TC_put_002: To verify init key with integer namespace. testNameSpaceValueInt' => 
                array(
                    array(
                        2312312, "test", 10000
                    ),
                    'PASS'
                ),
            
            'TC_put_003: To verify init key with integer set. testSetValueInt' => 
                array(
                    array(
                        "demo", 12312312, 10000
                    ),
                    'PASS'
                ),
            
            'TC_put_004: To verify init with key digest. testInitKeyDigest' => 
                array(
                    array(
                        "test", "demo", base64_decode("test_init_key"), true
                    ),
                    'PASS'
                ),
            
            'TC_put_005: To verify init with key value. testInitKeyDigestExceedingMaxLen' => 
                array(
                    array(
                        "test", "demo", base64_decode("test_init_key_with_exceeding_max_length"), true
                    ),
                    'FAIL'
                ),
            
            'TC_put_006: To verify init with name space parameter. testCheckNameSpaceParameterMissingInKeyArray' => 
                array(
                    array(
                        "set"=>"demo", "key"=>"10000"
                    ),
                    'FAIL'
                ),
            
            'TC_put_007: To verify init with set parameter. testCheckSetParameterMissingInKeyArray' => 
                array(
                    array(
                        "ns"=>"test", "key"=>"10000"
                    ),
                    'FAIL'
                ),
            
            'TC_put_008: To verify init with key value. testCheckParameterSequenceChangeInKeyArray' => 
                array(
                    array(
                        "demo", "test", "key_int1"
                    ),
                    'PASS'
                ),
            
            'TC_put_009: To verify init with non-existent set value. testCheckSetValueNotExistInDB' => 
                array(
                    array(
                        "test", "ssss---ssss", "key_int1"
                    ),
                    'PASS'
                ),
            
            'TC_put_010: To verify init when namespace does not exist. testCheckNameSpaceValueNotExistInDB' => 
                array(
                    array(
                        "sss--ss", "demo", "key_int1"
                    ),
                    'PASS'
                ),
                
            'TC_put_011: To verify init when namespace does not exist. testPUT' => 
                array(
                    array(
                        "sss--ss", "demo", "key_int1"
                    ),
                    'PASS'
                ),
        );
    }
}
?>
