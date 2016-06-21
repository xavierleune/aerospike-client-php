<?php

require_once 'Common.inc';
require_once(realpath(__DIR__ . '/../../../autoload.php'));
/**
 *Basic Geospatial tests
 */

class Employee
{
    public $desg = 'Manager';
}

class Geospatial extends AerospikeTestCommon
{
    protected function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        if (!$this->db->isConnected()) {
           return $this->db->errorno();
        }
    }

    /**
     * @test
     * Testing fromArray api of the Aerospike\GeoJSON class.
     * When passed a array as parameter it should return GeoJson Object.
     *
     * @remark
     * Variants: OO (fromArray)
     *
     * @test_plans{1.1}
     */
    function fromArray()
    {
        $arr = array("type"=>"Point", "coordinates"=>array(77.608389,  -77.604333));
        $geo_loc = \Aerospike\GeoJSON\GeoJSON::fromArray($arr);

        if (gettype($geo_loc) == "object") {
            if (get_class($geo_loc) == "Aerospike\GeoJSON\GeoJSON") {
                return AEROSPIKE::OK;
            } else {
                return AEROSPIKE::ERR_CLIENT;
            }
        } else {
            return AEROSPIKE::ERR_CLIENT;
        }
        return AEROSPIKE::OK;
    }

    /**
     * @test
     * Testing fromJson api of the Aerospike\GeoJSON class.
     * When passed a array as parameter it should return GeoJson Object.
     *
     * @remark
     * Variants: OO (fromJson)
     *
     * @test_plans{1.1}
     */
    function fromJson()
    {
        $jStr = "{\"type\": \"Point\", \"coordinates\": [23.608389, -89.604333]}";
        $geo_loc = \Aerospike\GeoJSON\GeoJSON::fromJSON((string)$jStr);

        if (gettype($geo_loc) == "object") {
            if (get_class($geo_loc) == "Aerospike\GeoJSON\GeoJSON") {
                return AEROSPIKE::OK;
            } else {
                return AEROSPIKE::ERR_CLIENT;
            }
        } else {
            return AEROSPIKE::ERR_CLIENT;
        }
        return AEROSPIKE::OK;
    }

    /**
     * @test
     * Testing get and put api of the Aerospike\GeoJSON type.
     * Expected to put the GeoJSON object as it is in case of new servers where GeoJSON is supported.
     * And convert into byte stream where GeoJSON is not supported.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (geojsonPut)
     *
     * @test_plans{1.1}
     */
    function geojsonPutGet()
    {
        $jStr = "{\"type\": \"Point\", \"coordinates\": [23.608389, -89.604333]}";
        $geo_loc = \Aerospike\GeoJSON\GeoJSON::fromJSON((string)$jStr);
        $key = $this->db->initKey("test", "demo", "geo1");
        $bins = ["tstamp"=> time(),
            "userid" => 12345,
            "loc"    => $geo_loc];

        $status = $this->db->put($key, $bins);

        if ($status !== Aerospike::OK) {
            return $this->db->errorno();
        }

        $status = $this->db->get($key, $returned);
        if ($status !== Aerospike::OK) {
            return $this->db->errorno();
        }

        if (!($returned['bins']['tstamp'] == $bins['tstamp'] &&
              $returned['bins']['userid'] == $bins['userid'] &&
              $returned['bins']['loc']    == $bins['loc'])) {
                return Aerospike::ERR_CLIENT;
        }
        return Aerospike::OK;
    }

    /**
     * @test
     * Testing predicateGeoWithinGeoJSONRegion api of the Aerospike\GeoJSON type.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateGeoWithinGeoJSONRegion)
     *
     * @test_plans{1.1}
     */
    function testPredicateGeoWithinGeoJSONRegion() {
        $jStr = "{\"type\": \"Polygon\", \"coordinates\":
            [28.60000, -80.590000],
            [28.61800, -80.590000],
            [28.61800, -80.620000],
            [28.600000,-80.620000]}";
        $predicate = $this->db->predicateGeoWithinGeoJSONRegion("loc", (string) $jStr);
        $this->assertArrayHasKey('op', $predicate, $msg = "Predicate missing 'op' key.");
        $this->assertArrayHasKey('bin', $predicate, $msg = "Predicate missing 'bin' key.");
        $this->assertArrayHasKey('val', $predicate, $msg = "Predicate missing 'val' key.");
        return Aerospike::OK;
    }

    /**
     * @test
     * Testing predicateGeoWithinRadius api of the Aerospike\GeoJSON type.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateGeoWithinRadius)
     *
     * @test_plans{1.1}
     */
    function testPredicateGeoWithinRadius() {
        $long = 78.99;
        $lat  = 80.77;
        $radiusMeter = 1000;
        $predicate = $this->db->predicateGeoWithinRadius("loc", $long, $lat, $radiusMeter);
        $this->assertArrayHasKey('op', $predicate, $msg = "Predicate missing 'op' key.");
        $this->assertArrayHasKey('bin', $predicate, $msg = "Predicate missing 'bin' key.");
        $this->assertArrayHasKey('val', $predicate, $msg = "Predicate missing 'val' key.");
        return Aerospike::OK;
    }

    /**
     * @test
     * Testing predicateGeoContainsGeoJSONPoint api of the Aerospike\GeoJSON type.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateGeoContainsGeoJSONPoint)
     *
     * @test_plans{1.1}
     */
    function testPredicateGeoContainsGeoJSONPoint() {
        $jStr = "{\"type\": \"Polygon\", \"coordinates\":
            [28.60000, -80.590000],
            [28.61800, -80.590000],
            [28.61800, -80.620000],
            [28.600000,-80.620000]}";
        $predicate = $this->db->predicateGeoContainsGeoJSONPoint("loc", $jStr);
        $this->assertArrayHasKey('op', $predicate, $msg = "Predicate missing 'op' key.");
        $this->assertArrayHasKey('bin', $predicate, $msg = "Predicate missing 'bin' key.");
        $this->assertArrayHasKey('val', $predicate, $msg = "Predicate missing 'val' key.");
        return Aerospike::OK;
    }

    /**
     * @test
     * Testing predicateGeoContainsPoint api of the Aerospike\GeoJSON type.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPredicateGeoContainsPoint)
     *
     * @test_plans{1.1}
     */
    function testPredicateGeoContainsPoint() {
        $long = 78.99;
        $lat  = 80.77;
        $radiusMeter = 1000;
        $predicate = $this->db->predicateGeoContainsPoint("loc", $long, $lat, $radiusMeter);
        $this->assertArrayHasKey('op', $predicate, $msg = "Predicate missing 'op' key.");
        $this->assertArrayHasKey('bin', $predicate, $msg = "Predicate missing 'bin' key.");
        $this->assertArrayHasKey('val', $predicate, $msg = "Predicate missing 'val' key.");
        return Aerospike::OK;
    }

    /**
     * @test
     * Testing Operate Write for GeoJSON data type.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO ()
     *
     * @test_plans{1.1}
     */
    function testOperateWrite() {
        $lat = 28.608389;
        $long = -80.604333;
        $geo_obj = new stdClass();
        $geo_obj->type = "Point";
        $geo_obj->coordinates = [$long, $lat];
        $loc = new \Aerospike\GeoJSON\GeoJSON($geo_obj);
        $operations = array(
            array("op" => Aerospike::OPERATOR_WRITE, "bin" => "tstamp", "val"=> time()),
            array("op" => Aerospike::OPERATOR_WRITE, "bin" => "userid", "val"=> 4567),
            array("op" => Aerospike::OPERATOR_WRITE, "bin" => "loc", "val"=> $loc),
        );
        $returned = array();
        $key = $this->db->initKey("test", "demo", "geo_operate");
        $status = $this->db->operate($key, $operations, $returned);
        if($status != Aerospike::OK)
        {
            return $status;
        }
        return Aerospike::OK;
    }
}
