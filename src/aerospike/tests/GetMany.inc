<?php
require_once 'Common.inc';

/**
 *Basic getMany opeartion tests
*/

class GetMany extends AerospikeTestCommon
{
    private $put_records = array();
    private $keys = array();

    protected function setUp(&$db, &$key) {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        $key1 = $db->initKey("test", "demo", "getMany1");
        $put_record1 = array("binA"=>10);
        $db->put($key1, $put_record1);
        $key2 = $db->initKey("test", "demo", "getMany2");
        $put_record2 = array("binB"=>20);
        $db->put($key2, $put_record2);
        $key3 = $db->initKey("test", "demo", "getMany3");
        $put_record3 = array("binC"=>30);
        $db->put($key3, $put_record3);
        $this->put_records = array($put_record1, $put_record2, $put_record3);
        $this->keys = array($key1, $key2, $key3);
    }

    protected function tearDown(&$db, &$key) {
        for($i = 0; $i < 3; $i++) { 
            $db->remove($this->keys[$i]);
        }
        $db->close();
    }

    /**
     * @test
     * Basic getMany operation with all arguments without options and filter bins.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGetManyPositive)
     *
     * @test_plans{1.1}
     */
    function testGetManyPositive($db, $key) {
        if ($db->isConnected()) {
            $key4 = $db->initKey("test", "demo", "getMany4");
            $my_keys = $this->keys + array($key4);
            $my_put_records = $this->put_records;
            $my_put_records[3] = NULL;
            $status = $db->getMany($my_keys, $records);
            if ($status != Aerospike::OK) {
                return $db->errorno();
            }
            if (empty($records)) {
                return Aerospike::ERR_CLIENT;
            }

            $i = 0;
            foreach ($records as $records_key=>$value) {
                if ($i == 3 && $value) {
                    return Aerospike::ERR_CLIENT;    
                } else {
                    return Aerospike::OK;
                }

                if ((!strcmp($my_keys[$i]["key"], $records_key)) &&
                    (!strcmp($my_keys[$i]["ns"], $value["key"]["ns"])) &&
                    (!strcmp($my_keys[$i]["set"], $value["key"]["set"]))) {
                    $result = array_diff_assoc_recursive($my_put_records[$i],
                                                $value["bins"]);
                    if (!empty($result)) {
                        return Aerospike::ERR_CLIENT;
                    }
                    $i++;
                } else {
                    return Aerospike::ERR_CLIENT;
                }
            }
            return $status;
        } else {
            return $db->errorno();
        }
    }

    /**
     * @test
     * Basic getMany operation with all arguments with options without filter bins.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGetManyPositive)
     *
     * @test_plans{1.1}
     */
    function testGetManyWithOptionsPositive($db, $key) {
        if ($db->isConnected()) {
            $key4 = $db->initKey("test", "demo", "getMany4");
            $my_keys = $this->keys + array($key4);
            $my_put_records = $this->put_records;
            $my_put_records[3] = NULL;
            $status = $db->getMany($my_keys, $records, NULL,
                array(Aerospike::OPT_READ_TIMEOUT=>3000));
            if ($status != Aerospike::OK) {
                return $db->errorno();
            }
            if (empty($records)) {
                return Aerospike::ERR_CLIENT;
            }

            $i = 0;
            foreach ($records as $records_key=>$value) {
                if ($i == 3 && $value) {
                    return Aerospike::ERR_CLIENT;    
                } else {
                    return Aerospike::OK;
                }
                if ((!strcmp($my_keys[$i]["key"], $records_key)) &&
                    (!strcmp($my_keys[$i]["ns"], $value["key"]["ns"])) &&
                    (!strcmp($my_keys[$i]["set"], $value["key"]["set"]))) {
                    $result = array_diff_assoc_recursive($my_put_records[$i],
                                                $value["bins"]);
                    if (!empty($result)) {
                        return Aerospike::ERR_CLIENT;
                    }
                    $i++;
                } else {
                    return Aerospike::ERR_CLIENT;
                }
            }
            return $status;
        } else {
            return $db->errorno();
        }
    }

    /**
     * @test
     * Basic getMany operation with empty key array.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGetManyEmptyKeyArrayNegative)
     *
     * @test_plans{1.1}
     */
    function testGetManyEmptyKeyArrayNegative($db, $key) {
        if ($db->isConnected()) {
            $keys = array();
            $status = $db->getMany($keys, $records, NULL,
                array(Aerospike::OPT_READ_TIMEOUT=>3000));
            if ($status != Aerospike::OK) {
                return $db->errorno();
            }
            if (!empty($records)) {
                return Aerospike::ERR_CLIENT;
            } else {
                return Aerospike::OK;
            }
        } else {
            return $db->errorno();
        }
     }

