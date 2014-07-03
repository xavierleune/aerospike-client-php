<?php
$keys = $argv[1];
$numberofbins = $argv[2];
$bins_key_value_array=array();

if ($keys < 10000) {
	echo "Enter first argument greater then or equal to 10000.\n";
}else {
	$config = array("hosts"=>array(array("name"=>"10.71.71.49", "port"=>3000)));
	$as = new Aerospike($config);
	$avg=0;
        $count = 0;
	$line="";
	for($i=10000;$i<=$keys;$i++)
	{
		$key = array("ns"=>"test", "set"=>"demo", "key"=>"$i");
		for($j=1;$j<=$numberofbins;$j++)
		{
			$value = $i+$j;
			$bins_key_value_array["bin".$j] = "$value";
		}
                $line = $line."Memory Usage Put:= ".memory_get_usage().", ";
                $start = microtime(true);
                $line = $line."Start PUT MicroS:= ".$start.", ";
		$rv = $as->put($key, $bins_key_value_array);
		$end = microtime(true);
                $line = $line."End  PUT MicroS:= ".$end.", ";
                $avg = $avg+($end-$start);
                $line = $line."Diff in MicroS:= ".($end-$start).", ";
                $line = $line."Memory Usage end Put:= ".memory_get_usage()."\n";
                $count++;
        }
        $average_put_time = $avg/$count;
        $line = $line."\nTotal PUT Time:= ".$avg."\n";
        $line = $line."Total AVG PUT Time:= ".$average_put_time."\n";
}
$handle = fopen("PutPerformance",'a+');
fwrite($handle,$line);
fclose($handle);	
?>
