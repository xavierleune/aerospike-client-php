<?php
$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>"3000")));
$db = new Aerospike($config);
if (!$db->isConnected()) {
    echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
    echo "Not connected";
    exit(1);
} else {
    echo "Connected";
    echo "<h1>Welcome!</h1>";
}

$app = new Phalcon\Mvc\Micro();
$app->get('/', function () use($db) {
    if (!$db->isConnected()) {
        echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
        echo "Not connected";
        exit(1);
    } else {
        echo "Connected";
        echo "<h1>Welcome!</h1>";
    }
});

$app->get('/get', function() use($db) {
    if($db->isConnected()) {
        $key = $db->initKey("test", "demo", "key1");
        $status = $db->get($key, $record);
        var_dump($record);
        if($status != Aerospike::OK) {
            echo "Get failed";
        }
    $db->close();
    }
});

$app->get('/multiget', function() use($db) {
    echo "In multiget";
    if($db->isConnected()) {
        for($i=1 ;$i<1000; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->get($key, $record);
            var_dump($record);
            if($status != Aerospike::OK) {
                echo "\nMultiget failed at ".$i."th record";
            }
        }
    $db->close();
    }
});

$app->get('/exists', function() use($db) {
    $key = $db->initKey("test", "demo", "exist_key");
    $db->put($key,array("Greet"=>"World_end"));
    if ($db->isConnected()) {
        $status = $db->exists($key, $metadata, array(Aerospike::OPT_READ_TIMEOUT=>3000));
    } else {
        echo "Failure";
        $db->close();
    }
});

$app->put('/increment', function() use($db) {
    $key = $db->initKey("test", "demo", "key1");
    $res = $db->increment($key, 'age', 20);
    if ($res != Aerospike::OK) {
        echo "Error is: \n".$db->errorno();;
    }
    $db->close();
});

$app->post('/put', function() use($db) {
    if($db->isConnected()) {
        $key = $db->initKey("test", "demo", "key1");
        $record = array("name" => "John", "age" => 32);
        $status = $db->put($key, $record);
        if($status != Aerospike::OK) {
            echo "Single put failed";
        }
        $db->close();
    }
});

$app->post('/multiput', function() use($db) {
    if($db->isConnected()) {
        for($i=1; $i<1000; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $record = array("name" => "name".$i, "age" => $i);
            $status = $db->put($key, $record);
            if($status != Aerospike::OK) {
                echo "\nMultiput failed at ".$i."th record";
                break;
            }
        }
    $db->close();
    }
});

$app->put('/multiappend', function() use($db) {
    if ($db->isConnected()) {
        for ($i=1; $i<1000; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->append($key, 'name', ' World',
                array(Aerospike::OPT_WRITE_TIMEOUT=>1000));
            if($status != Aerospike::OK) {
                echo "\nAppend failed at ".$i."th record";
            }
        }
    $db->close();
    }
});


$app->put('/multiprepend', function() use($db) {
    if ($db->isConnected()) {
        for ($i=1; $i<1000; $i++) {
        $key = $db->initKey("test", "demo", "key".$i);
        $status = $db->prepend($key, 'name', 'Greet',
            array(Aerospike::OPT_WRITE_TIMEOUT=>1000));
        if($status != Aerospike::OK) {
            echo "\nPrepend failed at ".$i."th record";
        }
        }
    $db->close();
    }
});

$app->get('/scan', function() use($db) { 
    if($db->isConnected()) {
        $status = $db->scan( "test", "demo", function ($record) {
            global $processed, $mystatus;
            $bins=$record['bins'];
            if (array_key_exists('age', $bins) && !is_null($bins['age']))
            {
                echo "\nName: ".$bins['name']."\nAge: ".$bins['age'];
            }
        }, array("age","name"));
    }
    $db->close();
});

