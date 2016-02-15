<?php
require_once 'Util.inc';

class Operate  extends PHPUnit_Framework_TestCase
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
     * @dataProvider    dpNormalOperate
     */
    function normalOperate($putVal, $opArray, $expValue, $options=array()) {
        self::$db->put($this->objKey, $putVal);
        self::$db->operate($this->objKey, $opArray, $returned, $options);
        $this->assertEquals($returned, $expValue);            
    }
    
    function dpNormalOperate(){
        return array(
            "test operate, positive test case"=>
            array(
                array("first_name"=>"John"),
                array(
                    array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
                    array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name","val" => "."),
                    array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                    array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
                ),
                array("first_name" => "Mr John")
            ),
            
            "test increment operate, positive tc"=>
            array(
                array("age"=>1),
                array(
                    array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
                    array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name","val" => "."),
                    array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                    array("op" => Aerospike::OPERATOR_READ, "bin" => "age")
                ),
                array("age" => 2)
            ),
            
            "test operate with options, positive tc"=>
            array(
                array("age"=>1),
                array(
                    array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
                    array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name","val" => "."),
                    array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                    array("op" => Aerospike::OPERATOR_READ, "bin" => "age")
                ),
                array("age" => 2),
                array(Aerospike::OPT_WRITE_TIMEOUT=>2000, Aerospike::OPT_POLICY_COMMIT_LEVEL=>Aerospike::POLICY_COMMIT_LEVEL_MASTER)
            ),
            
            "test operate upon same bins"=>
            array(
                array("first_name"=>"!"),
                array(
                    array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Hello"),
                    array("op" => Aerospike::OPERATOR_APPEND, "bin" => "first_name", "val" => " World"),
                    array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
                ),
                array("first_name" => "Hello! World"),
                array(Aerospike::OPT_WRITE_TIMEOUT=>2000, Aerospike::OPT_POLICY_COMMIT_LEVEL=>Aerospike::POLICY_COMMIT_LEVEL_MASTER)
            ),

            "test operate APPEND,INSERT,INSERT_ITEMS,POP for cdt data type"=>
            array(
                array("binA"=>100, "binB" => array("John", 15, array(25,45),array("name"=>"Dan"), 45.8, True)),
                array(
                    array("op" => Aerospike::OP_LIST_APPEND, "bin" => "binB", "val" => 1234),
                    array("op" => Aerospike::OP_LIST_INSERT, "bin" => "binB", "index" => 2, "val" => 1234),
                    array("op" => Aerospike::OP_LIST_INSERT_ITEMS, "bin" => "binB", "index" => 2, "val" => array(123, 456)),
                    array("op" => Aerospike::OP_LIST_POP, "bin" => "binB", "index" => 1)
                ),
                array("binB" => 15)
            ),

            "test operate POP_RANGE for cdt data type" =>
            array(
                array("binA"=>100, "binB" => array("John", 15, array(25,45),array("name"=>"Dan"), 45.8, True)),
                array(
                    array("op" => Aerospike::OP_LIST_POP_RANGE, "bin" => "binB", "index" => 1, "val" => 3)
                ),
                array("binB" => array(15, array(25,45), array("name"=>"Dan")))
            ),

            "test operate REMOVE, REMOVE_RANGE, CLEAR, SET, GET for cdt data type" =>
            array(
                array("binA"=>100, "binB" => array("John", 15, array(25,45),array("name"=>"Dan"), 45.8, True)),
                array(
                    array("op" => Aerospike::OP_LIST_REMOVE, "bin" => "binB", "index" => 1),
                    array("op" => Aerospike::OP_LIST_REMOVE_RANGE, "bin" => "binB", "index" => 1, "val" => 3),
                    array("op" => Aerospike::OP_LIST_CLEAR, "bin" => "binB"),
                    array("op" => Aerospike::OP_LIST_SET, "bin" => "binB", "index" => 2, "val" => "latest event at index 2"),
                    array("op" => Aerospike::OP_LIST_GET, "bin" => "binB", "index" => 2)
                ),
                array("binB" => "latest event at index 2")
            ),

            "test operate GET_RANGE, TRIM, SIZE for cdt data type" =>
            array(
                array("binA"=>100, "binB" => array("John", 15, array(25,45),array("name"=>"Dan"), 45.8, True)),
                array(
                    array("op" => Aerospike::OP_LIST_GET_RANGE, "bin" => "binB", "index" => 2, "val" => 3),
                    array("op" => Aerospike::OP_LIST_TRIM, "bin" => "binB", "index" => 2, "val" => 3),
                    array("op" => Aerospike::OP_LIST_SIZE, "bin" => "binB")
                ),
                array("binB" => 3)
            )
        );    
    }
    
    /**
     * @test
     */
    function errorEmptyArgument() {
        try {
            $status = self::$db->operate();
        } catch (Exception $e) {
            $status = AEROSPIKE::ERR_PARAM;
        }
        $this->assertEquals($status, AEROSPIKE::ERR_PARAM);
    }
    
    /**
     * @test
     */
    function errorKeyEmptyString() {
        $check = array("first_name" => "Mr John");
        $operations = array(
            array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
            array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name", "val" => "."),
            array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
            array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
        );
        $returned = array();
        $status = self::$db->operate("", $operations, $returned, array(Aerospike::OPT_WRITE_TIMEOUT=>2000, Aerospike::OPT_POLICY_COMMIT_LEVEL=>Aerospike::POLICY_COMMIT_LEVEL_ALL));
        $this->assertEquals($status, AEROSPIKE::ERR_PARAM);
    }
    
    /**
     * @test
     * @dataProvider    dpNormalOperateReturn
     */
    function normalOperateReturn($returnVal) {
        $check = array("first_name" => "Mr John");
        self::$db->put($this->objKey, array("first_name" => "John"));
        $operations = array(
            array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
            array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name", "val" => "."),
            array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
            array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
        );
        $returned = $returnVal;
        $status = self::$db->operate($this->objKey, $operations, $returned, array(Aerospike::OPT_WRITE_TIMEOUT=>2000));
        $this->assertEquals($status, AEROSPIKE::OK);
        $this->assertEquals($returned, $check);
    }
    
    function dpNormalOperateReturn(){
        return array(
            "test with return variable already assigned with string value"=>
            array(
                "abcdef"
            ),
            
            "test with return variable already assigned with NULL value"=>
            array(
                NULL
            ),
        );
    }
    
    /**
     *  @test 
     *  @dataProvider   dpNormalGenPolicy     
     */
    function normalGenPolicy($opArray, $expValue, $genPolicy){
        self::$db->put($this->objKey, array("first_name"=>"John"));
        self::$db->exists($this->objKey, $metadata);
        $this->gen_Value = $metadata["generation"];
        $status = self::$db->operate($this->objKey, $opArray, $returned, $genPolicy);
        $this->assertEquals($status, AEROSPIKE::OK);
        $this->assertEquals($returned, $expValue);
    }
    
    function dpNormalGenPolicy(){
        return array(
            "POLICY_GEN_IGNORE. Positive"=>
            array(
                array(
                       array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
                       array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name","val" => "."),
                       array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                       array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
                ),                
                array("first_name"=>"Mr John"),                                
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_IGNORE)),
            ),
            
            "POLICY_GEN_GT. Positive"=>
            array(
                array(
                       array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
                       array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name","val" => "."),
                       array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                       array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
                    
                ),                
                array("first_name"=>"Mr John"),                                
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT, ($this->gen_value+10))),
            ),
            
            "POLICY_GEN_GT. Negative"=>
            array(
                array(
                       array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
                       array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name","val" => "."),
                       array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                       array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
                    
                ),                
                array(),                                
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT, ($this->gen_value))),
            ),
            
            "POLICY_GEN_GT. Positive"=>
            array(
                array(
                       array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
                       array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name","val" => "."),
                       array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                       array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
                    
                ),               
                array("first_name"=>"Mr John"),                               
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT, ($this->gen_value+5))),
            ),
                       
            "POLICY_GEN_EQ. Positive"=>
            array(
                array(
                       array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
                       array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name","val" => "."),
                       array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                       array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
                    
                ),               
                array("first_name"=>"Mr John"),                               
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_EQ, ($this->gen_value+1))),
            ),
            
            "POLICY_GEN_EQ. Negative"=>
            array(
                array(
                       array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
                       array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name","val" => "."),
                       array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
                       array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
                    
                ),               
                array(),                               
                array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_EQ, ($this->gen_value))),
            ),
        );
    }
    
    
    /**
     * @test
     */
    function testOperateWithPolicyKeyDigest() {
        $check = array("first_name" => "Mr John");
        $this->objKey = self::$db->initKey("test", "demo", base64_decode("operate_policy_key_digest"), true);
        $put_status = self::$db->put($this->objKey, array("first_name"=>"John", "last_name"=>"Smith", "age"=>25), NULL, array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
        $operations = array(
            array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
            array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name", "val" => "."),
            array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
            array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
        );
        $returned = "abc";
        $status = self::$db->operate($this->objKey, $operations, $returned, array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
        $this->assertEquals($status, AEROSPIKE::OK);
        $this->assertEquals($returned, $check);
    }
    /**
     * @test
     */
    function testOperateWithPolicyKeySend() {
        $check = array("first_name" => "Mr John");
        
        self::$db->put($this->objKey,array("first_name"=>"John","last_name"=>"Smith","age"=>25));
        
        $operations = array(
            array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "first_name", "val" => "Mr "),
            array("op" => Aerospike::OPERATOR_APPEND, "bin" => "last_name", "val" => "."),
            array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
            array("op" => Aerospike::OPERATOR_READ, "bin" => "first_name")
        );

        $returned = "abc";
        
        $status = self::$db->operate($this->objKey, $operations, $returned, array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND));
        $this->assertEquals($status, AEROSPIKE::OK);
        $this->assertEquals($returned, $check);
        
    }

    /**
     * @test
     * @dataProvider  dpOperateTTL    
     */
    function operateTTL($ttl, $expStatus) {
        $operations = array(array("op" => Aerospike::OPERATOR_TOUCH, "ttl" => $ttl));
        $status=self::$db->put($this->objKey, array("first_name"=>"John", "last_name"=>"Smith", "age"=>25));
        $this->assertEquals($status, AEROSPIKE::OK);
        $status = self::$db->operate($this->objKey, $operations, $returned);
        if($expStatus==="FAIL"){
            $this->assertNotEquals($status, AEROSPIKE::OK);
            return;
        }
        $this->assertEquals($status, AEROSPIKE::OK);
        $status = self::$db->exists($this->objKey, $metadata);
        $this->assertEquals($status, AEROSPIKE::OK);
        $this->assertTrue(!empty($metadata));
    }
    
    function dpOperateTTL(){
        return array(
            "test operate with ttl as numeric, positive"=>
            array(
                400,
                "PASS"
            ),
            "test operate with ttl as string, negative"=>
            array(
                "abcd",
                "FAIL"
            ),
            "test operate with ttl offset as string, negative"=>
            array(
                "400",
                "FAIL"
            ),
            "test operate with ttl negative offset as string, negative"=>
            array(
                "-100",
                "FAIL"
            ),
            "test operate with ttl alpha numberic offset as string, negative"=>
            array(
                "100asd",
                "FAIL"
            )
        );
    }   
}
?>
