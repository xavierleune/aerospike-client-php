<?php
$keys = $argv[1];
if ($keys < 10000) {
	echo "Enter first argument greater then or equal to 10000.\n";
}else {
	$config = array("hosts"=>array(array("name"=>"10.71.71.49", "port"=>3000)));
	$line = "";	
	$as = new Aerospike($config);
	$r = array();
	$avg=0;
	$count = 0;
	for($i=10000;$i<=$keys;$i++)
	{
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"$i");
		$start = time();
		$as->get($key, &$r);
		$end = time();
		$avg = $avg+($end-$start);
		$count++;
	}
	$average_get_time = $avg/$count;
	echo "\nTotal GET Time:= ".$avg."\n";
	echo "Total AVG GET Time:= ".$average_get_time."\n"; 
}
?>
