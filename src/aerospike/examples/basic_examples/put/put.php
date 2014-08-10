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
        echo("php put.php [-h<HOST IP ADDRESS>|--host=<HOST IP ADDRESS> -p<HOST PORT NUMBER>|--port=<HOST PORT NUMBER>]\n");
        exit(1);
    }

    $HOST_ADDR = (isset($args["h"])) ? (string) $args["h"] : ((isset($args["host"])) ? (string) $args["host"] : "localhost");
    $HOST_PORT = (isset($args["p"])) ? (integer) $args["p"] : ((isset($args["port"])) ? (string) $args["port"] : 3000);


    /*----------------------------------------------------------------------------------------------------------------------------
     * PUT USAGE:
     * public int Aerospike::put ( array $key, array $record [, int $ttl = 0 [, array $options ]] )
     *----------------------------------------------------------------------------------------------------------------------------
     */

    /*
     * EXAMPLE 1: PUT AND UPDATE BASIC DATATYPES (string and integer), 
     * PASSING NO OPTIONAL PARAMETERS
     */

    $config = array("hosts"=>array(array("addr"=>$HOST_ADDR, "port"=>$HOST_PORT)));
    $db = new Aerospike($config, 'prod-db');
    if (!$db->isConnected()) {
        echo "Aerospike failed to connect to host $HOST_ADDR:$HOST_PORT [{$db->errorno()}]: {$db->error()}\n";
        exit(1);
    } else {
        echo "Aerospike connection to host $HOST_ADDR:$HOST_PORT successful\n";
        $key = $db->initKey("test", "users", 1234);
        $put_vals = array("email" => "hey@example.com", "name" => "Hey There");
        // will ensure a record exists at the given key with the specified bins
        $res = $db->put($key, $put_vals);
        if ($res == Aerospike::OK) {
            echo "Record written.\n";
        } else {
            echo "[{$db->errorno()}] ".$db->error();
        }
        // Updating the record
        $put_vals = array("name" => "You There", "age" => 33);
        // will update the name bin, and create a new 'age' bin
        $res = $db->put($key, $put_vals);
        if ($res == Aerospike::OK) {
            echo "Record updated.\n";
        } else {
            echo "[{$db->errorno()}] ".$db->error();
        }
    }

    /*
     * EXAMPLE 2: PUT LIST/MAP PASSING OPTIONAL TTL AND POLICY OPTIONS
     */

    if (!$db->isConnected()) {
        echo "Aerospike failed to connect to host INVALID_ADDR:INVALID_PORT [{$db->errorno()}]: {$db->error()}\n";
        exit(1);
    } else {
        $key = $db->initKey("test", "demo", "example_key");
        $put_vals = array("education" => array("degree"=>"B.Tech", "grade"=>"A+"),
                          "hobbies" => array("playing cricket", "playing guitar", "dance"));
        $opts = array(Aerospike::OPT_POLICY_EXISTS => Aerospike::POLICY_EXISTS_CREATE_OR_REPLACE,
                      Aerospike::OPT_POLICY_RETRY => Aerospike::POLICY_RETRY_ONCE,
                      Aerospike::OPT_WRITE_TIMEOUT => 100000);
        // will ensure a record exists at the given key with the specified bins
        $res = $db->put($key, $put_vals, 100, $opts);
        if ($res == Aerospike::OK) {
            echo "Record written.\n";
        } else {
            echo "[{$db->errorno()}] ".$db->error();
        }
    }
 
?>
