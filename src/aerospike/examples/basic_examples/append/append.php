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

function parse_args() {
    $shortopts  = "";
    $shortopts .= "h::";  /* Optional host */
    $shortopts .= "p::";  /* Optional port */
    $shortopts .= "p::";  /* Optional port */
    $shortopts .= "c";    /* Optionally clean up before leaving */

    $longopts  = array(
        "host::",         /* Optional host */
        "port::",         /* Optional port */
        "clean",          /* Optionally clean up before leaving */
        "help",           /* Usage */
    );

    $options = getopt($shortopts, $longopts);
    return $options;
}

$args = parse_args();
if (isset($args["help"])) {
    echo("php append.php [-h<HOST IP ADDRESS>|--host=<HOST IP ADDRESS> -p<HOST PORT NUMBER>|--port=<HOST PORT NUMBER>]\n");
    exit(1);
}
$HOST_ADDR = (isset($args["h"])) ? (string) $args["h"] : ((isset($args["host"])) ? (string) $args["host"] : "localhost");
$HOST_PORT = (isset($args["p"])) ? (integer) $args["p"] : ((isset($args["port"])) ? (string) $args["port"] : 3000);

/*----------------------------------------------------------------------------------------------------------------------------
 * APPEND USAGE:
 * public int Aerospike::append ( array $key, string $bin, string $value [, array $options ] )
 *----------------------------------------------------------------------------------------------------------------------------
 */

/*
 * EXAMPLE 1: APPEND PASSING NO OPTIONAL PARAMETERS
 */

$config = array("hosts"=>array(array("addr"=>$HOST_ADDR, "port"=>$HOST_PORT)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
    echo "Aerospike failed to connect to host $HOST_ADDR:$HOST_PORT [{$db->errorno()}]: {$db->error()}\n";
    $db->close();
    exit(1);
}
echo "Aerospike connected to host $HOST_ADDR:$HOST_PORT\n";
$key = $db->initKey("test", "users", 1234);
$put_vals = array("email" => "hey@example.com", "name" => "Hey There");
// will ensure a record exists at the given key with the specified bins
$res = $db->put($key, $put_vals);
if ($res == Aerospike::OK) {
    echo "Record written.\n";
    // append a value to a string bin.
    $res = $db->append($key, 'name', ' Ph.D.');
    if ($res == Aerospike::OK) {
        echo "Added the Ph.D. suffix to the user.\n";
        // clean-up
        if (isset($args['c']) || isset($args['clean'])) {
            $db->remove($key);
        }
    } else {
        echo "APPEND failed: [{$db->errorno()}] ".$db->error()."\n";
    }
} else {
    echo "PUT failed: [{$db->errorno()}] ".$db->error()."\n";
}

/*
 * EXAMPLE 2: APPEND PASSING OPTIONAL TIMEOUT AND RETRY POLICIES
 */
$key = $db->initKey("test", "demo", "example_key");
$put_vals = array("education" => array("degree"=>"B.Tech", "grade"=>"A+"),
                  "hobbies" => "playing");
$opts = array(Aerospike::OPT_POLICY_RETRY => Aerospike::POLICY_RETRY_ONCE,
              Aerospike::OPT_WRITE_TIMEOUT => 5000);
// will ensure a record exists at the given key with the specified bins
$res = $db->put($key, $put_vals);
if ($res == Aerospike::OK) {
    echo "Record written.\n";
    // append a value to a string bin.
    $res = $db->append($key, 'hobbies', ' cricket');
    if ($res == Aerospike::OK) {
        echo "Added the cricket to hobbies.\n";
        // clean-up
        if (isset($args['c']) || isset($args['clean'])) {
            $db->remove($key);
        }
    } else {
        echo "APPEND failed: [{$db->errorno()}] ".$db->error();
    }
} else {
    echo "PUT failed: [{$db->errorno()}] ".$db->error();
}
$db->close();

?>