$app->delete('/multiremove', function() use($db) {
    if($db->isConnected()) {
        for($i=1; $i<1000; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY
                =>Aerospike::POLICY_RETRY_NONE));
            if($status != Aerospike::OK) {
                echo "\nCannot remove record at ".$i."th location";
            }
        }
        $db->close();
    }
});

$app->delete('/multiremovebin', function() use($db) {
    if($db->isConnected()) {
        for($i=1; $i<1000; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->removeBin($key, array("age"));
        }
    $db->close();
    }
});

$app->get('/query', function() use($db) {
    $total = 0;
    $in_thirties = 0;
    if ($db->isConnected()) {
        $where = $db->predicateBetween("age", 30, 39);
        $status = $db->query("test", "demo", $where, function($record) {
            global $total, $in_thirties;
            $bins = $record['bins']; 
            if (array_key_exists("name", $bins) && !is_null($bins["name"])) {
                echo "\nFound record with name: ".$bins["name"]." and age:
".$bins["age"];
            }
            $total += (int) $bins['age'];
            $in_thirties++;
        }, array("age","name"));
        if($status == Aerospike::OK && $total) {
            echo "\nThe average age of employees in their thirties is
                ".round($total / $in_thirties)."\n";
        } else {
            echo "Failure encountered";
        }
        $db->close();
    } else {
        echo "Error is: ".$db->errorno();
    }
});

$app->put('/multitouch', function() use($db) {
    if ($db->isConnected()) {
        for($i=1; $i<1000; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->touch($key, 1000, array(Aerospike::OPT_WRITE_TIMEOUT=>200));
            if($status != Aerospike::OK) {
                echo "Multitouch failed at ".$i."th record";
                break;
            }
        }
        $db->close();
    }
});

$app->get('/registerderegister', function() use($db) {
    $register_status = $db->register("lua/stream_udf.lua", "stream_udf.lua");
    if ($register_status != Aerospike::OK) {
        echo "Register Failed";
    }
    $res = $db->deregister('stream_udf.lua');
    if ($res != Aerospike::OK) {
        echo "[{$db->errorno()}] ".$db->error();
    }
    $db->close();
});

$app->get('/listregistered', function() use($db) {
    $res = $db->listRegistered($modules);
    var_dump($modules);
    if ($res != Aerospike::OK) {
        echo "[{$db->errorno()}] ".$db->error();
    }
    $db->close();
});

$app->get('/register', function() use($db) {
    $register_status = $db->register("lua/stream_udf.lua", "stream_udf.lua");
    if ($register_status != Aerospike::OK) {
        echo "Register Failed";
    }
    $db->close();
});

$app->get('/deregister', function() use($db) {
    $res = $db->deregister('stream_udf.lua');
    if ($res != Aerospike::OK) {
        echo "[{$db->errorno()}] ".$db->error();
    }
    $db->close();
});

$app->get('/getregistered', function() use($db) {
    $res = $db->getRegistered('stream_udf.lua', $code);
    if ($res == Aerospike::OK) {
         $handle = fopen("lua/stream_udf.lua","r");
         $mycode = fread($handle,filesize("lua/stream_udf.lua"));
         $result = strcmp($code, $mycode);
         if($result) {
             echo "Failure";
         }
    } else {
        if ($res == Aerospike::ERR_UDF_FILE_NOT_FOUND) {
            echo "The UDF module my_udf was not found to be registered with the server.\n";
        } else {
            echo "[{$db->errorno()}] ".$db->error();
        }
    }
    $db->close();
});