    /**
     * @test
     * Basic getMany operation with empty string passed 
       in place of key array .
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGetManyKeyEmptyStringNegative)
     *
     * @test_plans{1.1}
     */
    function testGetManyKeyEmptyStringNegative($db, $key) {
        if ($db->isConnected()) {
            try {
                $status = $db->getMany("", $records, NULL,
                    array(Aerospike::OPT_READ_TIMEOUT=>3000));
                if ($status != Aerospike::OK) {
                    return $db->errorno();
                }
                if (empty($records)) {
                    return Aerospike::OK;
                }
                return $status;
            } catch (ErrorException $e) {
                return Aerospike::ERR_CLIENT;
            }

       } else {
           return $db->errorno();
       }
       return $status;
    }

    /**
     * @test
     * Basic getMany operation with no arguments.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGetManyNoArgumentsNegative)
     *
     * @test_plans{1.1}
     */
    function testGetManyNoArgumentsNegative($db, $key) {
        if ($db->isConnected()) {
            try {
                $status = $db->getMany();
                if ($status != AEROSPIKE_OK) {
                    return $db->errorno();
                }
                if (empty($records)) {
                    return Aerospike::ERR;
                }
                return $db->status;
            } catch (ErrorException $e) {
                return Aerospike::OK;
            }
        } else {
            return $db->errorno();
        }
    }

    /**
     * @test
     * Basic getMany, record is empty string variable.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGetManyRecordEmptyStringNegative)
     *
     * @test_plans{1.1}
     */
    function testGetManyRecordEmptyStringNegative($db, $key) {
       if ($db->isConnected()) {
           $records = "";
           $status = $db->getMany($this->keys, $records);
           if ($status != Aerospike::OK) {
               return $db->errorno();
           }
           if (empty($records)) {
               return Aerospike::ERR_CLIENT;
           }
           $i = 0;
           foreach ($records as $records_key=>$value) {
               if ((!strcmp($this->keys[$i]["key"], $records_key)) &&
                   (!strcmp($this->keys[$i]["ns"], $value["key"]["ns"])) &&
                   (!strcmp($this->keys[$i]["set"], $value["key"]["set"]))) {
                   $result = array_diff_assoc_recursive($this->put_records[$i],
                                              $value["bins"]);
                   if (!empty($result)) {
                       return Aerospike::ERR_CLIENT;
                   }
                   $i++;
               } else {
                   return Aerospike::ERR_CLIENT;
               }
           }
           return $status;
       } else {
           return $db->errorno();
       }
    }

    /**
     * @test
     * Basic getMany, Record is of type string.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGetManyMetadataStringVariableNegative)
     *
     * @test_plans{1.1}
     */
    function testGetManyRecordStringVariableNegative($db, $key) {
        if ($db->isConnected()) {
            $records = "abc";
            $status = $db->getMany($this->keys, $records);
            if ($status != Aerospike::OK) {
                return $db->errorno();
            }
            if (empty($records)) {
                return Aerospike::ERR_CLIENT;
            }
            $i = 0;
            foreach ($records as $records_key=>$value) {
                if ((!strcmp($this->keys[$i]["key"], $records_key)) &&
                    (!strcmp($this->keys[$i]["ns"], $value["key"]["ns"])) &&
                    (!strcmp($this->keys[$i]["set"], $value["key"]["set"]))) {
                    $result = array_diff_assoc_recursive($this->put_records[$i],
                                                $value["bins"]);
                    if (!empty($result)) {
                        return Aerospike::ERR_CLIENT;
                    }
                    $i++;
                } else {
                    return Aerospike::ERR_CLIENT;
                }
            }
            return $status;
        } else {
            return $db->errorno();
        }
    }

    /**
     * @test
     * Basic getMany, Options is of type integer.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testGetManyOptionsIntegerNegative)
     *
     * @test_plans{1.1}
     */
    function testGetManyOptionsIntegerNegative($db, $key) {
        if ($db->isConnected()) {
            try {
                $status = $db->getMany($this->keys, $records, NULL, 12);
                if ($status != Aerospike::OK) {
                    return $db->errorno();
                }
                if (empty($records)) {
                    return Aerospike::ERR_CLIENT;
                }
                return $status;
            } catch (ErrorException $e) {
                return Aerospike::OK;
            }
        } else {
            return $db->errorno();
        }
    }
}
