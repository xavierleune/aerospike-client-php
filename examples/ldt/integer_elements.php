<?php
################################################################################
# Copyright 2013-2015 Aerospike, Inc.
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
        "annotate",       /* Optionally annotate output with code */
        "clean",          /* Optionally clean up before leaving */
        "help",           /* Usage */
    );
    $options = getopt($shortopts, $longopts);
    return $options;
}

$args = parse_args();
if (isset($args["help"])) {
    echo "php llist.php [-hHOST] [-pPORT] [-a] [-c]\n";
    echo " or\n";
    echo "php llist.php [--host=HOST] [--port=PORT] [--annotate] [--clean]\n";
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

echo colorize("Adding a record to test.numbers with PK='primes' ≻", 'black', true);
$start = __LINE__;
$key = $db->initKey("test", "numbers", "primes");
$primes = array("types" => 1);
$options = array(Aerospike::OPT_POLICY_KEY => Aerospike::POLICY_KEY_SEND);
$status = $db->put($key, $primes, 0, $options);
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Instantiating an LList representing centered triangular primes ≻", 'black', true);
$start = __LINE__;
require_once(realpath(__DIR__ . '/../../autoload.php'));
$tri_primes = new \Aerospike\LDT\LList($db, $key, 'triangular');
if ($tri_primes->errorno() === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($tri_primes);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Checking if the server actually has an LList at bin 'triangular' of the record ≻", 'black', true);
$start = __LINE__;
if (!$tri_primes->isLDT()) {
    echo fail("No LList exists yet at bin 'triangular' of record {$key['key']}. Adding elements will initialize it.");
} else {
    echo success();
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Add an element to the record's LList bin ≻", 'black', true);
$start = __LINE__;
$prime = 19;
$status = $tri_primes->add($prime);
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($tri_primes);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Add several other elements to the record's LList bin ≻", 'black', true);
$start = __LINE__;
$primes = array(31, 109, 199, 409, 571, 631, 829, 1489, 1999, 2341, 2971, 3529, 4621, 4789, 7039, 7669, 8779, 9721, 10459, 10711);
$status = $tri_primes->addMany($primes);
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($tri_primes);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Getting the full record ≻", 'black', true);
$start = __LINE__;
$status = $db->get($key, $record);
if ($status === Aerospike::OK) {
    echo success();
    var_dump($record);
} elseif ($status === Aerospike::ERR_RECORD_NOT_FOUND) {
    echo fail("Could not find a vehicle with PK={$key['key']} in the set test.vehicles");
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Counting the elements in the record's LList bin ≻", 'black', true);
$start = __LINE__;
$status = $tri_primes->size($num_elements);
if ($status === Aerospike::OK) {
    echo success();
    echo colorize("There are $num_elements elements in the LList\n", 'green');
} else {
    echo standard_fail($tri_primes);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Get the triangular primes whose value is between 100 and 200 ≻", 'black', true);
$start = __LINE__;
$status = $tri_primes->findRange(100, 200, $elements);
if ($status === Aerospike::OK) {
    echo success();
    var_dump($elements);
} else {
    echo standard_fail($tri_primes);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Find if 681 is in the triangular prime LList ≻", 'black', true);
$start = __LINE__;
$status = $tri_primes->exists(681, $res);
if ($status === Aerospike::OK) {
    echo success();
    var_dump($res);
} else {
    echo standard_fail($tri_primes);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Register a filter module that will be used with this LDT ≻", 'black', true);
$start = __LINE__;
$copied = copy(__DIR__.'/lua/keyfilters.lua', ini_get('aerospike.udf.lua_user_path').'/keyfilters.lua');
if (!$copied) {
    echo fail("Could not copy the local lua/keyfilters.lua to ". ini_get('aerospike.udf.lua_user_path'));
}
$status = $db->register(ini_get('aerospike.udf.lua_user_path').'/keyfilters.lua', "keyfilters.lua");
if ($status == Aerospike::OK) {
    echo success();
} elseif ($status == Aerospike::ERR_UDF_NOT_FOUND) {
    echo fail("Could not find the udf file lua/keyfilters.lua");
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Use the range_filter function to find values between 6000 and 8000", 'black', true);
$start = __LINE__;
$status = $tri_primes->scan($elements, 'keyfilters', 'range_filter', array(6000, 8000));
if ($status === Aerospike::OK) {
    echo success();
    var_dump($elements);
} else {
    echo standard_fail($tri_primes);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

if (isset($args['c']) || isset($args['clean'])) {
    $start = __LINE__;
    echo colorize("Removing a list of elements from the LDT ≻", 'black', true);
    $status = $tri_primes->removeMany($primes);
    if ($status === Aerospike::OK) {
        echo success();
    } else {
        echo standard_fail($db);
    }
    if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

    $start = __LINE__;
    echo colorize("Removing any elements remaining in the range between 0 and 12000 from the LDT ≻", 'black', true);
    $status = $tri_primes->removeRange(0, 12000);
    if ($status === Aerospike::OK) {
        echo success();
    } else {
        echo standard_fail($db);
    }
    if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

    $start = __LINE__;
    echo colorize("Destroying the LDT ≻", 'black', true);
    $status = $tri_primes->destroy();
    if ($status === Aerospike::OK) {
        echo success();
    } else {
        echo standard_fail($db);
    }
    if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

    $start = __LINE__;
    echo colorize("Removing the record ≻", 'black', true);
    $status = $db->remove($key);
    if ($status === Aerospike::OK) {
        echo success();
    } else {
        echo standard_fail($db);
    }
    if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

}

$db->close();
?>