$app->post('/setserializer', function() use($db) {
    $key = $db->initKey("test", "demo", "map_of_objects_with_UDF_serializer");
    class Employee 
    {   
        public $desg = 'Manager';
    }

     #Set Serializer
    $db->setSerializer(function ($val) {
        if (is_bool ($val)) {
            return "b||". serialize($val);
        }
        if (is_object ($val)) {
            return "o||". serialize($val);
        }
        if (is_double ($val)) {
            return "d||". serialize($val);
        }
            return "r||". $val;
    });
    
     #Creating objects
     $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $map1 = array(12=>$obj1, $obj3);
     $map2 = array($map1, $obj2, " ");
     $map3 = array("k1", $obj4, 56=>$map2);
     $status = $db->put($key, array("bin1"=>$map3), NULL,
     array(Aerospike::OPT_SERIALIZER => Aerospike::SERIALIZER_USER));
     if ($status != AEROSPIKE::OK) {
         /* $db->close();*/
         return($status);
     }
  /*  $db->setDeserializer(function ($val) {
        $prefix = substr($val, 0, 3);
        if ($prefix != 'r||') {
            return unserialize(substr ($val, 3));
        }
            return unserialize(substr ($val, 3));
        });

        $status = $db->get($key, $get_record, array("bin1"));
        if ($status != AEROSPIKE::OK) {
            // return($status);
            echo "Error";
        }
        if (strcmp($get_record["key"]["ns"], $key["ns"]) == 0 &&
            strcmp($get_record["key"]["set"], $key["set"]) == 0 &&
            strcmp($get_record["key"]["key"], $key["key"]) == 0) {
            $comp_res = array_diff_assoc_recursive($put_record, $get_record["bins"]);
            if(!empty($comp_res))
            {
               // return self::$status_get_record_fail;
            } else {
               echo "Get successful";
            }
        }
        $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
            Aerospike::POLICY_RETRY_NONE));
        echo "Success";
   /* $status = $db->remove($key, array(aErospike::OPT_POLICY_RETRY =>
       Aerospike::POLICY_RETRY_NONE));*/
        $db->close();
});

$app->get('/setdeserializer', function() use($db) {
    $key = $db->initKey("test", "demo", "map_of_objects_with_UDF_serializer");
    class Employee 
    {   
        public $desg = 'Manager';
    }
    #Set Deserializer
    $db->setDeserializer(function ($val) {
        $prefix = substr($val, 0, 3);
        if ($prefix != 'r||') {
            return unserialize(substr ($val, 3));
        }
            return unserialize(substr ($val, 3));
        });

        $status = $db->get($key, $get_record, array("bin1"));
        if ($status != AEROSPIKE::OK) {
            // return($status);
            echo "Error";
        }
        if (strcmp($get_record["key"]["ns"], $key["ns"]) == 0 &&
            strcmp($get_record["key"]["set"], $key["set"]) == 0 &&
            strcmp($get_record["key"]["key"], $key["key"]) == 0) {
            $comp_res = array_diff_assoc_recursive($put_record, $get_record["bins"]);
            if(!empty($comp_res))
            {
               // return self::$status_get_record_fail;
            } else {
               echo "Get successful";
            }
        }
        $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
            Aerospike::POLICY_RETRY_NONE));
        echo "Success";
       // return($status);
});

