<?php
require_once 'Util.inc';

class poc extends PHPUnit_Framework_TestCase
{
    
   
    static $config = array("hosts"=>array(array("addr"=>"127.0.0.1", "port"=>"3000")));
    public $objKey = null;
    static $db = null;
    /**
    * @beforeClass
    */
    public static function getConnection(){
        self::$db = new Aerospike(self::$config);
        
    }
    

   public function initializeKey($varNs, $varSet, $varKey){
        $this->objKey = self::$db->initKey($varNs, $varSet, $varKey);
        self::$db->get($this->objKey, $getRecord);

        if(!empty($getRecord["bins"])){
            self::$db->remove($this->objKey);
        }
        $this->objKey = self::$db->initKey($varNs, $varSet, $varKey);
        return $this->objKey;
    }
    

    
    public function putData($putData){
        $this->stringKeyName=$putData[0];
        $this->valueToBeStored= $putData[1];
        $this->ttl = NULL;
        $this->flag = array();

        if(!empty($putData[2]))
        {
          $this->ttl = $putData[2];
        }

        if(!empty($putData[3]))
        {
          $this->flag = $putData[3];
        }
        
        $this->objKey= $this->initializeKey($putData[0][0],$putData[0][1],$putData[0][2]);
        $status = self::$db->put($this->objKey , $this->valueToBeStored,$this->ttl,$this->flag);
        $this->assertEquals($status, AEROSPIKE::OK);
        return $this->objKey;
    }
        
    public function setup(){
       
        
    }

    public function tearDown(){
        self::$db->remove($this->objKey);
        $status=self::$db->exists($this->objKey, $metadata);
        $this->assertEquals($status, Aerospike::ERR_RECORD_NOT_FOUND);
        
    }
    
     /**
     * @dataProvider dpNormal
     * @test
     */
    public function testNormal($dpNormal)
    {
        $this->objKey = $this->putData($dpNormal);
        $status = self::$db->get($this->objKey, $get_record);
        $this->assertEquals($status,Aerospike::OK);
        $this->assertEquals($get_record["key"]["ns"], $this->objKey["ns"]);
        $this->assertEquals($get_record["key"]["set"], $this->objKey["set"]);

        if(!empty($dpNormal[3])&&$dpNormal[3]==array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND)){
            $recordBin = null;
            foreach($dpNormal[1] as $key => $value) {
                $recordBin = $key;
            }
            $status = self::$db->get($this->objKey, $get_record, array($recordBin), $dpNormal[3]);
            $this->assertEquals($status, Aerospike::OK);
        }
        else{
            $status = self::$db->get($this->objKey, $get_record);
            $this->assertEquals($status, Aerospike::OK);
        }
      
        $this->assertEquals($get_record["key"]["ns"], $this->objKey["ns"]);
        $this->assertEquals($get_record["key"]["set"], $this->objKey["set"]);
      
