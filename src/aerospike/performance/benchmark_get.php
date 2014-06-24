<?php
$keys = $argv[1];
$handle = fopen ("benchmark_get",'a+');
if ($keys < 10000) {
	echo "Enter first argument greater then or equal to 10000.\n";
}else {
	$config = array("hosts"=>array(array("name"=>"10.71.71.49", "port"=>3000)));
	
	$as = new Aerospike($config);
	$r=new Record();
	//$r = array();
	$avg=0;
	for($i=10000;$i<=$keys;$i++)
	{
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"'$i'");
		
		fwrite($handle , "Memory Usage Get:= ".memory_get_usage().", ");
		$start = microtime(true);
		fwrite($handle , "Start GET MicroS:= ".$start.", ");
		$as->get($key, &$r);
		$rs = $r->getBins();
		$end = microtime(true);
		fwrite($handle , "End  GET MicroS:= ".$end.", ");
		$avg = $avg+($end-$start);
		fwrite($handle , "Diff in MicroS:= ".($end-$start).", ");
		fwrite($handle , "Memory Usage end Get:= ".memory_get_usage()."\n");
	}
	$average_get_time = $avg/99000;
	fwrite($handle , "Total GET Time:= ".$avg."\n");;
	fwrite($handle , "Total AVG GET Time:= ".$average_get_time."\n");;
}
fclose($handle);
?>