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
    echo "php lset.php [-hHOST] [-pPORT] [-a] [-c]\n";
    echo " or\n";
    echo "php lset.php [--host=HOST] [--port=PORT] [--annotate] [--clean]\n";
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
$futurama = array("creator" => array("Matt Groening","David X Cohen"), "broadcast" => array(array(1999,2003), array(2008,2013)));
$status = $db->put($key, $futurama);
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Instantiating an LSet representing bin 'characters' of the given record ≻", 'black', true);
$start = __LINE__;
require_once(realpath(__DIR__ . '/../../autoload.php'));
$characters = new \Aerospike\LDT\LSet($db, $key, 'characters');
if ($characters->errorno() === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($characters);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Checking if the server actually has an LSet at bin 'characters' of the record ≻", 'black', true);
$start = __LINE__;
if (!$characters->isLDT()) {
    echo fail("No LSet exists yet at bin 'characters' of record {$key['key']}. Adding elements will initialize it.");
} else {
    echo success();
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Add an element to the record's LSet bin ≻", 'black', true);
$start = __LINE__;
$status = $characters->add('Professor Hubert J. Farnsworth');
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($characters);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Add several other elements to the record's LSet bin ≻", 'black', true);
$start = __LINE__;
$planet_express_crew = array(
    "Amy Wong",
    "Hermes Conrad",
    "Dr. John A. Zoidberg",
    "Tarunga Leela",
    "Philip J. Fry",
    "Bender Bending Rodriguez",
    "Scruffy",
    "Cubert Farnsworth");
$status = $characters->addMany($planet_express_crew);
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($characters);
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

echo colorize("Trying to re-add an element that is already in the LSet bin ≻", 'black', true);
$start = __LINE__;
$status = $characters->add("Tarunga Leela");
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($characters);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Counting the elements in the record's LSet bin ≻", 'black', true);
$start = __LINE__;
$status = $characters->size($num_elements);
if ($status === Aerospike::OK) {
    echo success();
    echo colorize("There are $num_elements elements in the LSet\n", 'green');
} else {
    echo standard_fail($characters);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Find if Scruffy is a known character ≻", 'black', true);
$start = __LINE__;
$status = $characters->exists("Scruffy", $found);
if ($status === Aerospike::OK) {
    echo success();
    var_dump($found);
} else {
    echo standard_fail($characters);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Get the elements in the record's LSet bin ≻", 'black', true);
$start = __LINE__;
$status = $characters->scan($elements);
if ($status === Aerospike::OK) {
    echo success();
    var_dump($elements);
} else {
    echo standard_fail($characters);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

if (isset($args['c']) || isset($args['clean'])) {
    $start = __LINE__;
    echo colorize("Removing the LDT ≻", 'black', true);
    //$status = $characters->destroy(); // TODO: submit server bug
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
