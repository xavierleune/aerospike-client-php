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
        "annotate",       /* Optionally annotate output with code */
        "clean",          /* Optionally clean up before leaving */
        "help",           /* Usage */
    );
    $options = getopt($shortopts, $longopts);
    return $options;
}

$args = parse_args();
if (isset($args["help"])) {
    echo "php lstack.php [-hHOST] [-pPORT] [-a] [-c]\n";
    echo " or\n";
    echo "php lstack.php [--host=HOST] [--port=PORT] [--annotate] [--clean]\n";
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

echo colorize("Adding a record to test.shows with PK=Futurama ≻", 'black', true);
$start = __LINE__;
$key = $db->initKey("test", "shows", "Futurama");
$show = array("creator" => array("Matt Groening","David X Cohen"), "broadcast" => array(array(1999,2003), array(2008,2013)));
$options = array(Aerospike::OPT_POLICY_KEY => Aerospike::POLICY_KEY_SEND);
$status = $db->put($key, $show, 0, $options);
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Instantiating an LStack representing bin 'planets' of the given record ≻", 'black', true);
$start = __LINE__;
require_once(realpath(__DIR__ . '/../../autoload.php'));
$planets = new \Aerospike\LDT\LStack($db, $key, 'planets');
if ($planets->errorno() === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($planets);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Checking if the server actually has an LStack at bin 'planets' of the record ≻", 'black', true);
$start = __LINE__;
if (!$planets->isLDT()) {
    echo fail("No LStack exists yet at bin 'planets' of record {$key['key']}. Adding elements will initialize it.");
} else {
    echo success();
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Push an element onto the record's LStack bin ≻", 'black', true);
$start = __LINE__;
$status = $planets->push('Omicron Persei 8');
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($planets);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

$values = array(
"Amazonia",
"Amish Homeworld",
"Amphibios 9",
"Bogad",
"Cannibalon",
"Chapek 9",
"Cyclopia",
"Decapod 10",
"Doohan 6",
"Earth",
"Eden 7",
"Eternium",
"Globetrotter Homeworld",
"Glorianimous 3",
"Jupiter",
"Mars",
"McPluto",
"Mercury",
"Nylar 4",
"Neptune",
"Neutopia",
"Neutral Planet",
"Nintendu 64",
"Omega 3",
"Osiris 4",
"Pandora",
"Peoples α",
"Planet Vinci",
"Planet XXX",
"Planet Z-7",
"Pluto",
"Poopiter",
"Radiator planet",
"Saturn",
"Simian 7",
"Spa 5",
"Space Planet 4",
"Spheron 1",
"Stumbos 4",
"Tarantulon 6",
"Thuban 9",
"Tornadus",
"Trisol",
"Urectum",
"Vega 4",
"Venus",
"Vergon 6",
"Westminsteria",
"Wormulon",
"Zuban 5");
echo colorize("Push several other elements onto the record's LStack bin ≻", 'black', true);
$start = __LINE__;
$status = $planets->pushMany($values);
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($planets);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Getting the full record ≻", 'black', true);
$start = __LINE__;
$status = $db->get($key, $record);
if ($status === Aerospike::OK) {
    echo success();
    var_dump($record);
} elseif ($status === Aerospike::ERR_RECORD_NOT_FOUND) {
    echo fail("Could not find a show with PK={$key['key']} in the set test.shows");
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Counting the elements in the record's LStack bin ≻", 'black', true);
$start = __LINE__;
$status = $planets->size($num_elements);
if ($status === Aerospike::OK) {
    echo success();
    echo colorize("There are $num_elements elements in the 'planets' LStack\n", 'green');
} else {
    echo standard_fail($planets);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Scan the elements in the record's LStack bin ≻", 'black', true);
$start = __LINE__;
$status = $planets->scan($elements);
if ($status === Aerospike::OK) {
    echo success();
    var_dump($elements);
} else {
    echo standard_fail($planets);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Peek at top 10 elements in the LStack element ≻", 'black', true);
$start = __LINE__;
$status = $planets->peek(10, $elements);
if ($status === Aerospike::OK) {
    echo success();
    var_dump($elements);
} else {
    echo standard_fail($planets);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

if (isset($args['c']) || isset($args['clean'])) {
    $start = __LINE__;
    echo colorize("Removing the LDT ≻", 'black', true);
    $status = $planets->destroy();
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
