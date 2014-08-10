<?php

/*
 * FUNCTION TO PARSE COMMAND LINE ARGS TO SCRIPT
 */

function parse_args() {
    $shortopts  = "";
    $shortopts .= "h::";  /* Optional host */
    $shortopts .= "p::";  /* Optional port */

    $longopts  = array(
        "host::",         /* Optional host */
        "port::",         /* Optional port */
        "help",           /* Usage */
    );

    $options = getopt($shortopts, $longopts);
    return $options;
}


    $args = parse_args();

    if (isset($args["help"])) {
        echo("php connect.php [-h<HOST IP ADDRESS>|--host=<HOST IP ADDRESS> -p<HOST PORT NUMBER>|--port=<HOST PORT NUMBER>]\n");
        exit(1);
    }

    $HOST_ADDR = (isset($args["h"])) ? (string) $args["h"] : ((isset($args["host"])) ? (string) $args["host"] : "localhost");
    $HOST_PORT = (isset($args["p"])) ? (integer) $args["p"] : ((isset($args["port"])) ? (string) $args["port"] : 3000);


    /*----------------------------------------------------------------------------------------------------------------------------
     * CONNECT USAGE:
     * public int Aerospike::__construct ( array $config [, string $persistence_alias [, array $options]] )
     *----------------------------------------------------------------------------------------------------------------------------
     */

    /*
     * EXAMPLE 1: PASSING VALID CONFIG, OPTIONAL PERSISTENCE_ALIAS AND OPTIONAL 
     * TIMEOUT PARAMETERS
     */

    $config1 = array("hosts"=>array(array("addr"=>$HOST_ADDR, "port"=>$HOST_PORT)));
    $opts = array(Aerospike::OPT_CONNECT_TIMEOUT => 1250, Aerospike::OPT_WRITE_TIMEOUT => 1500, Aerospike::OPT_READ_TIMEOUT => 1500);
    $db1 = new Aerospike($config1, 'prod-db', $opts);
    if (!$db1->isConnected()) {
        echo "Aerospike failed to connect to host $HOST_ADDR:$HOST_PORT [{$db1->errorno()}]: {$db1->error()}\n";
    } else {
        echo "Aerospike connection to host $HOST_ADDR:$HOST_PORT successful\n";
    }

    /*
     * EXAMPLE 2: PASSING INVALID HOST/PORT IN CONFIG AND NO OPTIONAL PARAMETERS
     */

    $config2 = array("hosts"=>array(array("addr"=>"INVALID_ADDR", "port"=>0)));
    $db2 = new Aerospike($config2);
    if (!$db2->isConnected()) {
        echo "Aerospike failed to connect to host INVALID_ADDR:INVALID_PORT [{$db2->errorno()}]: {$db2->error()}\n";
    } else {
        echo "Aerospike connection to host INVALID_ADDR:INVALID_PORT successful\n";
    }
 
    /*
     * EXAMPLE 3: PASSING VALID CONFIG AND ONLY OPTIONAL TIMEOUT PARAMETERS
     */

    $config3 = array("hosts"=>array(array("addr"=>$HOST_ADDR, "port"=>$HOST_PORT)));
    $opts = array(Aerospike::OPT_CONNECT_TIMEOUT => 1250);
    $db3 = new Aerospike($config3, NULL, $opts);
    if (!$db3->isConnected()) {
        echo "Aerospike failed to connect to host $HOST_ADDR:$HOST_PORT [{$db3->errorno()}]: {$db3->error()}\n";
    } else {
        echo "Aerospike connection to host $HOST_ADDR:$HOST_PORT successful\n";
    }

    /*
     * EXAMPLE 4: PASSING VALID CONFIG AND ONLY OPTIONAL PERSISTENCE_ALIAS
     */

    $config4 = array("hosts"=>array(array("addr"=>$HOST_ADDR, "port"=>$HOST_PORT)));
    $db4 = new Aerospike($config4, "my-db");
    if (!$db4->isConnected()) {
        echo "Aerospike failed to connect to host $HOST_ADDR:$HOST_PORT [{$db4->errorno()}]: {$db4->error()}\n";
    } else {
        echo "Aerospike connection to host $HOST_ADDR:$HOST_PORT successful\n";
    }

?>