        if(!empty($dpNormal[3])&&$dpNormal[3]==array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND)){
            $this->assertEquals($get_record["key"]["key"], $this->objKey["key"]);
        }
        else{
            $this->assertEquals($get_record["key"]["key"], NULL);
        }
      
        $this->assertEquals($dpNormal[1], $get_record["bins"]);
    }
    
     /**
     * @dataProvider dpNormalPHPSerialized
     * @test
     */
    public function testNormalPHPSerialized($dpNormal){
      
        $this->objKey = $this->putData($dpNormal);
        $status = self::$db->get($this->objKey, $get_record);
        $this->assertEquals($status,Aerospike::OK);

        $this->assertEquals($get_record["key"]["ns"], $this->objKey["ns"]);
        $this->assertEquals($get_record["key"]["set"], $this->objKey["set"]);

        if(!empty($dpNormal[3])&&$dpNormal[3]==array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND)){
            $recordBin = null;
            foreach($dpNormal[1] as $key => $value) {
                $recordBin = $key;
            }
            $status = self::$db->get($this->objKey, $get_record, array($recordBin), $dpNormal[3]);
            $this->assertEquals($status, Aerospike::OK);        
        }
        else{
            $status = self::$db->get($this->objKey, $get_record);
            $this->assertEquals($status, Aerospike::OK);
        }
      
        $this->assertEquals($get_record["key"]["ns"], $this->objKey["ns"]);
        $this->assertEquals($get_record["key"]["set"], $this->objKey["set"]);
      
        if(!empty($dpNormal[3])&&$dpNormal[3]==array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND)){
            $this->assertEquals($get_record["key"]["key"], $this->objKey["key"]);
        }
        
        
        else{
            $this->assertEquals($get_record["key"]["key"], NULL);
        }
        $this->assertEquals($dpNormal[1], $get_record["bins"]);
 
     
    }

    public function testErrorSerializationUdfToPhp(){
        $this->objKey = self::$db->initKey("test", "demo", "map_list_objects_with_UDF_serializer");

         #Set Serializer
        Aerospike::setSerializer(function ($val) {
            if (is_bool ($val)) {
                return "b||". serialize($val);
            }
            if (is_object ($val)) {
                return "o||". serialize($val);
            }
            if (is_double ($val)) {
                return "d||". serialize($val);
            }
            return "r||". $val;
        });

        #Creating objects
        $obj1 = new Employee();
        $obj2 = new Employee();
        $obj3 = new Employee();
        $obj4 = new Employee();

        $list = array(12, $obj2, 63.2, " ", $obj4, FALSE);
        $map = array("k1"=>$obj3, "k8"=>TRUE, 56=>$list, $obj1, "k22"=>56.75);
        $put_record = array("bin1"=>$map);
        $status = self::$db->put($this->objKey , $put_record, NULL, array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
        $this->assertEquals($status, Aerospike::OK);
        $status = self::$db->get($this->objKey , $get_record, array("bin1"));
        #Expected status should not be OK as desrializer is not set. Here OK is returned. This TC is failing.
        $this->assertNotEquals($status, Aerospike::OK);
        $this->assertEquals($get_record["key"]["ns"], $this->objKey["ns"]);
        $this->assertEquals($get_record["key"]["set"], $this->objKey["set"]);
        $this->assertEquals($get_record["key"]["key"], NULL);
        $this->assertEquals($put_record, $get_record["bins"]);
    } 
    
    /**
     * @dataProvider dpNormalUDFSerialized
     * @test
     */
    public function testNormalUDFSerialized($dpNormal)
    {
        Aerospike::setSerializer(function ($val) {
            if (is_bool ($val)) {
                return "b||". serialize($val);
            }
            if (is_object ($val)) {
                return "o||". serialize($val);
            }
            if (is_double ($val)) {
                return "d||". serialize($val);
            }
            return "r||". $val;
        });
     
        #Set Deserializer
        Aerospike::setDeserializer(function ($val) {
            $prefix = substr($val, 0, 3);
            if ($prefix !== 'r||') {
                return unserialize(substr ($val, 3));
            }
            return unserialize(substr ($val, 3));
        });
        
        $this->objKey = $this->putData($dpNormal);
        $status = self::$db->get($this->objKey, $get_record);
        $this->assertEquals($status, Aerospike::OK);
        $this->assertEquals($get_record["key"]["ns"], $this->objKey["ns"]);
        $this->assertEquals($get_record["key"]["set"], $this->objKey["set"]);

        if(!empty($dpNormal[3])&&$dpNormal[3]==array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND)){
            $this->assertEquals($get_record["key"]["key"], $this->objKey["key"]);
        }
        else{
            $this->assertEquals($get_record["key"]["key"], NULL);
        }
        $this->assertEquals($dpNormal[1], $get_record["bins"]);

    }  
    
    
    public function testErrorSerializationPhpToUdf(){
        $this->objKey = self::$db->initKey("test", "demo", "list_of_objects_with_PHP_SERIALIZER_USER_deserializer");
        $list = array("k1", new Employee(), array(array(12, new Employee(), new Employee()), new Employee(), " "));
        $put_record = array("bin1"=>$list);
        $status = self::$db->put($this->objKey, $put_record, NULL, array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
        $this->assertEquals($status, Aerospike::OK);
        
        #Set Deserializer
        Aerospike::setDeserializer(function ($val) {
            
            $prefix = substr($val, 0, 3);
            if ($prefix !== 'r||') {
                return unserialize(substr ($val, 3));
            }
            return unserialize(substr ($val, 3));
        });
     
        $status = self::$db->get($this->objKey, $get_record, array("bin1"));
        $this->assertEquals($status, Aerospike::OK);
        $this->assertEquals($get_record["key"]["ns"], $this->objKey["ns"]);
        $this->assertEquals($get_record["key"]["set"], $this->objKey["set"]);
        $this->assertEquals($get_record["key"]["key"], NULL);
        $this->assertEquals($put_record, $get_record["bins"]);
    } 
    
    

    /**
     * @dataProvider dpErrorNotExistingVal
     * @test
     */
    public function testErrorNotExistingValues($dpError){
        $this->objKey = self::$db->initKey("test", "demo", "myKey");
        $status = self::$db->put($this->objKey, array("bin1"=>array("speaking", "reading", "writing")));
        $status = self::$db->get($dpError[0], $get_record, $dpError[1]);
        $this->assertEquals($status,$dpError[2]);
    
    }
    
function generateRandomString($length = 10) {
        $characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
        $charactersLength = strlen($characters);
        $randomString = '';
        for ($i = 0; $i < $length; $i++) {
            $randomString .= $characters[rand(0, $charactersLength - 1)];
        }
        return $randomString;
    }

public function dpErrorNotExistingVal(){
        return array(   
            'To verify error response code for get() method with non-existing ns. testCheckNameSpaceValueNotExistInDB' => 
                array(
                    array( 
                        array("ns"=>$this->generateRandomString(),"set"=>"demo","key"=>"myKey"),
                        array("bin1"),
                        Aerospike::ERR_NAMESPACE_NOT_FOUND
                    )
                ),
            'To verify error response code for get() method with non-existing set. testCheckSetValueNotExistInDB' => 
                array(
                    array( 
                        array("ns"=>"test","set"=>$this->generateRandomString(),"key"=>"myKey"),
                        array("bin1"),
                        Aerospike::ERR_RECORD_NOT_FOUND
                    )
                ),
            'To verify error response code for get() method with non-existing key. testCheckKeyValueNotExistInDB' => 
                array(
                    array( 
                        array("ns"=>"test","set"=>"demo","key"=>$this->generateRandomString()),
                        array("bin1"),
                        Aerospike::ERR_RECORD_NOT_FOUND
                    )
                ),
            'To verify error response code for get() method with non-existing bin. testCheckKeyValueNotExistInDB' => 
                array(
                    array( 
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array($this->generateRandomString()),
                        Aerospike::OK
                    )
                ),


            'To verify error response code for get() method with invalid bin name. testCheckKeyValueNotExistInDB' => 
                array(
                    array( 
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("0000000000fadsflk"),
                        Aerospike::ERR_PARAM
                    )
                ),
        );
        
    }
    
    
    
    
    public function dpNormalUDFSerialized()
       {
        return array(   
        'To verify get() method with Nested list of objects using UDF serializer. testGetNestedListOfObjectsWithUDFSerializer' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("k1", new Employee(), array(array(12, new Employee(), new Employee()), new Employee(), " "))
                    ),
                    2000,
                    array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),
             
        'To verify get() method with list of map of objects using UDF serializer. testGetListMapObjectsWithUDFSerializer' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("k1", new Employee(), array(12=>new Employee(), new Employee(), "aa"=>new Employee()), 55)
                    ),
                    2000,
                    array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),
             
        'To verify get() method with map of list of objects using UDF serializer. testGetMapListObjectsWithUDFSerializer' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("k1"=>new Employee(), new Employee(), 56=>array(12, new Employee(), 56, " ", new Employee()))
                    ),
                    2000,
                    array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),
             
        'To verify get() method with nested list of boolean using UDF serializer. testGetNestedListOfBoolsWithUDFSerializer' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("k1", 56, array(array(12, TRUE), " "), FALSE)
                    ),
                    2000,
                    array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),   
             
        'To verify get() method with nested map of boolean using UDF serializer. testGetNestedMapOfBoolsWithUDFSerializer' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("k1", TRUE, 56=>array("k1", TRUE, 56=>array(array(12=>TRUE, FALSE), "K5"=>" ")))
                    ),
                    2000,
                    array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),   
             
        'To verify get() method with list of map of boolean using UDF serializer. testGetListMapBoolsWithUDFSerializer' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("k1", new Employee(), array(12=>new Employee(), FALSE, new Employee(), "aa"=>new Employee()), 55=>TRUE)
                    ),
                    2000,
                    array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),   
             
        'To verify get() method with map of list of boolean using UDF serializer. testGetMapListBoolsWithUDFSerializer' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("k1"=>new Employee(), "k8"=>TRUE, 56=>array(12, new Employee(), 56, " ", new Employee(), FALSE), new Employee())
                    ),
                    2000,
                    array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),   
             
        'To verify get() method with nested list of floats using UDF serializer. testGetNestedListOfFloatsWithUDFSerializer' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("k1", TRUE, 56=>array(array(12=>89.4, 55.44), "K5"=>" "))
                    ),
                    2000,
                    array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ), 
             
        'To verify get() method with list of map of float using UDF serializer. testGetListMapFloatsWithUDFSerializer' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("k1", new Employee(), array(12=>new Employee(), new Employee(), "aa"=>25.6, "k10"=>TRUE), 55=>89.56, FALSE, 45.78)
                    ),
                    2000,
                    array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                )
            
                        
            );
       }
    public function dpNormal()
       {
         return array(
            'To verify get() method with list of String values. testCheckListInsert' => 
                array(
                    array(
                        array("test", "demo","myKey"),
                        array(
                            "bin1"=>array("speaking", "reading", "writing")
                        )
                    )
                ),
             
            'To verify get() method with list of Map values. testCheckMapInsert' => 
                array(
                    array(
                        array("test", "demo","myKey"),
                        array(
                            "bin1"=>array("k1"=>10, "k2"=>5, "k3"=>6, "k4"=>7, "k5"=>8)
                        )
                    )
                ),
        
        'To verify get() method with empty array. testEmptyMapToEmptyArray' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "array_bin"=>array()
                    )
                )
            ),
             
        'To verify get() method with combination of list and map. testCheckListMapCombineInsert. ' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array(10, 20, "whatsup", array(1,2,"facebook", array("twitter", 100)), array("name"=>"aero", "age"=>23, "edu"=>array("twitter", 100), "skills"=>array("python", "c", "java",array("speaking", "reading", "writing"))))
                    )
                )
            ),
             
        
             
          'To verify get() method with POLICY_KEY_SEND policy. testGetWithPolicyKeySend' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("k1", new Employee(), array(array(12, new Employee(), new Employee()), new Employee(), " "))
                    ),
                    2000,
                    array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND)
                )
            ),
             
        'To verify get() method with POLICY_KEY_SEND policy alongwith OPT_READ_TIMEOUT option. testGetWithPolicyKeySendAndReadTimeout' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("k1", new Employee(), array(array(12, new Employee(), new Employee()), new Employee(), " "))
                    ),
                    2000,
                    array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND, Aerospike::OPT_WRITE_TIMEOUT=>2000)
                )
            ),
        );
      }
      
    public function dpNormalPHPSerialized()
    {
        return array('To verify get() method with nested list of objects using PHP serializer. testGetNestedListOfObjectsWithPHPSerializer' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "array_bin"=>array("k1", new Employee(), array(array(12, new Employee(), new Employee()), new Employee(), " "))
                    ),
                    2000, 
                    array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                )
     
            ),
             
         'To verify get() method with nested map of objects using PHP serializer. testGetNestedMapOfObjectsWithPHPSerializer' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("k1", new Employee(), 56=>array(array(12=>new Employee(), new Employee()), new Employee(), " "))
                    ),
                    2000, 
                    array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                )
     
            ),
             
           'To verify get() method with list of map of objects using PHP serializer. testGetListMapObjectsWithPHPSerializer' => 
            array(  
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("bin1"=>array("k1", new Employee(), array(12=>"k5", "k78"=>new Employee(), new Employee())))
                    ),
                    2000, 
                    array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                )
     
            ),
             
            'To verify get() method with map of list of objects using PHP serializer. testGetMapListObjectsWithPHPSerializer' => 
            array(  
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("k1", new Employee(), new Employee(), array(12=>"k5", "k78"=>new Employee()))
                    ),
                    2000, 
                    array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                )
     
            ),
             
            'To verify get() method with nested list of booleans using PHP serializer. testGetNestedListOfBoolsWithPHPSerializer' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("k1", 56, array(array(12, TRUE), " "), FALSE)
                   ),
                   2000, 
                   array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                )
     
            ),
             
            'To verify get() method with list of map of booleans using PHP serializer. testGetListMapBoolsWithPHPSerializer' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("bin1"=>array("k1", new Employee(), array(12=>new Employee(), FALSE, new Employee(), "aa"=>new Employee()), 55=>TRUE))
                    ),
                    2000, 
                    array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                )
            ),
             
            'To verify get() method with map of list booleans using PHP serializer. testGetMapListBoolsWithPHPSerializer' => 
            array(
                array(
                    array("test", "demo","myKey"),
                    array(
                        "bin1"=>array("k1"=>new Employee(), "k8"=>TRUE, 56=>array(12, new Employee(), 56, " ", new Employee(), FALSE), new Employee())
                    ),
                    2000, 
                    array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                )
     
            ),
             
            'To verify get() method with nested list of floats using PHP serializer. testGetNestedListOfFloatsWithPHPSerializer' => 
            array(
                    
                        array(array("test", "demo","myKey"),
                          array(
                               "bin1"=>array("k1", 56, array(array(12, 11.2), " "), 56.896)
                          ),
                          2000, 
                          array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                        )
     
                ),
             
             'To verify get() method with nested map of float using PHP serializer. testGetNestedMapOfFloatsWithPHPSerializer' => 
            array(
                    
                        array(array("test", "demo","myKey"),
                          array(
                               "bin1"=>array("k1", TRUE, 56=>array(array(12=>89.4, 55.44), "K5"=>" "))
                          ),
                          2000, 
                          array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                        )
     
                ),
             'To verify get() method with list of map of floats using PHP serializer. testGetListMapFloatsWithPHPSerializer' => 
            array(
                    
                        array(array("test", "demo","myKey"),
                          array(
                               "bin1"=>array("k1", new Employee(), array(12=>new Employee(), new Employee(), "aa"=>25.6, "k10"=>TRUE), 55=>89.56, FALSE, 45.78)
                          ),
                          2000, 
                          array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                        )
     
                ),
             
             'To verify get() method with map of list of floats using PHP serializer. testGetMapListFloatsWithPHPSerializer' => 
            array(
                    
                        array(array("test", "demo","myKey"),
                          array(
                               "bin1"=>array("k1"=>new Employee(), "k8"=>TRUE, 56=>array(12, new Employee(), 63.2, " ", new Employee(), FALSE), new Employee(), "k22"=>56.75)
                          ),
                          2000, 
                          array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                        )
     
                ),
             'To verify get() method with map of booleans keys using PHP serializer. testGetMapOfBoolsKeyIsBool' => 
            array(
                    
                        array(array("test", "demo","myKey"),
                          array(
                               "bin1"=>array("k1", TRUE=>new Employee(), new Employee())
                          ),
                          2000, 
                          array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                        )
     
                ),
             'To verify get() method with map of float keys using PHP serializer. testGetMapOfFloatsKeyIsFloat' => 
            array(
                    
                        array(array("test", "demo","myKey"),
                          array(
                               "bin1"=>array("bin1"=>array("k1", 78.5=>56.35, new Employee()))
                          ),
                          2000, 
                          array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                        )
     
                      ),
     );
   }    
    
 
}
?>