<?php

/*
 * FUNCTION TO PARSE COMMAND LINE ARGS TO SCRIPT.
 */
function parseArgs() {
    $shortopts  = "";
    $shortopts .= "h::";    /* Optional host */
    $shortopts .= "p::";    /* Optional port */
    $shortopts .= "n::";    /* Optional namespace */
    $shortopts .= "s::";    /* Optional set */
    $shortopts .= "w::";    /* Optional workload */
    $shortopts .= "o";      /* Optional execute only once and quit */
    $shortopts .= "k::";    /* Optional keys */
    $shortopts .= "r::";    /* Optional report file */
    $shortopts .= "u";      /* Optional usage */
 
    $longopts  = array(
        "host::",           /* Optional host */
        "port::",           /* Optional port */
        "namespace::",      /* Optional namesapce */
        "set::",            /* Optional set */
        "workload::",       /* Optional workload*/
        "once",             /* Optional execute only once and quit */
        "keys::",           /* Optional keys */
        "report::",         /* Optional report file */
        "usage"             /* Optional usage */
    );

    $options = getopt($shortopts, $longopts);
    return $options;
}

/*
 * FUNCTION TO HANDLE CTRL+C SIGNAL 
 * TO STOP PERFORMANCE SCRIPT AND GENERATE REPORT.
 */
declare(ticks = 1);

function signal_handler($signal) {
    global $myPerformanceTest;
    switch($signal) {
        case SIGTERM:
            $myPerformanceTest->total_summary();
            exit;
        case SIGKILL:
            $myPerformanceTest->total_summary();
            exit;
        case SIGINT:
            $myPerformanceTest->total_summary();
            exit;
    }
}

/*
 * CLASS TO EXECUTE PERFORMANCE TESTS.
 */
class PerformanceTest {

    private $HOST_ADDR                 = "localhost";
    private $HOST_PORT                 = 3000;
    private $WORKLOAD                  = "RW";
    private $KEYS                      = 1000000;
    public $ONCE                       = FALSE;
    private $REPORT                    = "php://stdout";
    private $NAMESPACE                 = "test";
    private $SET                       = "demo";
    
    private $total_time                = 0;
    private $read_count                = 0;
    private $write_count               = 0;
    private $total_count               = 0;
    private $handle                    = NULL;
    private $r                         = 80;
    private $w                         = 20;
    private $expected_read_count       = 0;
    private $expected_write_count      = 0; 
    private $benchmark_time_after_keys = 100000;
    private $db                        = NULL;
    

    /*
     * CONSTRUCTOR PARSES COMMAND LINE ARGS AND SETS THE CLASS MEMBERS.
     */
    public function __construct() {
        $args = parseArgs();
        if (isset($args["usage"]) || isset($args["u"])) {
            echo("php benchmark.php [-h<HOST IP ADDRESS>|--host=<HOST IP ADDRESS>
                                     -p<HOST PORT NUMBER>|--port=<HOST PORT NUMBER>
                                     -n<NAMESPACE NAME>|--namespace=<NAMESPACE NAME>
                                     -s<SET NAME>|--set=<SET NAME>
                                     -w<WORKLOAD TYPE=R,W or RW>|--workload=<WORKLOAD TYPE=R,W or RW>
                                     -o|--once     
                                     -k<NO. OF KEYS>|--keys<NO. OF KEYS>
                                     -r</path/for/report/file>|--report=</path/for/report/file>           
                                     -u|--usage]\n");
            exit(1);
        }

        $this->HOST_ADDR = (isset($args["h"])) ? (string) $args["h"] : ((isset($args["host"])) ? (string) $args["host"] : "localhost");
        $this->HOST_PORT = (isset($args["p"])) ? (integer) $args["p"] : ((isset($args["port"])) ? (integer) $args["port"] : 3000);
        $this->NAMESPACE = (isset($args["n"])) ? (string) $args["n"] : ((isset($args["namespace"])) ? (string) $args["namespace"] : "test");
        $this->SET       = (isset($args["s"])) ? (string) $args["s"] : ((isset($args["set"])) ? (string) $args["set"] : "demo");
        if (isset($args["w"])) {
            $val = explode(",", (string) $args["w"]);
            $this->WORKLOAD = (string) $val[0];
            if (count($val) > 1) {
                $this->r = (integer) $val[1];
            } else {
                $this->r = 80;
            }
        } else if (isset($args["workload"])) {
            $val = explode(",", (string) $args["workload"]);
            $this->WORKLOAD = (string) $val[0];
            if (count($val) > 1) {
                $this->r = (integer) $val[1];
            } else {
                $this->r = 80;
            }
        } else {
            $this->WORKLAD = "RW";
            $this->r = 80;
        }
        $this->w = 100 - $this->r;

        $this->KEYS      = (isset($args["k"])) ? (integer) $args["k"] : ((isset($args["keys"])) ? (integer) $args["keys"] : 1000000);
        $this->ONCE      = (isset($args["o"])) ? TRUE : ((isset($args["once"])) ? TRUE : FALSE);
        $this->REPORT    = (isset($args["r"])) ? (string) $args["r"] : ((isset($args["report"])) ? (string) $args["report"] : "php://stdout");
        $this->handle    = fopen ($this->REPORT, 'a+');
        $this->expected_read_count = (integer) $this->KEYS * ($this->r/100);
        $this->expected_write_count = (integer) $this->KEYS * ($this->w/100); 
    }

