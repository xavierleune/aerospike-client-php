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
    static $myscan_id = 0;
    protected function setUp(&$db, &$key) {
	    $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        if (!$db->isConnected()) {
            return $db->errorno();
        }
        $this->ensureUdfModule($db, "tests/lua/test_transform.lua", "test_transform.lua");
        $key = $db->initKey("test", "demo", "Get_key");
        $db->put($key,array("first_name"=>"raunak", "age"=>29));
        $key = $db->initKey("test", "demo", "Get_key1");
        $db->put($key,array("first_name"=>"pavan", "age"=>27));
        $key = $db->initKey("test", "demo", "Get_key2");
        $db->put($key,array("first_name"=>"bhushan", "age"=>22));
	    $key = $db->initKey("test", "demo", "Get_key3");
        $db->put($key,array("first_name"=>"ajit", "age"=>32));
    }

    protected function tearDown(&$db, &$key) {
        if (!$db->isConnected()) {
            $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
            $db = new Aerospike($config);
        }
        $key = $db->initKey("test", "demo", "Get_key");
        $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
            =>Aerospike::POLICY_RETRY_NONE));
        for( $i=1; $i<4; $i++) {
             $key=$db->initKey("test", "demo", "Get_key".$i);
             $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                 =>Aerospike::POLICY_RETRY_NONE));
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
	function testScanInfoPositive($db)
    {
        $info = array();
        $status = $db->scanApply("test", "demo", "test_transform", "mytransform", array(20), $scan_id);
        if ($status !== Aerospike::OK) {
            return($db->errorno());
        }
        ScanInfo::$myscan_id = $scan_id;
        do {
            time_nanosleep(0, 5000000); // pause 5ms
            $status = $db->scanInfo(ScanInfo::$myscan_id, $info);
            if ($status != Aerospike::OK) {
                return($db->errorno());
            }
        } while($info['status'] != Aerospike::SCAN_STATUS_COMPLETED);
        if ($info['progress_pct'] != 100) {
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
	function testScanInfoScanIdString($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        try {
            $status = $db->scanInfo("", $info);
        } catch (Exception $e) {
            return($db->errorno());
        }
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
	function testScanInfoScanIdArray($db)
    {
        $status = $db->scanApply("test", "demo", "test_transform", "mytransform", array(20), $scan_id);
        if ($status !== Aerospike::OK) {
            return($db->errorno());
        }
        ScanInfo::$myscan_id = $scan_id;
        try {
            $status = $db->scanInfo(array(ScanInfo::$myscan_id), $info);
        } catch (Exception $e) {
              return($db->errorno());
        }
    }
	/**
	 * @test
     * ScanInfo - Scan id is negative
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
	function testScanInfoScanIdNegative($db)
    {
        if (!$db->isConnected()) {
            return($db->errorno());
        }
        $status = $db->scanInfo(-1, $info);
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
	function testScanInfoInfoIsString($db)
    {
        $status = $db->scanApply("test", "demo", "test_transform", "mytransform", array(20), $scan_id);
        if ($status !== Aerospike::OK) {
            return($db->errorno());
        }
        ScanInfo::$myscan_id = $scan_id;
        $scan_info = "age";
        $status = $db->scanInfo(ScanInfo::$myscan_id, $scan_info);
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
        $status = $db->scanApply("test", "demo", "test_transform", "mytransform", array(20), $scan_id);
        if ($status !== Aerospike::OK) {
            return($db->errorno());
        }
        ScanInfo::$myscan_id = $scan_id;
        $status = $db->scanInfo(ScanInfo::$myscan_id, $info, array(Aerospike::OPT_WRITE_TIMEOUT=>2000));
        $info = array();
        do {
            time_nanosleep(0, 5000000); // pause 5ms
            $status = $db->scanInfo(ScanInfo::$myscan_id, $info);
            if ($status != Aerospike::OK) {
                return($db->errorno());
            }
        } while($info['status'] != Aerospike::SCAN_STATUS_COMPLETED);
        if ($info['progress_pct'] != 100) {
            return($db->errorno());
        }
        return($status);
    }
}
?>
