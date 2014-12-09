<?php
require_once 'Common.inc';

/**
 *Basic Key Exists/getMetaData opeartion tests
*/

class Employee
{
    public $desg = 'Manager';
}
class Exist extends AerospikeTestCommon
{
    protected function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        if (!$this->db->isConnected()) {
            return $this->db->errorno();
        }
        $key = $this->db->initKey("test", "demo", "exist_key");
        $this->db->put($key,array("Greet"=>"World_end"));
        $this->keys[] = $key;
    }

    /**
     * @test
     * Basic key exist/getmedata operation
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyExist)
     *
     * @test_plans{1.1}
     */
   function testKeyExist() {
       $status = $this->db->exists($this->keys[0], $metadata);
       if(array_key_exists('generation', $metadata) && array_key_exists('ttl', $metadata)) {
            return $status;
       }
       return Aerospike::ERR_CLIENT;
    }
    /**
     * @test
     * Basic key exist/getmedata operation with policy
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyExist)
     *
     * @test_plans{1.1}
     */
   function testKeyExistWithPolicy() {
       $status = $this->db->exists($this->keys[0], $metadata, array(Aerospike::OPT_READ_TIMEOUT=>3000));
       if(array_key_exists('generation', $metadata) && array_key_exists('ttl', $metadata)) {
            return $status;
       }
       return Aerospike::ERR_CLIENT;
    }
    /**
     * @test
     * Basic key exist/getmedata operation with mixed data
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyExist)
     *
     * @test_plans{1.1}
     */
   function testKeyExistWithMixedData() {
        $key = $this->db->initKey("test", "demo", "exist_key");
        $obj1 = new Employee();
        $obj2 = new Employee();
        $obj3 = new Employee();
        $obj4 = new Employee();
        $basiclist = array(12, 63.2, 44, 58);
        $list = array(12, $obj2, 63.2, " ", $obj4, FALSE);
        $basicmap = array("k1"=>1, "k2"=>2, "k3"=>3);
        $map = array("k1"=>$obj3, "k8"=>TRUE, 56=>$list, $obj1, "k22"=>56.75);

        $put_record = array("basiclist"=>$basiclist, "list"=>$list, "basicmap"=>$basicmap, "map"=>$map);
        $status = $this->db->put($key, $put_record, NULL, array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_PHP));
        $status = $this->db->exists($this->keys[0], $metadata, array(Aerospike::OPT_READ_TIMEOUT=>3000));
        $this->db->remove($key);
       if(array_key_exists('generation', $metadata) && array_key_exists('ttl', $metadata)) {
            return $status;
       }
       return Aerospike::ERR_CLIENT;
    }
    /**
     * @test
     * Basic key exist operation when key not exist
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyNotExist)
     *
     * @test_plans{1.1}
     */
    function testKeyNotExist() {
        $key1 = $this->db->initKey("test", "demo", "----sss---------");
        return $this->db->exists($key1, $metadata);
    }
}
