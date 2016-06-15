<?php

namespace {
    ini_set('display_errors', 'on');
    ini_set('error_reporting', E_ALL);
    ini_set('session.save_handler','aerospike');
    ini_set('session.save_path','session|sess|127.0.0.1:3000');

    // Start session
    session_start();

    class session{
        private $property = 'myProperty';
    }

    // Write session
    $_SESSION['test'] = [new \session()];
    // Read session
    $db = new Aerospike(["hosts" => [["addr" => "127.0.0.1", "port" => 3000]]]);
    $key = $db->initKey('session', 'sess', session_id());
    $status = $db->get($key, $record);

    var_dump($status, $record);

    echo htmlentities($record['bins']['PHP_SESSION']);

    var_dump($_SESSION['test']);
}