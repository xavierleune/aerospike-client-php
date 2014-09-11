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
    echo("php udf-operations.php [-h<HOST IP ADDRESS>|--host=<HOST IP ADDRESS> -p<HOST PORT NUMBER>|--port=<HOST PORT NUMBER> -a|--annotate -c|--clean]\n");
    exit(1);
}
$HOST_ADDR = (isset($args["h"])) ? (string) $args["h"] : ((isset($args["host"])) ? (string) $args["host"] : "localhost");
$HOST_PORT = (isset($args["p"])) ? (integer) $args["p"] : ((isset($args["port"])) ? (string) $args["port"] : 3000);
$UDF_FILE  = __DIR__ . '/lua/basic_udf.lua';
$UDF_MODULE = 'basic_udf';
$UDF_MODULE_TO_REGISTER =  $UDF_MODULE . '.lua';

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

echo colorize("Ensuring that a UDF is registered ≻", 'black', true);
$start = __LINE__;
$res = $db->register($UDF_FILE, $UDF_MODULE_TO_REGISTER);
if ($res == Aerospike::OK) {
    echo success();
} elseif ($res == Aerospike::ERR_UDF_NOT_FOUND) {
    echo fail("Could not find the udf file {$UDF_FILE}");
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Ensuring that a record is put at test.users with PK=1234 ≻", 'black', true);
$start = __LINE__;
$key = $db->initKey("test", "users", 1234);
$put_vals = array("email" => "freudian.circuits@hal-inst.org", "name" => "Perceptron", "age" => 30);
$res = $db->put($key, $put_vals);
if ($res == Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Applying a simple UDF on the record with PK=1234 to increment age by 10 ≻", 'black', true);
$start = __LINE__;
$res = $db->apply($key, $UDF_MODULE, "bin_age_add_10");
if ($res == Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Applying a UDF with arguments and return value on the record with PK=1234 to update age and return the new age ≻", 'black', true);
$start = __LINE__;
$res = $db->apply($key, $UDF_MODULE, "bin_transform", array("age", 2, 20), $ret_val);
if ($res == Aerospike::OK) {
    echo success();
    var_dump($ret_val);
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Ensuring that the UDF is deregistered ≻", 'black', true);
$start = __LINE__;
$res = $db->deregister($UDF_MODULE_TO_REGISTER);
if ($res == Aerospike::OK) {
    echo success();
} elseif ($res == Aerospike::ERR_UDF_NOT_FOUND) {
    echo fail("Could not find the udf module {$UDF_MODULE_TO_REGISTER}");
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

if (isset($args['c']) || isset($args['clean'])) {
    $start = __LINE__;
    echo colorize("Removing the record ≻", 'black', true);
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