$app->get('/aggregate', function() use($db) {
    $register_status = $db->register("lua/stream_udf.lua", "stream_udf.lua");
    if ($register_status != Aerospike::OK) {
        echo "Register Failed";
    } else {
        echo "Register succeeded";
    }
    if($db->isConnected()) {
        $where = $db->predicateBetween("age", 920, 929);
        $res = $db->aggregate("test", "demo", $where, "stream_udf", "group_count", array("name"), $names);
        $bins = $names['bins'];
        if ($res == Aerospike::OK) {
            var_dump($bins);
        } else {
            echo "An error occured while running the AGGREGATE [{$db->errorno()}] ".$db->error();
        }
    }
});
$app->get('/setdeserializer', function() use($db) {
function array_diff_assoc_recursive($array1, $array2) {
    $difference=array();
    foreach($array1 as $key => $value) {
        if( is_array($value) ) {
            if( !isset($array2[$key]) || !is_array($array2[$key]) ) {
                $difference[$key] = $value;
            } else {
                $new_diff = array_diff_assoc_recursive($value, $array2[$key]);
                if( !empty($new_diff) )
                    $difference[$key] = $new_diff;
            }
        } else if( !array_key_exists($key,$array2) || $array2[$key] != $value )     {
            $difference[$key] = $value;
        }
    }
    return $difference;
}

    $key = $db->initKey("test", "demo", "map_of_objects_with_UDF_serializer");
    class Employee 
    {   
        public $desg = 'Manager';
    }
 $obj1 = new Employee();
     $obj2 = new Employee();
     $obj3 = new Employee();
     $obj4 = new Employee();

     $map1 = array(12=>$obj1, $obj3);
     $map2 = array($map1, $obj2, " ");
     $map3 = array("k1", $obj4, 56=>$map2);
     $put_record = array("bin1"=>$map3);
    #Set Deserializer
    $db->setDeserializer(function ($val) {
        $prefix = substr($val, 0, 3);
        if ($prefix != 'r||') {
            return unserialize(substr ($val, 3));
        }
            return unserialize(substr ($val, 3));
        });

        $status = $db->get($key, $get_record, array("bin1"));
        if ($status != AEROSPIKE::OK) {
            // return($status);
            echo "Error";
        }
        if (strcmp($get_record["key"]["ns"], $key["ns"]) == 0 &&
            strcmp($get_record["key"]["set"], $key["set"]) == 0 &&
            strcmp($get_record["key"]["key"], $key["key"]) == 0) {
            $comp_res = array_diff_assoc_recursive($put_record, $get_record["bins"]);
            if(!empty($comp_res))
            {
               echo "Get failed";
            } else {
               echo "Get successful";
            }
        }
        $status = $db->remove($key, array(Aerospike::OPT_POLICY_RETRY =>
            Aerospike::POLICY_RETRY_NONE));
});
$app->get('/scanapply', function() use($db) {
    $register_status = $db->register("lua/my_udf.lua", "my_udf.lua");
    if ($register_status != Aerospike::OK) {
        echo "Register Failed";
    } else {
        echo "Register succeeded";
    }
    $status = $db->scanApply("test", "demo", "my_udf", "mytransform", array(20), $scan_id);
    if ($status != Aerospike::OK) {
        echo "\nUnable to initiate a scan";
    } else {
        echo "\nInitiated a scan with Scan ID:" . $scan_id . "\n";
    }
    do {
        sleep(10);
        $status = $db->scanInfo($scan_id, $info);
        if ($status != Aerospike::OK) {
            echo "Error no is: ".$db->errorno();
        }
    } while($info['status'] != Aerospike::SCAN_STATUS_COMPLETED);
    if ($info['progress_pct'] != 100) {
        var_dump($info);
    } else {
        var_dump($info);
    }
});
$app->get('/apply', function() use($db) {
    $register_status = $db->register("lua/my_udf.lua", "my_udf.lua");
    if ($register_status != Aerospike::OK) {
        echo "Register Failed";
    }
    $key = array("ns" => "test", "set" => "users", "key" => "1234");
    $record = array("name" => "Hey John", "email" => "hey@gmail.com");
    $status = $db->put($key, $record);
    $res = $db->apply($key, 'my_udf', 'startswith', array('email', 'hey@'), $returned);
    if ($res == Aerospike::OK) {
        if (!$returned) {
            echo "The email of the user with key {$key['key']} does not start with 'hey@'.\n";
        }
    } elseif ($res == Aerospike::ERR_UDF_NOT_FOUND) {
        echo "The UDF module my_udf.lua was not registered with the Aerospike DB.\n";
    } else {
        echo "[{$db->errorno()}] ".$db->error();
    }
    $res = $db->deregister('my_udf.lua');
    if ($res != Aerospike::OK) {
        echo "[{$db->errorno()}] ".$db->error();
    }
    $db->close();
});

$app->get('/closeconnection', function() use($db) {
    $db->close();
    echo "Connection closed";
});

$app->notFound(function () use ($app) {
    $app->response->setStatusCode(404, "Not Found")->sendHeaders();
    echo 'This is crazy, but this page was not found!';
});

$app->handle();
?>