    /*
     * FUNCTION TO CONNECT TO AEROSPIKE DB.
     */
    public function connect() {
        $config = array("hosts"=>array(array("addr"=>$this->HOST_ADDR, "port"=>$this->HOST_PORT)));
        $this->db = new Aerospike($config);

        $status = $this->db->isConnected();
        if (!$status) {
            echo "Aerospike failed to connect[{$this->db->errorno()}]: {$this->db->error()}\n";
            exit(1);
        }
    }

    /*
     * FUNCTION TO GENERATE SUMMARY REPORT.
     */
    public function total_summary() {

        if ($this->total_count) {
            $average_time = ($this->total_time)/($this->total_count);
            $tps = ($this->total_count)/($this->total_time);
        } else {
            $average_time = $tps = 0;
        }

        if ($this->REPORT != "php://stdout") {
            echo "Check the performance report: " .$this->REPORT ."\n";
        } else {
            echo "Performance report:\n";
        }
        fwrite($this->handle, "\nTOTAL TIME = " .($this->total_time) ." seconds FOR " .($this->total_count) ." OPERATIONS\n");
        fwrite($this->handle, "AVERAGE TIME PER OPERATION = " .$average_time ." seconds\n");
        fwrite($this->handle, "TOTAL READS = " .($this->read_count) ."\n");
        fwrite($this->handle, "TOTAL WRITES = " .($this->write_count) ."\n");
        fwrite($this->handle, "AVERAGE OPERATIONS PER SECOND = ". $tps. "\n");
        fclose($this->handle);
    }

    /*
     * FUNCTION TO WRITE A RECORD TO AEROSPIKE DB.
     */
    public function put_rec($key) {
        $rec = array("key" => $key["key"]);
        $start = microtime(true);
        $status = $this->db->put($key, $rec);
        if ($status != Aerospike::OK) {
            echo "Aerospike put failed[{($this->db)->errorno()}]: {($this->db)->error()}\n";
            exit(1);
        }
        $end = microtime(true);
        $this->write_count++;
        $this->total_count++;
        return ($end-$start);
    }

    /*
     * FUNCTION TO READ A RECORD FROM AEROSPIKE DB.
     */
    public function get_rec($key) {
        $start = microtime(true);
        $status = $this->db->get($key, $record);
        if ($status != Aerospike::OK) {
            echo "Aerospike get failed[{$this->db->errorno()}]: {$this->db->error()}\n";
            exit(1);
        }
        $end = microtime(true);
        $this->read_count++;
        $this->total_count++;
        return ($end-$start);
    }

    /*
     * FUNCTION TO RANDOMLY READ/WRITE A RECORD FROM/TO 
     * AEROSPIKE DB BASED ON r/w RATIO.
     */
    public function operation($key) {
        $op = rand(0, 100);
        if ($op <= ($this->r)) {
            if (($this->expected_read_count) <= 0) {
                if (!($this->ONCE)) {
                    $this->expected_read_count = (integer) ($this->KEYS) * (80/100);
                } else {
                    return (0);
                }
            }
            $this->expected_read_count--;
            return $this->get_rec($key);
        } else if ($op > ($this->r)) {
            if (($this->expected_write_count) <= 0) {
                if (!($this->ONCE)) {
                    $this->expected_write_count = (integer) $KEYS * (20/100);
                } else {
                    return (0);
                }
            }
            $this->expected_write_count--;
            return $this->put_rec($key);
        } else {
            echo "Failure\n";
            exit(1);
        }
    }

    /*
     * FUNCTION TO EXECUTE THE PERFORMANCE TEST.
     * RUNS THE TEST FOR ALL SPECIFIED NUMBER OR KEYS
     * OR INDEFINITELY UNTIL CTRL+C IS PRESSED.
     */
    public function execute() {
        do {
            while(($this->total_count) < $this->KEYS) {
                $key = $this->db->initKey($this->NAMESPACE, $this->SET, "$this->total_count");
                if (!$key) {
                    echo "Aerospike initKeyfailed[{($this->db)->errorno()}]: {($this->db)->error()}\n";
                    exit(1);
                }
                if ($this->WORKLOAD == "W") {
                    $this->total_time += $this->put_rec($key);
                } else if ($this->WORKLOAD == "R") {
                    $this->total_time += $this->get_rec($key);
                } else if ($this->WORKLOAD == "RW") {
                    $this->total_time += $this->operation($key);
                } else {
                    echo "Invalid workload\n";
                    exit(1);
                }
                if (($this->total_count) % $this->benchmark_time_after_keys == 0 && ($this->total_count>0)) {
                    fwrite($this->handle, "\nOPERATION TIME = " .($this->total_time) ." seconds FOR ". ($this->total_count) ." KEYS\n");
                }
            }
        } while(!($this->ONCE));
    }
}

/*
 * INSTANTIATE PERFORMANCE TEST CLASS, CONNECT TO DB,
 * RUN THE TEST AND GENERATE REPORT.
 */

$myPerformanceTest = new PerformanceTest();
if (extension_loaded('pcntl')) {
    pcntl_signal(SIGTERM, "signal_handler");
    pcntl_signal(SIGINT, "signal_handler");
} else if (!$myPerformanceTest->ONCE){
    echo "PHP: pcntl extension not found. Please install pcntl extension or re-run with the -o or --once option.\n";
    exit(1);
}

$myPerformanceTest->connect();
$myPerformanceTest->execute();
$myPerformanceTest->total_summary();

?>
