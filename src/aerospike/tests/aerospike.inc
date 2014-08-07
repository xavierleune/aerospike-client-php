<?php
if(file_exists(dirname(__FILE__)."/aerospike.local.inc")) {
    include(dirname(__FILE__)."/aerospike.local.inc");
} else {
    define("AEROSPIKE_CONFIG_NAME", "10.43.3.69");
    define("AEROSPIKE_CONFIG_PORT", 3000);
    define("AEROSPIKE_CONFIG_NAME2", "10.43.4.53");
    define("AEROSPIKE_CONFIG_PORT2", 3010);
    define("AEROSPIKE_CONFIG_NAME3", "172.20.25.180");
}
