<?php

require_once 'ASTestFramework.inc';
require_once dirname(__FILE__).'/../Common.inc';

$status_codes = array(0=>"OK", 
                      100=>"ERR",
                      200=>"ERR_CLIENT", 
                      201=>"ERR_PARAM",
                      300=>"ERR_CLUSTER",
                      400=>"ERR_TIMEOUT",
                      401=>"ERR_THROTTLED",
                      500=>"ERR_SERVER",
                      501=>"ERR_REQUEST_INVALID", 
                      503=>"ERR_SERVER_FULL",
                      504=>"ERR_CLUSTER_CHANGE",
                      505=>"ERR_UNSUPPORTED_FEATURE",
                      506=>"ERR_DEVICE_OVERLOAD", 
                      600=>"ERR_RECORD",
                      601=>"ERR_RECORD_BUSY", 
                      602=>"ERR_RECORD_NOT_FOUND",
                      603=>"ERR_RECORD_EXISTS", 
                      604=>"ERR_RECORD_GENERATION",
                      605=>"ERR_RECORD_TOO_BIG",
                      606=>"ERR_BIN_INCOMPATIBLE_TYPE",
                      607=>"ERR_RECORD_KEY_MISMATCH", 
                      1000=>"ERR_SCAN",
                      10001=>"ERR_SCAN_ABORTED",  
                      1100=>"ERR_QUERY",
                      1101=>"ERR_QUERY_ABORTED", 
                      1102=>"ERR_QUERY_QUEUE_FULL",
                      1200=>"ERR_INDEX", 
                      1201=>"ERR_INDEX_OOM",
                      1202=>"ERR_INDEX_NOT_FOUND", 
                      1203=>"ERR_INDEX_FOUND",
                      1204=>"ERR_INDEX_NOT_READABLE",
                      1205=>"ERR_INDEX_NAME_MAXLEN",
                      1206=>"ERR_INDEX_MAXCOUNT", 
                      1300=>"ERR_UDF",
                      1301=>"ERR_UDF_NOT_FOUND",
                      2000=>"Parameter_Exception", 
                      2001=>"Undefined_Error",
                      2002=>"ERR_GET_RECORD");

function aerospike_phpt_runtest($clsname, $name) {
    require_once(dirname(__FILE__)."/../$clsname.inc");
    $reflector = new ReflectionClass($clsname);

    if ($reflector->hasMethod("setUpBeforeClass")) {
        $reflector->getMethod("setUpBeforeClass"
                              )->invoke(NULL);
    }

    $obj = $reflector->newInstance();

    # If any assertions fail here, PHPUnitCompat will fail the test
    # and complain.
    $rv= $obj->runSingleTest($name);
    global $status_codes;
    printf("%s", ($status_codes[$rv]));
}

function aerospike_phpt_skipif($clsname, $name) {
    require_once(dirname(__FILE__)."/../$clsname.inc");
    $reflector = new ReflectionClass($clsname);
    $obj = $reflector->newInstance();
    $skip_cls = "skipALL";
    $matchlist = array();

    $bname = preg_match("/^test(.+)/", $name, $matchlist);
    $skip_meth = "skip" . $matchlist[1];

    $is_skipped;

    $meths = array($skip_cls, $skip_meth);

    foreach ($meths as $meth) {
        if ($reflector->hasMethod($meth)) {
            try {
                $is_skipped = $obj->$meth();
            } catch (CBTestFramework_TestSkipException $ex) {
                die("skip " . $ex->getMessage());
            }
        }
    }
}
