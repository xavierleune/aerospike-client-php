<?php



$config = array("hosts"=>array(array("addr"=>"127.0.0.1", "port"=>"3000")));
$db = new Aerospike($config);
$objKey = $db->initKey("test", "demo", base64_decode("put_option_policy_key_digest"), true);
$put_record = array("binA"=>10, "binB"=>20);
$put_status = $db->put($objKey, array("binA"=>10, "binB"=>20), NULL,
    array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
$get_status = $db->get($objKey, $record, array());
echo "xxxxxxxxxxxxx STATUS xxxxxxxxxxxx.\n";
var_dump($get_status);
var_dump($db->error());

?>

