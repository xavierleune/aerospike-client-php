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
 * CONNECT USAGE:
 * public int Aerospike::__construct ( array $config [, string $persistent_connection [, array $options]] )
 *----------------------------------------------------------------------------------------------------------------------------
 */

/*
 * EXAMPLE 1: PASSING VALID CONFIG, OPTIONAL PERSISTENT CONNECTION FLAG
 * AND OPTIONAL TIMEOUT PARAMETERS
 */
$config1 = array("hosts"=>array(array("addr"=>$HOST_ADDR, "port"=>$HOST_PORT)));
$opts = array(Aerospike::OPT_CONNECT_TIMEOUT => 1250, Aerospike::OPT_WRITE_TIMEOUT => 1500, Aerospike::OPT_READ_TIMEOUT => 1500);
$db1 = new Aerospike($config1, true, $opts);
if (!$db1->isConnected()) {
    echo "Aerospike failed to connect to host $HOST_ADDR:$HOST_PORT [{$db->errorno()}]: {$db->error()}\n";
} else {
    echo "Aerospike connected to host $HOST_ADDR:$HOST_PORT\n";
    $db1->close();
}

/*
 * EXAMPLE 2: PASSING INVALID HOST/PORT IN CONFIG AND NO OPTIONAL PARAMETERS
 */
$config2 = array("hosts"=>array(array("addr"=>"INVALID_ADDR", "port"=>0)));
$db2 = new Aerospike($config2);
if (!$db2->isConnected()) {
    echo "Aerospike failed to connect to host INVALID_ADDR:INVALID_PORT [{$db2->errorno()}]: {$db2->error()}\n";
} else {
    echo "Aerospike connected to host INVALID_ADDR:INVALID_PORT (?!)\n";
    $db2->close();
}

/*
 * EXAMPLE 3: PASSING VALID CONFIG AND OPTIONAL TIMEOUT PARAMETERS
 */
$config3 = array("hosts"=>array(array("addr"=>$HOST_ADDR, "port"=>$HOST_PORT)));
$opts = array(Aerospike::OPT_CONNECT_TIMEOUT => 1250);
$db3 = new Aerospike($config3, false, $opts);
if (!$db3->isConnected()) {
    echo "Aerospike failed to connect to host $HOST_ADDR:$HOST_PORT [{$db3->errorno()}]: {$db3->error()}\n";
} else {
    echo "Aerospike connected to host $HOST_ADDR:$HOST_PORT\n";
    $db3->close();
}

/*
 * EXAMPLE 4: PASSING VALID CONFIG AND ONLY OPTIONAL PERSISTENT CONNECTION
 */
$config4 = array("hosts"=>array(array("addr"=>$HOST_ADDR, "port"=>$HOST_PORT)));
$db4 = new Aerospike($config4, true);
if (!$db4->isConnected()) {
    echo "Aerospike failed to connect to host $HOST_ADDR:$HOST_PORT [{$db4->errorno()}]: {$db4->error()}\n";
} else {
    echo "Aerospike connected to host $HOST_ADDR:$HOST_PORT\n";
    $db3->close();
}

?>
