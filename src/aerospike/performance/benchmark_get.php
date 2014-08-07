<?php
$host_ip = "172.20.25.179"
$host_port = 3000;
$keys = $argv[1];
$benchmark_time_after_keys = 100000;

if ($keys < 500000) {
    echo "Enter first argument greater then or equal to 500000.\n";
} else {
    $handle = fopen ("benchmark_get_report.log", 'a+');
    $config = array("hosts"=>array(array("addr"=>$host_ip, "port"=>$host_port)));
    $db = new Aerospike($config);

    if (!$db->isConnected()) {
        echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
        exit(1);
    }

    $total_time = 0;

    for($i=0; $i<=$keys; $i++) {
        $key = $db->initKey("test", "demo", "$i");
        if (!$key) {
            echo "Aerospike initKeyfailed[{$db->errorno()}]: {$db->error()}\n";
            exit(1);
        }

        //$mem_usage_start = memory_get_usage();
        $start = microtime(true);
        $status = $db->get($key, $record);
        if ($status != Aerospike::OK) {
            echo "Aerospike get failed[{$db->errorno()}]: {$db->error()}\n";
            exit(1);
        }

        $end = microtime(true);
        $total_time += ($end - $start);
        //$mem_usage_end = memory_get_usage();
	if ($i % $benchmark_time_after_keys == 0) {
            fwrite($handle, "\nGET TIME = " .$total_time. " seconds FOR ". $i. " KEYS\n");
        }
    }

    $average_get_time = $total_time/$keys;
    $read_tps = $keys/$total_time;

    fwrite($handle, "\nTOTAL GET TIME = " .$total_time. " seconds FOR ". $keys. " KEYS\n");
    fwrite($handle, "AVERAGE GET TIME PER KEY = ". $average_get_time. " seconds\n");
    fwrite($handle, "AVERAGE READS PER SECOND = ". $read_tps. "\n");
    fclose($handle);
    echo "Check the performance report: benchmark_get_report.log";
}
?>

