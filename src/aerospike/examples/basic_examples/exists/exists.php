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
 * EXITS/GETMETADATA USAGE:
 * public int Aerospike::exists ( array $key, array &$metadata [, array $options ] )
 * is an alias for
 * public int Aerospike::getMetadata ( array $key, array &$metadata [, array $options ] )
 *----------------------------------------------------------------------------------------------------------------------------
 */

/*
 * EXAMPLE 1: EXISTS/METADATA PASSING NO OPTIONAL PARAMETERS
 */
$config = array("hosts"=>array(array("addr"=>$HOST_ADDR, "port"=>$HOST_PORT)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
    echo "Aerospike failed to connect to host $HOST_ADDR:$HOST_PORT [{$db->errorno()}]: {$db->error()}\n";
    exit(1);
echo "Aerospike connected to host $HOST_ADDR:$HOST_PORT\n";
$key = $db->initKey("test", "users", 1234);
$put_vals = array("email" => "hey@example.com", "name" => "Hey There");
// will ensure a record exists at the given key with the specified bins
$res = $db->put($key, $put_vals);
if ($res == Aerospike::OK) {
    echo "Record written.\n";
    // Check if the record exists.
    $res = $db->exists($key, $metadata);
    if ($res == Aerospike::OK) {
        var_dump($metadata);
    } elseif ($res == Aerospike::ERR_RECORD_NOT_FOUND) {
        echo "EXISTS failed: A user with key ". $key['key']. " does not exist in the database\n";
    } else {
        echo "EXISTS failed: [{$db->errorno()}] ".$db->error();
    }
} else {
    echo "PUT failed: [{$db->errorno()}] ".$db->error();
}

/*
 * EXAMPLE 2: EXISTS/METADATA PASSING OPTIONAL TIMEOUT POLICY
 */

if (!$db->isConnected()) {
    echo "Aerospike failed to connect to host INVALID_ADDR:INVALID_PORT [{$db->errorno()}]: {$db->error()}\n";
    $db->close();
    exit(1);
} else {
    $key = $db->initKey("test", "demo", "example_key");
    $put_vals = array("education" => array("degree"=>"B.Tech", "grade"=>"A+"),
                      "hobbies" => "playing");
    $opts = array(Aerospike::OPT_READ_TIMEOUT => 100000);
    // will ensure a record exists at the given key with the specified bins
    $res = $db->put($key, $put_vals);
    if ($res == Aerospike::OK) {
        echo "Record written.\n";
        // Check if the record exists.
        unset($metadata);
        $res = $db->exists($key, $metadata, $opts);
        if ($res == Aerospike::OK) {
            var_dump($metadata);
        } elseif ($res == Aerospike::ERR_RECORD_NOT_FOUND) {
            echo "A user with key ". $key['key']. " does not exist in the database\n";
        } else {
            echo "[{$db->errorno()}] ".$db->error();
        }
    } else {
        echo "[{$db->errorno()}] ".$db->error();
    }
}
$db->close();

?>
