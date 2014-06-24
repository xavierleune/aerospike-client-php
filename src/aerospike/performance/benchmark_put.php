<?php
$keys = $argv[1];
$numberofbins = $argv[2];
$bins_key_value_array=array();

if ($keys < 10000) {
	echo "Enter first argument greater then or equal to 10000.\n";
}else {
	$config = array("hosts"=>array(array("name"=>"10.71.71.49", "port"=>3000)));
	$as = new Aerospike($config);
	for($i=10000;$i<=$keys;$i++)
	{
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"'$i'");
		for($j=1;$j<=count($numberofbins);$j++)
		{
			$value = $i+$j;
			$bins_key_value_array["bin".$j] = "'$value'";
		}
		$rv = $as->put($key, $bins_key_value_array);
	}
}	
?>