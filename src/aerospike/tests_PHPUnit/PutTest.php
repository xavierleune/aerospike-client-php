<?php
require_once 'Util.inc';


class Put extends PHPUnit_Framework_TestCase{
    
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
    
    /**
    * @before
    */
    function initializeKey(){
        $this->objKey = self::$db->initKey("test", "demo", "put_test");
        self::$db->remove($this->objKey);
        $this->objKey = self::$db->initKey("test", "demo", "put_test");
        
    }
    
    /**
    * @dataProvider dpNormal
    * @test
    */
    function testNormalOnly($dpNormal)
    {
        
        $this->objKey = $dpNormal[0];
        
        //todo: enhancement. make index 2 and 3 optional
        $status = self::$db->put($dpNormal[0], $dpNormal[1], $dpNormal[2], $dpNormal[3]);
        $this->assertEquals($status,0);
        $status = self::$db->get($this->objKey, $returned);
        $this->assertEquals($status,0);
        
        $this->assertEquals($returned['bins'], $dpNormal[1]);
        
        
    }
    
        
    function dpNormal(){
        
        return array(   
            
            'To verify put with string value. testPUT' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>"Hello World"),
                        NULL,
                        array()                        
                    )
                ),
            
            ' To verify put with string value using multiple bins. testMultiPUT' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>"Hello World", "bin2"=>123456),
                        NULL,
                        array()
                    )
                ),
            
            'To verify put with string value with UNICODE character. testPUTWithUnicodeCharacter' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>utf8_encode("Hello World")),
                        NULL,
                        array()
                    )
                ),
            
                      
            'To verify put with list of string value, ttl and option. testCheckListInsert' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("speaking", "reading", "writing")),
                        NULL,
                        array()
                    )    
                ),
            
            'To verify put with map of string value, ttl and option. testCheckMapInsert' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1"=>10,"k2"=>5,"k3"=>6,"k4"=>7,"k5"=>8)),
                        NULL,
                        array()
                    )       
                ),
            
            'To verify put with an empty array. testPutEmptyArrayIntoBin' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("array_bin"=>array()),
                        NULL,
                        array()
                    ) 
                ),
            
            'To verify put with nested list. testNestedList' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array(10, 20, "whatsup", array(1,2,"facebook", array("twitter", 100)), array("name"=>"aero", "age"=>23, "edu"=>array("degree"=>array("month"=>"May", "year"=>2013), "aggregate"=>70), "skills"=>array("python", "c", "java",array("speaking", "reading", "writing"))))),
                        NULL,
                        array()
                    ) 
                ),
            
            'To verify put with combination of map and list. testCheckListMapCombineInsert' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1" => array(46,array(array("aa"),1,"kk"))),
                        NULL,
                        array()
                    )
                ),
            
            'To verify put with int and list of string. testIntlistStringlist' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("bin1" => array(100,566,52,array("aa","vv","hh")))),
                        NULL,
                        array()
                    )
                ),
            
            'To verify put with map of int value. testMapWithIntval' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array(1,"k6"=>"abc",array("k10"=>10))),
                        NULL,
                        array()
                    )
                ),
            
            'To verify put with Integer, string and list value. testIntStringList' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array(3,"k1","k4"=>array("aa",2))),
                        NULL,
                        array()
                    )
                ),
            
            'To verify put with Associative array. testPutAssocArray' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k10"=>89,85)),
                        NULL,
                        array()
                    )
                ),
            
            'To verify put with NULL record. testPutNullDataRecordPositive' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array('null_bin' => NULL),
                        NULL,
                        array()
                      
                    )
                ),
            
            'To verify put with map, having value as "NULL". testPutNullDataMapPositive' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array('my_bin' => array('null_data' => NULL)),
                        NULL,
                        array()
                      
                    )
                ),
            
            'To verify put with a list having NULL as an element. testPutNullDataListPositive' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array('my_bin' => array(1, 'hello', NULL)),
                        NULL,
                        array()
                      
                    )
                ),
            
            
            'To verify put with mixed datatype. testPutNullDataMixedPositive' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array('my_data' => array(1, array('age' => 20,'job' => NULL, 'salary' => NULL,'qualifications' => array('B.Tech', 'M.Tech', NULL)),'hello', NULL), 'no_data' => NULL),
                        NULL,
                        array()
                      
                    )
                ),
            
            

            );
    }
    
    
    /**
    * @dataProvider dpErrorNoneSerializer
    * @test
    */
    function testErrorSerializerNone($dpNormal)
    {
        $this->objKey = $dpNormal[0];
        //todo: make index 2 and 3 optional
        $status = self::$db->put($dpNormal[0], $dpNormal[1], $dpNormal[2], $dpNormal[3]);
        $this->assertEquals($status,AEROSPIKE::ERR_PARAM);
        
    }
    
    function dpErrorNoneSerializer(){
        return array(
            'To verify put with list of objects with serializer_none. testPutListOfObjectsSerializerNone' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", new Employee(), array(array(12, new Employee(), new Employee()), new Employee()))),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_NONE)
                        )
                    ),
            
            'To verify put with map of objects with serializer_none testPutMapOfObjectsSerializerNone' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1"=>12, new Employee(), "k4"=>array(array(12=>new Employee(), new Employee()), "k45"=>new Employee()))),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_NONE)
                        )
                    ),
            
            'To verify put with list of boolean with serializer_none. testPutListOfBoolsSerializerNone' => 
                array(
                       array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", TRUE, array(array(12, TRUE), FALSE, 89))),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_NONE)
                      )
                     ),
            
            'To verify put with map of booleans with serializer_none. testPutMapOfBoolsSerializerNone' => 
                array(
                       array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1"=>12, 89, "k4"=>array(array(12=>TRUE, "k2"), "aa"=>FALSE))),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_NONE)
                      )
                     ),
            
            'To verify put with map of floats with serializer_none. testPutMapOfFloatsSerializerNone' => 
                array(
                       array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1"=>12, 89, "k4"=>array(array(12=>89.4, "k2"), "k8"=>17.1))),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_NONE)
                     ) 
                    ),
            

        );
        
    }
    
    function testPutMapOfBoolsKeyIsBool() {
     $key = self::$db->initKey("test", "demo", "map_of_bools_key_is_bool");

     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();

     $map = array("k1", TRUE=>$obj2, $obj1);
     $status = self::$db->put($key, array("bin1"=>$map), NULL,
         array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
        
 }
    
    
    
    /**
    * @dataProvider dpNormalPhpSerialized
    * @test
    */
    function testNormalPhpSerialized($dpNormal)
    {
        
        $this->objKey = $dpNormal[0];
        $status = self::$db->put($this->objKey, $dpNormal[1], $dpNormal[2], $dpNormal[3]);
        $this->assertEquals($status, AEROSPIKE::OK);
        $status = self::$db->get($this->objKey, $returned);
        $this->assertEquals($status,AEROSPIKE::OK);
        $this->assertEquals($returned['bins'], $dpNormal[1]);
        
    }
    
     
    function dpNormalPhpSerialized(){
        
        return array( 
            'To verify put with mixed datatype using default serializer. testMixedTypesWithDefaultSerializer' => 
                array(
                     array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("_string" => "A string value","_integer" => 101,"_boolean" => true,"_float" => 6.969,"_object" => new Employee(),"_list" => array("A", "Word", "to", "the", "Wise"),"_map" => array("foo" => "bar", "boo" => "hoo")),
                        10,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                        )
                     ),
            

            
            'To verify put with nested list of objects using PHP serializer. testPutNestedListOfObjectsWithPHPSerializer' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", new Employee(), array(array(12, new Employee(), new Employee()), new Employee(), " "))),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                        )
                    ),
            

            
            'To verify put with nested map of objects with PHP serializer. testPutNestedMapOfObjectsWithPHPSerializer' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", new Employee(), 56=>array(array(12=>new Employee(), new Employee()), new Employee(), " "))),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                        )
                    ),
            
            'To verify put with list of map objects with PHP serializer. testPutListMapObjectsWithPHPSerializer' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", new Employee(), array(12=>"k5", "k78"=>new Employee(), new Employee()))),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                        )
                    ),
            
            'To verify put with list of map with PHP serializer . testPutMapListObjectsWithPHPSerializer' => 
                array(
                       array( 
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1"=>new Employee(), new Employee(), 56=>array(12, new Employee(), 56, " ", new Employee()))),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                       )
                     ),
            

            'To verify put with nested list of booleans with PHP serializer. testPutNestedListOfBoolsWithPHPSerializer' => 
                array(
                       array( 
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", 56, array(array(12, TRUE), " "), FALSE)),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                      )
                     ),
            

            
            'To verify put with nested map of booleans with PHP serializer. testPutNestedMapOfBoolsWithPHPSerializer' => 
                array(
                       array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", TRUE, 56=>array(array(12=>TRUE, FALSE), "K5"=>" "))),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                      )
                     ),
            
            'To verify put with list of map of booleans. testPutListMapBoolsWithPHPSerializer' => 
                array(
                       array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", new Employee(), array(12=>new Employee(), FALSE, new Employee(), "aa"=>new Employee()), 55=>TRUE)),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                      )
                     ),
            
            'To verify put with map of list of booleans. testPutMapListBoolsWithPHPSerializer' => 
                array(
                       array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", new Employee(), array(12=>new Employee(), FALSE, new Employee(), "aa"=>new Employee()), 55=>TRUE)),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                      )
                     ),
            
                 
            'To verify put with map having boolean key. testPutMapOfBoolsKeyIsBool' => 
                array(
                       array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", TRUE=>new Employee(), new Employee())),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                      )
                     ),
            
            'To verify put with list of floats with PHP serializer. ' => 
                array(
                       array( 
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1"=>new Employee(), "k8"=>TRUE, 56=>array(12, new Employee(), 56, " ", new Employee(), FALSE), new Employee())),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                      )
                     ),
            
            'To verify put with list of floats using serializer_php. ' => 
                array(
                       array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),  
                        array("bin1"=>array("k1", array(array(12.23, TRUE), 56, 89.2))),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                      )
                     ),
            
            'To verify put with nested list of float with PHP serializer. testPutNestedListOfFloatsWithPHPSerializer' => 
                array(
                       array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", 56, array(array(12, 11.2), " "), 56.896)),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                     ) 
                    ),
            
            'To verify put with nested map of float with PHP serializer. testPutNestedMapOfFloatsWithPHPSerializer' => 
                array(
                       array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", TRUE, 56=>array(array(12=>89.4, 55.44), "K5"=>" "))),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                     )
                    ),
            
            'To verify put with list of map of float value with PHP serializer. testPutListMapFloatsWithPHPSerializer' => 
                array(
                       array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", new Employee(), array(12=>new Employee(), new Employee(), "aa"=>25.6, "k10"=>TRUE), 55=>89.56, FALSE, 45.78)),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
                     )
                    )
            

            );
    }
    
      
    /**
    * @dataProvider dpGenPolicy
    * @test
    */
    function testGenPolicy($objKey, $dpNormalGenPolicy, $assertion)
    {
      
       $this->objKey = $objKey;
       $put_status =self::$db->put(array("ns"=>"test","set"=>"demo","key"=>"myKey"), array("bin1"=>5));
       $exists_status = self::$db->exists(array("ns"=>"test","set"=>"demo","key"=>"myKey"), $metadata);
       $this->gen_value = $metadata["generation"];
       $this->putValue =  $dpNormalGenPolicy[0];
       $status = self::$db->put($this->objKey, $dpNormalGenPolicy[0], $dpNormalGenPolicy[1], $dpNormalGenPolicy[2]);
       $exists_status = self::$db->exists($this->objKey, $metadata);
       $this->gen_value = $metadata["generation"];
       
       $status = self::$db->get($this->objKey, $returned);
              
       if($assertion==="Fail"){
            if(isset($returned['bins'])){
                $this->assertNotEquals($returned['bins'], $dpNormalGenPolicy[0]);
            }
            else{
                $this->assertEmpty($returned);
            }
        }
        
        else{
            $this->assertEquals($returned['bins'], $dpNormalGenPolicy[0]);
        }
    }
    
    function dpGenPolicy(){
        return array(   
            'To verify put with policy_gen_ignore policy, positive test case. testPutGenPolicyIgnoreWithGenValPositive' => 
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array(
                            array("bin1"=>"Hello World"),
                            2000,
                            array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_IGNORE)),
                            ),
                        'Pass'
                        ),
            
            'To verify put with POLICY_GEN_EQ policy, negative test case. testPutGenPolicyEQWithGenValPositive' => 
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array(
                            array("bin1"=>"Hello World"),
                            2000,
                            array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_EQ, $this->gen_value))
                            ),
                        'Fail'
                        ),
            'To verify put with POLICY_GEN_GT policy, positive test case. testPutGenPolicyGTWithGenValPositive' => 
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array(
                            array("bin1"=>"Hello World"),
                            2000,
                            array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT, ($this->gen_value+5)))
                            ),
                        'Pass'
                        ),
            
            'To verify put with POLICY_GEN_EQ, positive test case. testPutGenPolicyEQPositive' => 
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array(
                            array("bin1"=>"Hello World"),
                            2000,
                            array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_EQ, ($this->gen_value+1)))
                            ),
                        'Pass'
                        ),
            
            'To verify put with POLICY_GEN_GT policy, positive test case. testPutGenPolicyGTPositive' => 
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array(
                            array("bin1"=>"Hello World"),
                            2000,
                            array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT, ($this->gen_value+2)))
                            ),
                        'Pass'
                        ),
            
            'To verify put with POLICY_EXISTS_CREATE policy, negative test case. testPutGenPolicyExistsCreatePositive' => 
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array(
                            array("bin1"=>"Hello World"),
                            2000,
                            array(Aerospike::OPT_POLICY_EXISTS=>Aerospike::POLICY_EXISTS_CREATE )
                            ),
                        'Fail'
                        ),
            
            'To verify put with POLICY_EXISTS_CREATE, positive test case. testPutGenPolicyExistsCreateNegative' => 
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myAnotherKey"),
                        array(
                            array("bin1"=>"Hello World"),
                            2000,
                            array(Aerospike::OPT_POLICY_EXISTS=>Aerospike::POLICY_EXISTS_CREATE )
                            ),
                        'Pass'
                        ),
            
                        
            'To verify put with POLICY_EXISTS_UPDATE, positive test case. testPutGenPolicyExistsUpdatePositive' => 
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array(
                            array("bin1"=>"Hello World"),
                            2000,
                            array(Aerospike::OPT_POLICY_EXISTS=>Aerospike::POLICY_EXISTS_UPDATE )
                            ),
                        'Pass'
                        ),
            
            'To verify put with POLICY_EXISTS_CREATE_OR_REPLACE, positive test case. testPutGenPolicyExistsCreateOrReplacePositive' => 
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array(
                            array("bin1"=>"Hello World"),
                            2000,
                            array(Aerospike::OPT_POLICY_EXISTS=>Aerospike::POLICY_EXISTS_CREATE_OR_REPLACE )
                            ),
                        'Pass'
                        ),
            
            'To verify put with POLICY_EXISTS_CREATE_OR_REPLACE, positive test case. testPutGenPolicyExistsCreateOrReplacePositive' => 
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array(
                            array("bin1"=>"Hello World"),
                            2000,
                            array(Aerospike::OPT_POLICY_EXISTS=>Aerospike::POLICY_EXISTS_CREATE_OR_REPLACE )
                            ),
                        'Pass'
                        ),
            
            'To verify put with POLICY_EXISTS_IGNORE, positive test case. testPutGenPolicyExistsIgnorePositive' => 
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array(
                            array("bin1"=>"Hello World"),
                            2000,
                            array(Aerospike::OPT_POLICY_EXISTS=>Aerospike::POLICY_EXISTS_IGNORE )
                            ),
                        'Pass'
                        ),
           
            'To verify put with POLICY_EXISTS_REPLACE, positive test case. testPutGenPolicyExistsReplacePositive' => 
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array(
                            array("bin1"=>"Hello World"),
                            2000,
                            array(Aerospike::OPT_POLICY_EXISTS=>Aerospike::POLICY_EXISTS_REPLACE )
                            ),
                        'Pass'
                        ),
            
            'To verify put with POLICY_EXISTS_REPLACE, negative test case. testPutGenPolicyExistsReplaceNegative' => 
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKeyGenPolicyReplaceNegative"),
                        array(
                            array("bin1"=>"Hello World"),
                            2000,
                            array(Aerospike::OPT_POLICY_EXISTS=>Aerospike::POLICY_EXISTS_REPLACE )
                            ),
                        'Fail'
                        ),
            
            'To verify put with POLICY_EXISTS_REPLACE, negative test case. testPutGenPolicyExistsCreatePositive' => 
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKeyGenPolicyReplaceNegetive"),
                        array(
                            array("bin1"=>"Hello World"),
                            2000,
                            array(Aerospike::OPT_POLICY_EXISTS=>Aerospike::POLICY_EXISTS_REPLACE )
                            ),
                        'Fail'
                        ),
                    );
            
            
        
    }
    
    
    /**
    * @dataProvider dpNormalUDFSerialized
    * @test
    */
    function testNormalUDFSerialized($dpNormal){
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

        $this->objKey = $dpNormal[0];
        $status = self::$db->put($dpNormal[0], $dpNormal[1]);
        $this->assertEquals($status,AEROSPIKE::OK);
        $status = self::$db->get($dpNormal[0], $returned);
        $this->assertEquals($status,AEROSPIKE::OK);
        $this->assertEquals($returned['bins'], $dpNormal[1]);
    }
    
    function dpNormalUDFSerialized(){
        return array(
            'To verify put with nested list of object with UDF serializer. testPutNestedListOfObjectsWithUDFSerializer' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", new Employee(), array(array(12, new Employee(), new Employee()), new Employee(), " "))),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),
            
            'To verify put list of map of objects with UDF serializer. testPutListMapObjectsWithUDFSerializer' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", new Employee(), array(12=>new Employee(), new Employee(), "aa"=>new Employee()), 55)),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),
            
            'To verify put with map of list of objects and string value. testPutMapListObjectsWithUDFSerializer' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1"=>new Employee(), new Employee(), 56=>array(12, new Employee(), 56, " ", new Employee()))),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),
            
            'To verify put with nested list of boolean with UDF serializer. testPutNestedListOfBoolsWithUDFSerializer' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", 56, array(array(12, TRUE), " "), FALSE)),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),
            
            'To verify put with nested map of booleans. testPutNestedMapOfBoolsWithUDFSerializer' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", TRUE, 56=>array(array(12=>TRUE, FALSE), "K5"=>" "))),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),
            'To verify put list of map of booleans with UDF serializer. testPutListMapBoolsWithUDFSerializer' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", new Employee(), array(12=>new Employee(), FALSE, new Employee(), "aa"=>new Employee()), 55=>TRUE)),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),
            
            'To verify put map of list of booleans with UDF serializer. testPutMapListBoolsWithUDFSerializer' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1"=>new Employee(), "k8"=>TRUE, 56=>array(12, new Employee(), 56, " ", new Employee(), FALSE), new Employee())),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),
            
            'To verify put with nested list of floats with UDF serializer. testPutNestedListOfFloatsWithUDFSerializer' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", 56, array(array(12, 11.2), " "), 56.896)),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),
            
            'To verify put with nested map of float with UDF serializer. testPutNestedMapOfFloatsWithUDFSerializer' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", TRUE, 56=>array(array(12=>TRUE, FALSE), "K5"=>" "))),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),
            
            'To verify put with list of map of floats with UDF serailizer. testPutListMapFloatsWithUDFSerializer' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1", new Employee(), array(12=>new Employee(), new Employee(), "aa"=>25.6, "k10"=>TRUE), 55=>89.56, FALSE, 45.78)),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),
            
            'To verify put with map of list of floats with UDF serializer. testPutMapListFloatsWithUDFSerializer' => 
                array(
                    array(
                        array("ns"=>"test","set"=>"demo","key"=>"myKey"),
                        array("bin1"=>array("k1"=>new Employee(), "k8"=>TRUE, 56=>array(12, new Employee(), 63.2, " ", new Employee(), FALSE), new Employee(), "k22"=>56.75)),
                        NULL,
                        array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER)
                    )
                ),
                
            
            
        );
        
    }
    
    
    
    
    /**
    * 
     * @test
    */
     function testNormalPolicyKey1() {
        #Test Put init Key with digest and option key digest, positive test case.
        $this->objKey = self::$db->initKey("test", "demo", base64_decode("put_option_policy_key_digest"), true);
        $put_record = array("binA"=>10, "binB"=>20);
        $put_status = self::$db->put($this->objKey, array("binA"=>10, "binB"=>20), NULL,
            array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
        $this->assertEquals($put_status,AEROSPIKE::OK);
        $get_status = self::$db->get($this->objKey, $record, array());
        $this->assertEquals($get_status,AEROSPIKE::OK);
    }
    /**
    * 
     * @test
    */
    function testNormalPolicyKey2() {
        $policy = array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND);
        $this->objKey = self::$db->initKey("test", "demo", "policy_key_send_test");
        $put_status = self::$db->put($this->objKey, array("bin1"=>10), NULL, $policy);
        $this->assertEquals($put_status,AEROSPIKE::OK);
        $get_status = self::$db->get($this->objKey, $record, array(), $policy);
        $this->assertEquals($get_status,AEROSPIKE::OK);
        $this->assertEquals($record['key']['key'], 'policy_key_send_test');
     
    }
 
    /**
    * 
    * @test
    */
    function testErrorArrayNull()
    {
        $this->objKey = array("ns"=>"test", "set"=>"demo","key"=>"test_null_record_negative");
        $status = self::$db->put($this->objKey, array(NULL));
        $this->assertEquals($status, AEROSPIKE::ERR_CLIENT);
    }
    
    /**
    * 
    * @test
    */
    function testErrorInvalidBin()
    {
        $this->objKey = self::$db->initKey("test", "demo", "put_test");
        $status = self::$db->put($this->objKey,  array("bin456789012345"=>"Hello World"));
        $this->assertEquals($status,  Aerospike::ERR_BIN_NAME);
        //var_dump($status);
        
    }
    
    /**
    * 
    * @test
    */
    function bugCLIENT69()
    {
        $this->objKey = self::$db->initKey("test", "demo", "no_data");
        $status = self::$db->put($this->objKey,  array());
        $this->assertEquals($status,  Aerospike::ERR_PARAM);
    }
    
    
    /**
    * 
    * @test
    */
    function testEdge001()
    {
        $this->objKey = self::$db->initKey("test", "demo", "bin_name");
        $status = self::$db->put($this->objKey,  array("binA"));
        $this->assertEquals($status,  Aerospike::ERR_CLIENT);
    }
    
    /**
    * @after
    */
    function verifyValue(){
        self::$db->remove($this->objKey);
        $status=self::$db->exists($this->objKey, $metadata);
        $this->assertEquals($status,2);
             
    }
 }
?>
