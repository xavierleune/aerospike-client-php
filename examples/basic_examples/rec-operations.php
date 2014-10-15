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
    echo "php rec-operations.php [-hHOST] [-pPORT] [-a] [-c]\n";
    echo " or\n";
    echo "php rec-operations.php [--host=HOST] [--port=PORT] [--annotate] [--clean]\n";
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

echo colorize("Clear out the record that may exist at test.users with PK=1 ≻", 'black', true);
$start = __LINE__;
$key = $db->initKey("test", "users", 1);
$res = $db->remove($key);
if ($res == Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Inserting a new record into test.users at PK=1 ≻", 'black', true);
$start = __LINE__;
$needs_force = false;
$put_vals = array(
    "title" => "Professor",
    "name" => "Hubert Farnsworth",
    "age" => 150,
    "is_senior" => true,
    "positions" => array("Inventor", "Founder", "Lecturer", "Scientist", "Hyper Yeti"));
$res = $db->put($key, $put_vals, 300, array(Aerospike::OPT_POLICY_EXISTS => Aerospike::POLICY_EXISTS_CREATE));
if ($res == Aerospike::OK) {
    echo success();
} elseif ($res == Aerospike::ERR_RECORD_EXISTS) {
    echo fail("There already is a record at PK={$key['key']} in the set test.users");
    $needs_force = true;
} else {
    echo standard_fail($db);
    $needs_force = true;
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

if ($needs_force) {
    echo colorize("Retry inserting a record into test.users at PK=1 using Aerospike::POLICY_EXISTS_IGNORE ≻", 'black', true);
    $start = __LINE__;
    $res = $db->put($key, $put_vals, 300, array(Aerospike::OPT_POLICY_EXISTS => Aerospike::POLICY_EXISTS_IGNORE));
    if ($res == Aerospike::OK) {
        echo success();
    } else {
        echo standard_fail($db);
        exit(2);
    }
    if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);
}

echo colorize("Inserting a new record into test.users at PK=2 ≻", 'black', true);
$start = __LINE__;
$needs_force = false;
$put_record = array("bin1"=>10);
$key = $db->initKey("test", "users", 2);
$res = $db->put($key, $put_record, NULL);

if ($res == Aerospike::OK) {
    echo success();
} elseif ($res == Aerospike::ERR_RECORD_EXISTS) {
    echo fail("There already is a record at PK={$key['key']} in the set test.users");
    $needs_force = true;
} else {
    echo standard_fail($db);
    $needs_force = true;
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Inserting a new record into test.users at PK=2 and generation policy ≻", 'black', true);
$start = __LINE__;
$needs_force = false;
$key = $db->initKey("test", "users", 2);
$exists_status = $db->exists($key, $metadata);
if ($exists_status != AEROSPIKE::OK) {
    $db->close();
    return($exists_status);
}
$gen_value = $metadata["generation"] + 10;
$put_record = array("bin1"=>10);
$res = $db->put($key, $put_record, NULL, array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT,$gen_value)));
if ($res == Aerospike::OK) {
    echo success();
} elseif ($res == Aerospike::ERR_RECORD_EXISTS) {
    echo fail("There already is a record at PK={$key['key']} in the set test.users");
    $needs_force = true;
} else {
    echo standard_fail($db);
    $needs_force = true;
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Inserting a new record into test.users at PK=3 and digest policy ≻", 'black', true);
$start = __LINE__;
$needs_force = false;
$key = $db->initKey("test", "demo", base64_decode("jhj56888"), 1);
$res = $db->put($key, array("k1"=>10, "k2"=>20), NULL, array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));

if ($res == Aerospike::OK) {
    echo success();
} elseif ($res == Aerospike::ERR_RECORD_EXISTS) {
    echo fail("There already is a record at PK={$key['key']} in the set test.users");
    $needs_force = true;
} else {
    echo standard_fail($db);
    $needs_force = true;
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Getting the record ≻", 'black', true);
$start = __LINE__;
$key = $db->initKey("test", "users", 1);
$res = $db->get($key, $record);
if ($res == Aerospike::OK) {
    echo success();
    var_dump($record);
} elseif ($res == Aerospike::ERR_RECORD_NOT_FOUND) {
    echo fail("Could not find a user with PK={$key['key']} in the set test.users");
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Retrieving record metadata ≻", 'black', true);
$start = __LINE__;
$key = $db->initKey("test", "users", 1);
$res = $db->exists($key, $metadata);
if ($res == Aerospike::OK) {
    echo success();
    var_dump($metadata);
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Updating the record using Aerospike::POLICY_EXISTS_UPDATE ≻", 'black', true);
$start = __LINE__;
$key = $db->initKey("test", "users", 1);
$put_vals = array(
    "eyesight" => "bad",
    "age" => 160,
    "locations" => array(
        "Mars" => "Mars University",
        "Earth" => "Planet Express, New New York",
        "Near Death Star" => "Nobody Knows!"));
$res = $db->put($key, $put_vals, 300, array(Aerospike::OPT_POLICY_EXISTS => Aerospike::POLICY_EXISTS_UPDATE));
if ($res == Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Getting the record ≻", 'black', true);
$start = __LINE__;
$key = $db->initKey("test", "users", 1);
$res = $db->get($key, $record);
if ($res == Aerospike::OK) {
    echo success();
    var_dump($record);
} elseif ($res == Aerospike::ERR_RECORD_NOT_FOUND) {
    echo fail("Could not find a user with PK={$key['key']} in the set test.users");
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Retrieving record metadata ≻", 'black', true);
$start = __LINE__;
$key = $db->initKey("test", "users", 1);
$res = $db->exists($key, $metadata);
if ($res == Aerospike::OK) {
    echo success();
    var_dump($metadata);
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Touching the record ≻", 'black', true);
$start = __LINE__;
$res = $db->touch($key, 500);
if ($res == Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Retrieving record metadata ≻", 'black', true);
$start = __LINE__;
$res = $db->exists($key, $metadata);
if ($res == Aerospike::OK) {
    echo success();
    var_dump($metadata);
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

if (isset($args['c']) || isset($args['clean'])) {
    echo colorize("Removing the record ≻ ", 'black', true);
    $start = __LINE__;
    $res = $db->remove($key);
    if ($res == Aerospike::OK) {
        echo success();
    } else {
        echo standard_fail($db);
    }
    $key = $db->initKey("test", "users", 2);
    if ($res == Aerospike::OK) {
        echo success();
    } else {
        echo standard_fail($db);
    }
    if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);
}

$db->close();
?>
