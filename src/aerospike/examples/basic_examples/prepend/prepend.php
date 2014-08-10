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
        echo("php prepend.php [-h<HOST IP ADDRESS>|--host=<HOST IP ADDRESS> -p<HOST PORT NUMBER>|--port=<HOST PORT NUMBER>]\n");
        exit(1);
    }

    $HOST_ADDR = (isset($args["h"])) ? (string) $args["h"] : ((isset($args["host"])) ? (string) $args["host"] : "localhost");
    $HOST_PORT = (isset($args["p"])) ? (integer) $args["p"] : ((isset($args["port"])) ? (string) $args["port"] : 3000);


    /*----------------------------------------------------------------------------------------------------------------------------
     * PREPEND USAGE:
     * public int Aerospike::prepend ( array $key, string $bin, string $value [, array $options ] )
     *----------------------------------------------------------------------------------------------------------------------------
     */

    /*
     * EXAMPLE 1: PREPEND PASSING NO OPTIONAL PARAMETERS
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
            // prepend a value to a string bin.
            $res = $db->prepend($key, 'name', '*');
            if ($res == Aerospike::OK) {
                echo "Starred the user.\n";
            } else {
                echo "[{$db->errorno()}] ".$db->error();
            } 
        } else {
            echo "[{$db->errorno()}] ".$db->error();
        }
    }

    /*
     * EXAMPLE 2: PREPEND PASSING OPTIONAL TIMEOUT AND RETRY POLICIES
     */

    if (!$db->isConnected()) {
        echo "Aerospike failed to connect to host INVALID_ADDR:INVALID_PORT [{$db->errorno()}]: {$db->error()}\n";
        exit(1);
    } else {
        $key = $db->initKey("test", "demo", "example_key");
        $put_vals = array("education" => array("degree"=>"B.Tech", "grade"=>"A+"),
                          "hobbies" => "playing");
        $opts = array(Aerospike::OPT_POLICY_RETRY => Aerospike::POLICY_RETRY_ONCE,
                      Aerospike::OPT_WRITE_TIMEOUT => 100000);
        // will ensure a record exists at the given key with the specified bins
        $res = $db->put($key, $put_vals);
        if ($res == Aerospike::OK) {
            echo "Record written.\n";
            // prepend a value to a string bin.
            $res = $db->prepend($key, 'hobbies', 'Guitar');
            if ($res == Aerospike::OK) {
                echo "Added the guitar to hobbies.\n";
            } else {
                echo "[{$db->errorno()}] ".$db->error();
            }
        } else {
            echo "[{$db->errorno()}] ".$db->error();
        }
    }
 
?>
