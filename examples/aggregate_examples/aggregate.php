<?php
################################################################################
# Copyright 2013-2014 Aerospike, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################
require_once(realpath(__DIR__ . '/../examples_util.php'));

function parse_args() {
    $shortopts  = "";
    $shortopts .= "h::";  /* Optional host */
    $shortopts .= "p::";  /* Optional port */
    $shortopts .= "p::";  /* Optional port */
    $shortopts .= "a";    /* Optionally annotate output with code */
    $shortopts .= "c";    /* Optionally clean up before leaving */

    $longopts  = array(
        "host::",         /* Optional host */
        "port::",         /* Optional port */
        "clean",          /* Optionally clean up before leaving */
        "annotate",       /* Optionally annotate output with code */
        "help",           /* Usage */
    );
    $options = getopt($shortopts, $longopts);
    return $options;
}

$args = parse_args();
if (isset($args["help"])) {
    echo("php bin-operations.php [-h<HOST IP ADDRESS>|--host=<HOST IP ADDRESS> -p<HOST PORT NUMBER>|--port=<HOST PORT NUMBER> -a|--annotate -c|--clean]\n");
    exit(1);
}
$HOST_ADDR = (isset($args["h"])) ? (string) $args["h"] : ((isset($args["host"])) ? (string) $args["host"] : "localhost");
$HOST_PORT = (isset($args["p"])) ? (integer) $args["p"] : ((isset($args["port"])) ? (string) $args["port"] : 3000);

echo colorize("Connecting to the host ≻", 'black', true);
$start = __LINE__;
$config = array("hosts" => array(array("addr" => $HOST_ADDR, "port" => $HOST_PORT)));
$db = new Aerospike($config, false);
if (!$db->isConnected()) {
    echo fail("Could not connect to host $HOST_ADDR:$HOST_PORT [{$db->errorno()}]: {$db->error()}");
    exit(1);
}
echo success();
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Ensuring that a record is put at test.users with PK=1234 ≻", 'black', true);
$start = __LINE__;
$key = $db->initKey("test", "users", 1234);
$put_vals = array("first_name" => "Peter", "age" => 30);
$res = $db->put($key, $put_vals);
if ($res == Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Ensuring that a record is put at test.users with PK=2345 ≻", 'black', true);
$start = __LINE__;
$key = $db->initKey("test", "users", 2345);
$put_vals = array("first_name" => "John", "age" => 35);
$res = $db->put($key, $put_vals);
if ($res == Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Ensuring that a record is put at test.users with PK=3456 ≻", 'black', true);
$start = __LINE__;
$key = $db->initKey("test", "users", 3456);
$put_vals = array("first_name" => "Alex", "age" => 26);
$res = $db->put($key, $put_vals);
if ($res == Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Ensuring that a record is put at test.users with PK=4567 ≻", 'black', true);
$start = __LINE__;
$key = $db->initKey("test", "users", 4567);
$put_vals = array("first_name" => "Jimmy", "age" => 23);
$res = $db->put($key, $put_vals);
if ($res == Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Ensuring that module is registerd  ≻", 'black', true);
$start = __LINE__;
$register_status = $db->register("lua/aggregate_udf.lua", "aggregate_udf.lua");
if ($register_status != Aerospike::OK) {
    echo standard_fail($db);
} else {
    echo success();
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Performing aggregate on records ≻", 'black', true);
$start = __LINE__;
$where = $db->predicateBetween("age", 20, 29);
$status = $db->aggregate("aggregate_udf", "group_count", array("first_name"), "test", "demo", $where, $names);
$testArray = array("Alex","Jimmy");
$result = array_diff_assoc_recursive($testArray, $names);
if(!empty($result)) {
    echo standard_fail($db);
} else {
    echo success();
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

if (isset($args['c']) || isset($args['clean'])) {
    echo "In here";
    $start = __LINE__;
    echo colorize("Removing the record ≻", 'black', true);
    $deregister_status = $db->deregister("aggregate_udf.lua");
    $key = $db->initKey("test", "users", 1234);
    $res = $db->remove($key);
    $key = $db->initKey("test", "users", 2345);
    $res = $db->remove($key);
    if ($res == Aerospike::OK) {
        echo success();
    } else {
        echo standard_fail($db);
    }
    if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);
}
$db->close();
?>
