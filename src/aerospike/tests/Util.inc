<?php
require_once dirname(__FILE__).'/aerospike.inc';
function make_handle($persistent = true) {
$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
    $handle = new Aerospike($config);
    return $handle;
}

function make_handle_oo($persistent = true) {
$config = array("hosts"=>array(array("name"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
    $oo = new Aerospike($config);

    return $oo;
}
