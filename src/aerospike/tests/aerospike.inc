<?php
if(file_exists(dirname(__FILE__)."/aerospike.local.inc")) {
    include(dirname(__FILE__)."/aerospike.local.inc");
} else {
    define("AEROSPIKE_CONFIG_NAME", "10.71.71.49");
    define("AEROSPIKE_CONFIG_PORT", 3000);
}
