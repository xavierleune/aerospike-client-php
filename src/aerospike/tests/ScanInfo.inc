<?php
require_once 'Common.inc';
/**

 *Basic Aggregate tests

*/
class ScanInfo extends AerospikeTestCommon
{
    /*
     * TODO: Remove these constants and map with actual Aerospike class
     * constants.
     */
    protected function setUp(&$db, &$scan_id) {
	$config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
    $db = new Aerospike($config);
    $key = $db->initKey("test", "demo", "Get_key");
    $db->put($key,array("first_name"=>"raunak","age"=>29));
    $key = $db->initKey("test", "demo", "Get_key1");
    $db->put($key,array("first_name"=>"pavan","age"=>27));
    $key = $db->initKey("test", "demo", "Get_key2");
    $db->put($key,array("first_name"=>"bhushan","age"=>22));
	$key = $db->initKey("test", "demo", "Get_key3");
    $db->put($key,array("first_name"=>"ajit","age"=>32));

    $register_status = $db->register("./tests/phpt/ScanBackground/my_udf.lua", "my_udf");
        if ($register_status != Aerospike::OK) {
            return($db->errorno());
        }
    $status = $db->scanBackground("my_udf", "mytransform", array(20),"test", "demo", $scan_id);
        if ($status != Aerospike::OK) {
            return($db->errorno());
        }
    }

    protected function tearDown(&$db) {
        if (!$db->isConnected()) {
            $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
            $db = new Aerospike($config);
        }
        $key = $db->initKey("test","demo","Get_key");
        for($i=1;$i<4;$i++) {
        $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
            =>Aerospike::POLICY_RETRY_NONE));
        $key=$db->initKey("test","demo","Get_key".$i);
        }

        $degister_status = $db->deregister("my_udf");
        if ($deregister_status != Aerospike::OK) {
        return($db->errorno());
        }
        $db->close();
    }

	/**
	 * @test
     * ScanInfo - All correct arguments
	 *
	 * @pre
	 * Connect using aerospike object to the specified node
	 *
	 * @post
	 * newly initialized Aerospike objects
	 *
	 * @remark
	 * Variants: OO (testAggregatePositive)
	 *
	 * @test_plans{1.1}
	 */
	function testScanInfoPositive($db,$scan_id)
    {
    if (!$db->isConnected()) {
       return($db->errorno());
    }
    $status = $db->scanInfo($scan_id,$info);
	if($info['status] == Aerospike::SCAN_STATUS_COMPLETED ||
	$info['status'] == Aerospike::SCAN_STATUS_ABORTED ||
	$info['status'] == Aerospike::SCAN_STATUS_INPROGRESS ||
	$info['status] == Aerospike::SCAN_STATUS_COMPLETED) {
	return($status);
	} else {
            return($db->errorno());
	}
    return($status);
    }
	/**
	 * @test
     * ScanInfo - Scan id is string
	 *
	 * @pre
	 * Connect using aerospike object to the specified node
	 *
	 * @post
	 * newly initialized Aerospike objects
	 *
	 * @remark
	 * Variants: OO (testAggregatePositive)
	 *
	 * @test_plans{1.1}
	 */
	function testScanInfoScanIdString($db,$scan_id)
    {
         if (!$db->isConnected()) {
            return($db->errorno());
         }
        $status = $db->scanInfo("",$info);
	if($status != Aerospike::OK) {
            return($db->errorno());
	}
    return($status);
    }
	/**
	 * @test
     * ScanInfo - Scan id is array
	 *
	 * @pre
	 * Connect using aerospike object to the specified node
	 *
	 * @post
	 * newly initialized Aerospike objects
	 *
	 * @remark
	 * Variants: OO (testAggregatePositive)
	 *
	 * @test_plans{1.1}
	 */
	function testScanInfoScanIdArray($db,$scan_id)
    {
         if (!$db->isConnected()) {
            return($db->errorno());
         }
        $status = $db->scanInfo(array($scan_id),$info);
	if($status != Aerospike::OK) {
            return($db->errorno());
	}
        return($status);
        }
	/**
	 * @test
     * ScanInfo - Info parameter is string
	 *
	 * @pre
	 * Connect using aerospike object to the specified node
	 *
	 * @post
	 * newly initialized Aerospike objects
	 *
	 * @remark
	 * Variants: OO (testAggregatePositive)
	 *
	 * @test_plans{1.1}
	 */
	function testScanInfoInfoIsString($db,$scan_id)
    {
    if (!$db->isConnected()) {
      return($db->errorno());
    }
    $status = $db->scanInfo($scan_id,"age");
	if($status != Aerospike::OK) {
      return($db->errorno());
	}
    return($status);
    }
	/**
	 * @test
     * ScanInfo - All correct arguments with optionals
	 *
	 * @pre
	 * Connect using aerospike object to the specified node
	 *
	 * @post
	 * newly initialized Aerospike objects
	 *
	 * @remark
	 * Variants: OO (testAggregatePositive)
	 *
	 * @test_plans{1.1}
	 */
	function testScanInfoWithOptionals($db,$scan_id)
    {
    if (!$db->isConnected()) {
       return($db->errorno());
    }
    $status = $db->scanInfo($scan_id,$info,array(Aerospike::OPT_WRITE_TIMEOUT=>2000));
	if($info['status] == Aerospike::SCAN_STATUS_COMPLETED ||
	$info['status'] == Aerospike::SCAN_STATUS_ABORTED ||
	$info['status'] == Aerospike::SCAN_STATUS_INPROGRESS ||
	$info['status] == Aerospike::SCAN_STATUS_COMPLETED) {
	return($status);
	} else {
    return($db->errorno());
	}
    return($status);
    }
}
?>
