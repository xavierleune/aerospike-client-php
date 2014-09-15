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
    echo("php simple.php [-h<HOST IP ADDRESS>|--host=<HOST IP ADDRESS> -p<HOST PORT NUMBER>|--port=<HOST PORT NUMBER> -a|--annotate -c|--clean]\n");
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

echo colorize("Ensuring that records are put at test.users with PKs=2345-2349 ≻", 'black', true);
$start = __LINE__;
$emails = array("peter.john@hal-inst.org",
    "freudian.circuits@hal-inst.org",
    "steve.circuits@hal-inst.org",
    "wolverine.john@hal-inst.org",
    "amelia.johnson@hal-inst.org");
$ages = array(26, 30, 35, 39, 45);
for ($i = 2345, $j = 0; $i < 2350; $i++, $j++) {
    $key = $db->initKey("test", "users", $i);
    $put_vals = array("email" => $emails[$j], "age" => $ages[$j]);
    $res = $db->put($key, $put_vals);
    if ($res == Aerospike::OK) {
        echo success();
    } else {
        echo standard_fail($db);
    }
}

if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Querying records with ages between 30 and 39≻", 'black', true);
$start = __LINE__;
$total = 0;
$in_thirties = 0;
$where = $db->predicateBetween("age", 30, 39);
$status = $db->query("test", "users", $where, function ($record) {
    global $total, $in_thirties;
    if (array_key_exists('email', $record) && !is_null($record['email'])) {
            echo "\nFound record with email-id: " . $record['email'] . "and age: " . $record['age'];
        }
        $total += (int) $record['age'];
        $in_thirties++;
}, array("email", "age"));
if ($status == Aerospike::OK && $total) {
    echo "\nThe average age of employees in their thirties is ".round($total / $in_thirties)."\n";
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

if (isset($args['a']) || isset($args['clean'])) {
    $start = __LINE__;
    echo colorize("Removing the records ≻", 'black', true);
    $res = true;
    for ($i = 2345; $i < 2350; $i++) {
        $key = $db->initKey("test", "users", $i);
        $res &= ((Aerospike::OK == $db->remove($key)) ? true : false);
    }
    if ($res) {
        echo success();
    } else {
        echo standard_fail($db);
    }
    if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);
}
$db->close();
?>
