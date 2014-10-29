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
    echo "php bin-operations.php [-hHOST] [-pPORT] [-a] [-c]\n";
    echo " or\n";
    echo "php bin-operations.php [--host=HOST] [--port=PORT] [--annotate] [--clean]\n";
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
$put_vals = array("email" => "freudian.circuits@hal-inst.org", "name" => "Perceptron");
$res = $db->put($key, $put_vals);
if ($res == Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Getting the record ≻", 'black', true);
$start = __LINE__;
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

echo colorize("Prepending the string value 'Doctor ' value to the 'name' bin ≻", 'black', true);
$start = __LINE__;
$res = $db->prepend($key, 'name', 'Doctor ');
if ($res == Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Appending the string value ' MD' to the 'name' bin ≻", 'black', true);
$start = __LINE__;
$res = $db->append($key, 'name', ' MD');
if ($res == Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Getting the modified 'name' bin of the record ≻", 'black', true);
$start = __LINE__;
$res = $db->get($key, $record, array('name'));
if ($res == Aerospike::OK) {
    echo success();
    var_dump($record);
} elseif ($res == Aerospike::ERR_RECORD_NOT_FOUND) {
    echo fail("Could not find a user with PK={$key['key']} in the set test.users");
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Setting a 'patients_cured' bin on the record to (int) 3 using Aerospike::put() ≻", 'black', true);
$start = __LINE__;
$put_vals = array("patients_cured" => 3);
$res = $db->put($key, $put_vals);
if ($res == Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Incrementing the 'patients_cured' bin by (int) -1 ≻", 'black', true);
$start = __LINE__;
$res = $db->increment($key, 'patients_cured', -1);
if ($res == Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Getting the bins 'email' and 'patients_cured' from the record ≻", 'black', true);
$start = __LINE__;
$res = $db->get($key, $record, array('email', 'patients_cured'));
if ($res == Aerospike::OK) {
    echo success();
    var_dump($record);
} elseif ($res == Aerospike::ERR_RECORD_NOT_FOUND) {
    echo fail("Could not find a user with PK={$key['key']} in the set test.users");
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Removing the 'patients_cured' bin ≻", 'black', true);
$start = __LINE__;
$res = $db->removeBin($key, array('patients_cured'));
if ($res == Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Getting the full record ≻", 'black', true);
$start = __LINE__;
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

echo colorize("Ensuring that another record is put at test.users with PK=2345 ≻", 'black', true);
$start = __LINE__;
$key = $db->initKey("test", "users", 2345);
$put_vals = array("email" => "steve.circuits@hal-inst.org", "name" => "Steve", "patients_cured" => 100);
$res = $db->put($key, $put_vals);
if ($res == Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Getting the full record ≻", 'black', true);
$start = __LINE__;
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

echo colorize("Performing multiple operations:\nprepending string value 'Doctor ' value to the 'name' bin and\nincrementing the 'patients_cured' bin by (int) 1 and Reading back the bin 'name' ≻≻", 'black', true);
$start = __LINE__;
$operations = array(
                array("op" => Aerospike::OPERATOR_PREPEND, "bin" => "name", "val" => "Doctor "),
                array("op" => Aerospike::OPERATOR_INCR, "bin" => "patients_cured", "val" => 1),
                array("op" => Aerospike::OPERATOR_READ, "bin" => "name")
            );
$res = $db->operate($key, $operations, $returned);
if ($res == Aerospike::OK) {
    echo "\nRead bin 'name':\n";
    var_dump($returned);
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Getting the full record ≻", 'black', true);
$start = __LINE__;
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
