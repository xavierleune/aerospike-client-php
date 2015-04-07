<?php
require_once 'Common.inc';

/**
 *Basic Index creation and drop tests
*/

class Index extends AerospikeTestCommon
{
    function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        $key = $this->db->initKey("test", "demo", "index_test");
        $this->db->put($key, array("bin1"=>"1", "bin2"=>2, "bin3"=>3, "bin4"=>array(1,2,3)));
        $this->keys[] = $key;
   }


    /**
     * @test
     * Basic index create and drop operation on integer bin
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexStringBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateAndDropIndexIntegerBinPositive() {
        if (Aerospike::OK == $this->db->createIndex("test", "demo", "bin2",
            Aerospike::INDEX_TYPE_INTEGER, "bin2_integer_index")) {
            return($this->db->dropIndex("test", "bin2_integer_index"));
        } else {
            return $this->db->errorno();
        }
    }

    /**
     * @test
     * Basic index create and drop operation on integer bin with options
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexStringBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateAndDropIndexIntegerBinWithOptionsPositive() {
        if (Aerospike::OK == $this->db->createIndex("test", "demo", "bin2",
            Aerospike::INDEX_TYPE_INTEGER, "bin2_integer_index",
            array(Aerospike::OPT_WRITE_TIMEOUT => 12000))) {
            return($this->db->dropIndex("test", "bin2_integer_index",
                array(Aerospike::OPT_WRITE_TIMEOUT => 10000)));
        } else {
            return $this->db->errorno();
        }
    }
    /**
     * @test
     * Index recreate and drop operation on integer bin with same name on which index is already created
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexIntegerBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateAndDropIndexSameIntegerBinWithSameNamePositive() {
        if (Aerospike::OK == $this->db->createIndex("test", "demo", "bin2",
            Aerospike::INDEX_TYPE_INTEGER, "bin2_integer_index")) {
            if (Aerospike::OK == $this->db->createIndex("test", "demo", "bin2",
                Aerospike::INDEX_TYPE_INTEGER, "bin2_integer_index")) {
                return($this->db->dropIndex("test", "bin2_integer_index"));
            } else {
                return $this->db->errorno();
            }
        } else {
            return $this->db->errorno();
        }
    }

    /**
     * @test
     * Index create and drop operation with same name as an existing index on a different integer bin.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexIntegerBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateAndDropIndexDifferentIntegerBinWithSameNamePositive() {
        if (Aerospike::OK == $this->db->createIndex("test", "demo", "bin2",
            Aerospike::INDEX_TYPE_INTEGER, "test_index")) {
            if (Aerospike::OK == $this->db->createIndex("test", "demo", "bin3",
                Aerospike::INDEX_TYPE_INTEGER, "test_index")) {
                return($this->db->dropIndex("test", "test_index"));
            } else {
                return $this->db->errorno();
            }
        } else {
            return $this->db->errorno();
        }
    }

    /**
     * @test
     * Index create and drop operation with new index with different name on an already
     * indexed integer bin.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexIntegerBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateAndDropIndexSameIntegerBinWithDifferentNamePositive() {
        if (Aerospike::OK == $this->db->createIndex("test", "demo", "bin2",
            Aerospike::INDEX_TYPE_INTEGER, "test_index")) {
            if (Aerospike::OK == $this->db->createIndex("test", "demo", "bin2",
                Aerospike::INDEX_TYPE_INTEGER, "test_index_new")) {
                return($this->db->dropIndex("test", "test_index"));
            } else {
                return $this->db->errorno();
            }
        } else {
            return $this->db->errorno();
        }
     }

    /**
     * @test
     * Basic index create and drop operation on string bin
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexIntegerBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateAndDropIndexStringBinPositive() {
        if (Aerospike::OK == $this->db->createIndex("test", "demo", "bin1",
            Aerospike::INDEX_TYPE_STRING, "bin1_string_index")) {
            return($this->db->dropIndex("test", "bin1_string_index"));
        } else {
            return $this->db->errorno();
        }
    }
    /**
     * @test
     * Basic index create and drop operation on string bin with set as NULL
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexIntegerBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateAndDropIndexStringSetNull() {
        if (Aerospike::OK == $this->db->createIndex("test", NULL, "bin1",
            Aerospike::INDEX_TYPE_STRING, "bin1_string_index")) {
            return($this->db->dropIndex("test", "bin1_string_index"));
        } else {
            return $this->db->errorno();
        }
    }

    /**
     * @test
     * Basic index create and drop operation on string bin with options
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexIntegerBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateAndDropIndexStringBinWithOptionsPositive() {
        if (Aerospike::OK == $this->db->createIndex("test", "demo", "bin1",
            Aerospike::INDEX_TYPE_STRING, "bin1_string_index",
            array(Aerospike::OPT_WRITE_TIMEOUT => 10000))) {
            return($this->db->dropIndex("test", "bin1_string_index",
                array(Aerospike::OPT_WRITE_TIMEOUT => 12000)));
        } else {
            return $this->db->errorno();
        }
    }
    /**
     * @test
     * Index recreate and drop operation on string bin with same name on which index is already created
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexIntegerBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateAndDropIndexSameStringBinWithSameNamePositive() {
        if (Aerospike::OK == $this->db->createIndex("test", "demo", "bin2",
            Aerospike::INDEX_TYPE_STRING, "bin2_string_index")) {
            if (Aerospike::OK == $this->db->createIndex("test", "demo", "bin2",
                Aerospike::INDEX_TYPE_STRING, "bin2_string_index")) {
                return($this->db->dropIndex("test", "bin2_string_index"));
            } else {
                return $this->db->errorno();
            }
        } else {
            return $this->db->errorno();
        }
    }

    /**
     * @test
     * Index create and drop operation with same name as an existing index on a different string bin.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexIntegerBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateAndDropIndexDifferentStringBinWithSameNamePositive() {
        if (Aerospike::OK == $this->db->createIndex("test", "demo", "bin2",
            Aerospike::INDEX_TYPE_STRING, "test_index")) {
            if (Aerospike::OK == $this->db->createIndex("test", "demo", "bin3",
                Aerospike::INDEX_TYPE_STRING, "test_index")) {
                return($this->db->dropIndex("test", "test_index"));
            } else {
                return $this->db->errorno();
            }
        } else {
            return $this->db->errorno();
        }
    }

    /**
     * @test
     * Index create and drop operation with new index with different name on an already
     * indexed integer bin.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexIntegerBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateAndDropIndexSameStringBinWithDifferentNamePositive() {
        if (Aerospike::OK == $this->db->createIndex("test", "demo", "bin2",
            Aerospike::INDEX_TYPE_STRING, "test_index")) {
            if (Aerospike::OK == $this->db->createIndex("test", "demo", "bin2",
                Aerospike::INDEX_TYPE_STRING, "test_index_new")) {
                return($this->db->dropIndex("test", "test_index"));
            } else {
                return $this->db->errorno();
            }
        } else {
            return $this->db->errorno();
        }
     }
    /**
     * @test
     * Positive index create and drop operation on integer bin: non existing bin
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexIntegerBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateAndDropIndexIntegerBinInvalidBinPositive() {
        if (Aerospike::OK == $this->db->createIndex("test", "demo",
            "invalid_bin", Aerospike::INDEX_TYPE_INTEGER,
            "bin2_integer_index")) {
            return($this->db->dropIndex("test", "bin2_integer_index"));
        } else {
            return $this->db->errorno();
        }
    }

    /**
     * @test
     * Positive index create and drop operation on integer bin: non existing set
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexIntegerBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateAndDropIndexIntegerBinInvalidSetPositive() {
        if (Aerospike::OK == $this->db->createIndex("test", "invalid_set",
            "bin2", Aerospike::INDEX_TYPE_INTEGER,
            "bin2_integer_index")) {
            return($this->db->dropIndex("test", "bin2_integer_index"));
        } else {
            return $this->db->errorno();
        }
    }


    /**
     * @test
     * Negative index create and drop operation on integer bin: non existing namespace
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexIntegerBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateAndDropIndexIntegerBinInvalidNamespaceNegative() {
        if (Aerospike::OK == $this->db->createIndex("invalid_namespace", "demo",
            "bin2", Aerospike::INDEX_TYPE_INTEGER,
            "bin2_integer_index")) {
            return($this->db->dropIndex("test", "bin2_integer_index"));
        } else {
            return $this->db->errorno();
        }
    }

    /**
     * @test
     * Positive index create and drop operation on integer bin: invalid index
     * type, will use the default string index type.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexIntegerBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateAndDropIndexIntegerBinInvalidIndexTypePositive() {
        if (Aerospike::OK == $this->db->createIndex("test", "demo",
            "bin2", 10,
            "bin2_integer_index")) {
            return($this->db->dropIndex("test", "bin2_integer_index"));
        } else {
            return($this->db->errorno());
        }
    }

    /**
     * @test
     * Negative index create and drop operation on integer bin: very long index name
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexIntegerBinPositive)
     *
     * @test_plans{1.1}
     */
    function testCreateAndDropIndexIntegerBinVeryLongIndexNameNegativeWithDBCrash() {
        if (Aerospike::OK == $this->db->createIndex("test", "demo",
            "bin2", Aerospike::INDEX_TYPE_INTEGER,
            "bin2_integer_indexsdadadfasdfasdfeartfqrgahfas;dfheudsdfasdfawf312342q3453rf9qwfasdcfasdcalskdcbacfq34915rwcfasdcas.cnabscbaskjdbcalsjkbcdasc")) {
            return($this->db->dropIndex("test", "bin2_integer_index"));
        } else {
            return $this->db->errorno();
        }
    }

    /**
     * @test
     * Negative index drop operation on integer bin: drop a non existing index
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testCreateIndexIntegerBinPositive)
     *
     * @test_plans{1.1}
     */
    function testDropIndexIntegerBinInvalidIndexNegative() {
        return($this->db->dropIndex("test", "invalid_index"));
    }
}
?>
