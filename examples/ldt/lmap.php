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
    echo "php lmap.php [-hHOST] [-pPORT] [-a] [-c]\n";
    echo " or\n";
    echo "php lmap.php [--host=HOST] [--port=PORT] [--annotate] [--clean]\n";
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

echo colorize("Instantiating an LMap representing bin 'episodes' of the given record ≻", 'black', true);
$start = __LINE__;
require_once(realpath(__DIR__ . '/../../autoload.php'));
$episodes = new \Aerospike\LDT\LMap($db, $key, 'episodes');
if ($episodes->errorno() === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($episodes);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Checking if the server actually has an LMap at bin 'episodes' of the record ≻", 'black', true);
$start = __LINE__;
if (!$episodes->isLDT()) {
    echo fail("No LMap exists yet at bin 'episodes' of record {$key['key']}. Adding elements will initialize it.");
} else {
    echo success();
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Add an element to the record's LMap bin ≻", 'black', true);
$start = __LINE__;
$status = $episodes->put('S01E01', 'Space Pilot 3000');
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($episodes);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Add several other elements to the record's LMap bin ≻", 'black', true);
$start = __LINE__;
$first_season = array(
    "S01E02" => "The Series Has Landed",
    "S01E03" => "I, Roomate",
    "S01E04" => "Love's Labours Lost in Space",
    "S01E05" => "Fear of A Bot Planet",
    "S01E06" => "A Fishful of Dollars",
    "S01E07" => "My Three Suns",
    "S01E08" => "A Big Piece of Garbage",
    "S01E09" => "Hell is Other Robots",
    "S02E01" => "A Flight to Remember",
    "S02E02" => "Mars University",
    "S02E03" => "When Aliens Attack",
    "S02E04" => "Fry and the Slurm Factory",
);
$status = $episodes->putMany($first_season);
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($episodes);
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

echo colorize("Trying to re-add an element that is already in the LMap bin ≻", 'black', true);
$start = __LINE__;
$status = $episodes->put("S01E05", "Fear of a Bot Planet");
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($episodes);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Counting the elements in the record's LMap bin ≻", 'black', true);
$start = __LINE__;
$status = $episodes->size($num_elements);
if ($status === Aerospike::OK) {
    echo success();
    echo colorize("There are $num_elements elements in the LMap\n", 'green');
} else {
    echo standard_fail($episodes);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Get the elements in the record's LMap bin ≻", 'black', true);
$start = __LINE__;
$status = $episodes->scan($elements);
if ($status === Aerospike::OK) {
    echo success();
    var_dump($elements);
} else {
    echo standard_fail($episodes);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

if (isset($args['c']) || isset($args['clean'])) {
    $start = __LINE__;
    echo colorize("Removing the LDT ≻", 'black', true);
    //$status = $episodes->destroy(); // TODO: submit server bug
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
