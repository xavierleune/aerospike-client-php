<?php
require_once 'Util.inc';

class Exists extends PHPUnit_Framework_TestCase
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
        if(isset($this->objKey)){
            self::$db->remove($this->objKey);
        }
        $this->objKey = self::$db->initKey("test", "demo", "exist_key");
        self::$db->put($this->objKey, array("Greet"=>"Hello World!"));
    }
    
    function teardown(){
        self::$db->remove($this->objKey);
    }
      
    /**
     * @test
     * @dataProvider dpNormalKeyExistsWithPolicy
     */
    function normalKeyExistsWithPolicy($arrayPolicy) {
       $status = self::$db->exists($this->objKey, $metadata, $arrayPolicy);
       $this->assertTrue(array_key_exists('generation', $metadata));
       $this->assertTrue(array_key_exists('ttl', $metadata));
    }
    
    function dpNormalKeyExistsWithPolicy(){
        return array(
            array(
                array(Aerospike::OPT_READ_TIMEOUT=>3000,
                    Aerospike::OPT_POLICY_REPLICA=>Aerospike::POLICY_REPLICA_MASTER,
                    Aerospike::OPT_POLICY_CONSISTENCY=>Aerospike::POLICY_CONSISTENCY_ONE)
                )
        );
    }
    
    /**
     * @test
     * @dataProvider dpNormalExists
     */
   function normalExists($valueToPut, $ttl, $option) {
        $status = self::$db->put($this->objKey, $valueToPut, $ttl, $option);
        $status = self::$db->exists($this->objKey, $metadata);
        $this->assertTrue(array_key_exists('generation', $metadata));
        $this->assertTrue(array_key_exists('ttl', $metadata));
    }
    
    function dpNormalExists(){
        return array(
            "Test with mixed data"=>
            array(
                array("basiclist"=>array(12, 63.2, 44, 58), "list"=>array(12, new Employee(), 63.2, " ", new Employee(), FALSE), "basicmap"=>array("k1"=>1, "k2"=>2, "k3"=>3), "map"=>array("k1"=>new Employee(), "k8"=>TRUE, 56=>array(12, new Employee(), 63.2, " ", new Employee(), FALSE), new Employee(), "k22"=>56.75)),
                NULL,
                array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP)
            )
        );
    }
    
    /**
     * @test
     * @dataProvider dpKeyDoesNotExist
     */
    function testKeyDoesNotExist($ns, $set, $key) {
        $this->objKey = self::$db->initKey($ns, $set, $key);
        $status = self::$db->exists($this->objKey, $metadata);
        $this->assertEquals($status, AEROSPIKE::ERR_RECORD_NOT_FOUND);
    }
    
    function dpKeyDoesNotExist(){
        return array(
            "test with invalid key"=>
            array(
                "test",
                "demo",
                "-----------sss----"
            )
        );
    }
}
