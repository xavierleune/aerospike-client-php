<?php
require_once 'Common.inc';

/**
 *Basic Index creation and drop tests
*/

class Index extends AerospikeTestCommon
{
    /*
     * TODO: Remove these constants and map with actual Aerospike class
     * constants.
     */
    static $status_ok = 0;
    static $status_err = 201;

    function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key = $db->initKey("test", "demo", "index_test");
        $db->put($key, array("bin1"=>"1", "bin2"=>2, "bin3"=>3, "bin4"=>array(1,2,3)));
   }

   function tearDown(&$db, &$key) {
       $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
           =>Aerospike::POLICY_RETRY_NONE));
       $db->close();
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
    function testCreateAndDropIndexIntegerBinPositive($db, $key) {
        if ($db->isConnected()) {
            if (Aerospike::OK == $db->createIndex("test", "demo", "bin2",
                Aerospike::INDEX_TYPE_INTEGER, "bin2_integer_index")) {
                return($db->dropIndex("test", "bin2_integer_index"));
            } else {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
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
    function testCreateAndDropIndexIntegerBinWithOptionsPositive($db, $key) {
        if ($db->isConnected()) {
            if (Aerospike::OK == $db->createIndex("test", "demo", "bin2",
                Aerospike::INDEX_TYPE_INTEGER, "bin2_integer_index",
                array(Aerospike::OPT_WRITE_TIMEOUT => 12000))) {
                return($db->dropIndex("test", "bin2_integer_index",
                    array(Aerospike::OPT_WRITE_TIMEOUT => 10000)));
            } else {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
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
    function testCreateAndDropIndexSameIntegerBinWithSameNamePositive($db, $key) {
        if ($db->isConnected()) {
            if (Aerospike::OK == $db->createIndex("test", "demo", "bin2",
                Aerospike::INDEX_TYPE_INTEGER, "bin2_integer_index")) {
                if (Aerospike::OK == $db->createIndex("test", "demo", "bin2",
                    Aerospike::INDEX_TYPE_INTEGER, "bin2_integer_index")) {
                    return($db->dropIndex("test", "bin2_integer_index"));
                } else {
                    return($db->errorno());
                }
            } else {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
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
    function testCreateAndDropIndexDifferentIntegerBinWithSameNamePositive($db, $key) {
        if ($db->isConnected()) {
            if (Aerospike::OK == $db->createIndex("test", "demo", "bin2",
                Aerospike::INDEX_TYPE_INTEGER, "test_index")) {
                if (Aerospike::OK == $db->createIndex("test", "demo", "bin3",
                    Aerospike::INDEX_TYPE_INTEGER, "test_index")) {
                    return($db->dropIndex("test", "test_index"));
                } else {
                    return($db->errorno());
                }
            } else {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
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
    function testCreateAndDropIndexSameIntegerBinWithDifferentNamePositive($db, $key) {
        if ($db->isConnected()) {
            if (Aerospike::OK == $db->createIndex("test", "demo", "bin2",
                Aerospike::INDEX_TYPE_INTEGER, "test_index")) {
                if (Aerospike::OK == $db->createIndex("test", "demo", "bin2",
                    Aerospike::INDEX_TYPE_INTEGER, "test_index_new")) {
                    return($db->dropIndex("test", "test_index"));
                } else {
                    return($db->errorno());
                }
            } else {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
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
    function testCreateAndDropIndexStringBinPositive($db, $key) {
        if ($db->isConnected()) {
            if (Aerospike::OK == $db->createIndex("test", "demo", "bin1",
                Aerospike::INDEX_TYPE_STRING, "bin1_string_index")) {
                return($db->dropIndex("test", "bin1_string_index"));
            } else {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
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
    function testCreateAndDropIndexStringBinWithOptionsPositive($db, $key) {
        if ($db->isConnected()) {
            if (Aerospike::OK == $db->createIndex("test", "demo", "bin1",
                Aerospike::INDEX_TYPE_STRING, "bin1_string_index",
                array(Aerospike::OPT_WRITE_TIMEOUT => 10000))) {
                return($db->dropIndex("test", "bin1_string_index",
                    array(Aerospike::OPT_WRITE_TIMEOUT => 12000)));
            } else {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }

    /**
     * @test
     * Negative index create and drop operation on integer bin: non existing bin
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
    function testCreateAndDropIndexIntegerBinInvalidBinNegative($db, $key) {
        if ($db->isConnected()) {
            if (Aerospike::OK == $db->createIndex("test", "demo",
                "invalid_bin", Aerospike::INDEX_TYPE_INTEGER,
                "bin2_integer_index")) {
                return($db->dropIndex("test", "bin2_integer_index"));
            } else {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
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
    function testCreateAndDropIndexIntegerBinInvalidNamespaceNegative($db, $key) {
        if ($db->isConnected()) {
            if (Aerospike::OK == $db->createIndex("invalid_namespace", "demo",
                "bin2", Aerospike::INDEX_TYPE_INTEGER,
                "bin2_integer_index")) {
                return($db->dropIndex("test", "bin2_integer_index"));
            } else {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }

    /**
     * @test
     * Negative index create and drop operation on integer bin: non existing set
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
    function testCreateAndDropIndexIntegerBinInvalidSetNegative($db, $key) {
        if ($db->isConnected()) {
            if (Aerospike::OK == $db->createIndex("test", "invalid_set",
                "bin2", Aerospike::INDEX_TYPE_INTEGER,
                "bin2_integer_index")) {
                return($db->dropIndex("test", "bin2_integer_index"));
            } else {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
        }
    }

    /**
     * @test
     * Negative index create and drop operation on integer bin: invalid index type
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
    function testCreateAndDropIndexIntegerBinInvalidIndexTypeNegative($db, $key) {
        if ($db->isConnected()) {
            if (Aerospike::OK == $db->createIndex("test", "demo",
                "bin2", 10,
                "bin2_integer_index")) {
                return($db->dropIndex("test", "bin2_integer_index"));
            } else {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
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
    function testCreateAndDropIndexIntegerBinVeryLongIndexNameNegativeWithDBCrash($db, $key) {
        if ($db->isConnected()) {
                return (Aerospike::ERR);
            if (Aerospike::OK == $db->createIndex("test", "demo",
                "bin2", Aerospike::INDEX_TYPE_INTEGER,
                "bin2_integer_indexsdadadfasdfasdfeartfqrgahfas;dfheudsdfasdfawf312342q3453rf9qwfasdcfasdcalskdcbacfq34915rwcfasdcas.cnabscbaskjdbcalsjkbcdasc")) {
                return($db->dropIndex("test", "bin2_integer_index"));
            } else {
                return($db->errorno());
            }
        } else {
            return($db->errorno());
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
    function testDropIndexIntegerBinInvalidIndexNegative($db, $key) {
        if ($db->isConnected()) {
            return($db->dropIndex("test", "invalid_index"));
        } else {
            return($db->errorno());
        }
    }
}
?>
