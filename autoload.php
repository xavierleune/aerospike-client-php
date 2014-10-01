<?php
/* If you are getting this package through Composer the Aerospike\LDT autoloader
 * is already registered.  Otherwise, you can include this file and it will 
 * handle registering the autoloader.
 */
$autoloaders = spl_autoload_functions();
if (!$autoloaders || !array_key_exists('Aerospike\\LDT\\Autoloader', $autoloaders)) {
    require __DIR__. '/src/LDT/Loader.php';
    \Aerospike\LDT\Autoloader::register();
}

?>
