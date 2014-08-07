<?php
$host_ip = "172.20.25.179"
$host_port = 3000;
$keys = $argv[1];
$numberofbins = $argv[2];
$bins_key_value_array = array();
$benchmark_time_after_keys = 100000;

if ($keys < 500000) {
    echo "Enter first argument greater then or equal to 500000.\n";
} else {
    $handle = fopen ("benchmark_put_report.log", 'a+');
    $config = array("hosts"=>array(array("addr"=>$host_ip, "port"=>$port)));
    $db = new Aerospike($config);

    if (!$db->isConnected()) {
        echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
        exit(1);
    }

    $total_time = 0;
    $line = "";
    $time_at_every_ = array();

    for($i=0; $i<=$keys; $i++) {
        $key = $db->initKey("test", "demo", "$i");
        if (!$key) {
            echo "Aerospike initKeyfailed[{$db->errorno()}]: {$db->error()}\n";
            exit(1);
        }

        for($j=1; $j<=$numberofbins; $j++) {
            $value = $i + $j;
            $bins_key_value_array["bin".$j] = "$value";
        }

        $start = microtime(true);
        $status = $db->put($key, $bins_key_value_array);
        if ($status != Aerospike::OK) {
            echo "Aerospike put failed[{$db->errorno()}]: {$db->error()}\n";
            exit(1);
        }

        $end = microtime(true);
        $total_time += ($end - $start);
	if ($i % $benchmark_time_after_keys == 0) {
	    fwrite($handle, "\nPUT TIME = " .$total_time. " seconds FOR ". $i. " KEYS\n");
	}
    }

    $average_put_time = $total_time/$keys;
    $write_tps = $keys/$total_time;
    fwrite($handle, "\nTOTAL PUT TIME = " .$total_time. " seconds FOR ". $keys. " KEYS\n");
    fwrite($handle, "AVERAGE PUT TIME PER KEY = ". $average_put_time. " seconds\n");
    fwrite($handle, "AVERAGE WRITES PER SECOND = ". $write_tps. "\n");
    fclose($handle);
    echo "Check the performance report: benchmark_put_report.log";
}
?>
