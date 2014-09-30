<?php
/**
 * Copyright 2013-2014 Aerospike, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @package    Aerospike
 * @subpackage LDT
 * @category   Database
 * @author     Ronen Botzer <rbotzer@aerospike.com>
 * @copyright  Copyright 2013-2014 Aerospike, Inc.
 * @license    http://www.apache.org/licenses/LICENSE-2.0 Apache License, Version 2
 * @link       http://www.aerospike.com/docs/guide/llist.html
 */

namespace Aerospike\LDT;
use Aerospike;

/**
 * Large Data Types (LDTs) allow individual record bins to contain collections
 * of hundreds of thousands of objects. Developers can employ LDTs to
 * manipulate large amounts of data quickly and efficiently, without being 
 * concerned with record or record bin size limitations.
 *
 * @package    Aerospike
 * @subpackage LDT
 * @link       http://www.aerospike.com/docs/guide/llist.html
 * @author     Ronen Botzer <rbotzer@aerospike.com>
 */
abstract class LDT
{
    // LDT Types:
    const LLIST  = 1; // Large Ordered List
    const LMAP   = 2; // Large Map
    const LSET   = 3; // Large Set
    const LSTACK = 4; // Large Stack

    /* LDT Status Codes:
     * Each Aerospike API method invocation returns a status code
     *  depending upon the success or failure condition of the call.
     *
     * The error status codes map to the C client AEROSPIKE_ERR_LDT_* codes
     *  src/include/aerospike/as_status.h
     */
    const OK                        =    0; // Success
    const ERR_LDT                   = 1300; // Generic LDT error
    const ERR_INPUT_PARAM           = 1409; // Generic input parameter error
    const ERR_INTERNAL              = 1400; // Generic server-side error
    const ERR_NOT_FOUND             = 1401; // Element not found
    const ERR_UNIQUE_KEY            = 1402; // Duplicate element written when 'unique key' set
    const ERR_INSERT                = 1403; // Generic error for insertion op
    const ERR_SEARCH                = 1404; // Generic error for search op
    const ERR_DELETE                = 1405; // Generic error for delete op
    const ERR_TYPE_MISMATCH         = 1410; // LDT type mismatched for the bin
    const ERR_NULL_BIN_NAME         = 1411; // The LDT bin name is null
    const ERR_BIN_NAME_NOT_STRING   = 1412; // The LDT bin name must be a string
    const ERR_BIN_NAME_TOO_LONG     = 1413; // The LDT bin name exceeds 14 chars
    const ERR_TOO_MANY_OPEN_SUBRECS = 1414; // Server-side error: open subrecs
    const ERR_TOP_REC_NOT_FOUND     = 1415; // record containing the LDT not found
    const ERR_SUB_REC_NOT_FOUND     = 1416; // Server-side error: subrec not found
    const ERR_BIN_DOES_NOT_EXIST    = 1417; // LDT bin does not exist
    const ERR_BIN_ALREADY_EXISTS    = 1418; // Collision creating LDT at bin
    const ERR_BIN_DAMAGED           = 1419; // Control structures in the top record are damaged
    const ERR_SUBREC_POOL_DAMAGED   = 1420; // Subrec pool is damaged
    const ERR_SUBREC_DAMAGED        = 1421; // Control structures in the sub record are damaged
    const ERR_SUBREC_OPEN           = 1422; // Error while opening the sub record
    const ERR_SUBREC_UPDATE         = 1423; // Error while updating the sub record
    const ERR_SUBREC_CREATE         = 1424; // Error while creating the sub record
    const ERR_SUBREC_DELETE         = 1425; // Error while deleting the sub record
    const ERR_SUBREC_CLOSE          = 1426; // Error while closing the sub record
    const ERR_TOPREC_UPDATE         = 1427; // Error while updating the top record
    const ERR_TOPREC_CREATE         = 1428; // Error while creating the top record

    protected $key;
    protected $bin;
    protected $type;
    protected $module;
    protected $db;
    protected $error;
    protected $errorno;

    /**
     * Constructor for the abstract \Aerospike\LDT class. Inherited by LDT types.
     *
     * @param Aerospike $db
     * @param array $key initialized with Aerospike::initKey()
     * @param string $bin name
     * @param int $type of the LDT
     * @see errorno()
     * @see error()
     */
    protected function __construct(Aerospike $db, array $key, $bin, $type) {
        $this->key = $key;
        $this->bin = $bin;
        $this->type = $type;
        $this->module = $this->getModuleName();
        $this->db = $db;
        if (!$db->isConnected()) {
            if ($db->errorno() != Aerospike::OK) {
                $this->errorno = $db->errorno();
                $this->error = $db->error();
            } else {
                $this->errorno = Aerospike::ERR_CLUSTER;
                $this->error = "Aerospike object could not successfully connect to the cluster";
            }
        } else {
            $this->errorno = Aerospike::OK;
            $this->error = '';
        }
    }

    /**
     * Returns the error message for the previous operation.
     *
     * @return string
     */
    public function error() {
        return $this->error;
    }

    /**
     * Returns the status code for the previous operation.
     *
     * @return int
     */
    public function errorno() {
        return $this->errorno;
    }

    /**
     * Checks whether there actually is an LDT at the key and bin the class
     * was initialized with.
     *
     * @return boolean
     */
    public function isLDT() {
        $res = $this->db->apply($this->key, $this->module, 'ldt_exists', array($this->bin), $returned);
        $this->processStatusCode($res);
        if ($res !== Aerospike::OK) {
            return false;
        } else {
            return (boolean) $returned;
        }
    }

    /**
     * Sets $num_elements with the number of elements in the LDT.
     *
     * @param int $num_elements returned
     * @return int status code of the operation
     */
    public function size(&$num_elements) {
        $res = $this->db->apply($this->key, $this->module, 'size', array($this->bin), $num_elements);
        $this->processStatusCode($res);
        return $res;
    }

    /**
     * Sets $num_elements with the max number of elements the LDT can hold.
     *
     * @param int $num_elements returned
     * @return int status code of the operation
     */
    public function getCapacity(&$num_elements) {
        $res = $this->db->apply($this->key, $this->module, 'get_capacity', array($this->bin), $num_elements);
        $this->processStatusCode($res);
        return $res;
    }

    /**
     * Sets the max number of elements that the LDT can hold.
     *
     * @param int $num_elements
     * @return int status code of the operation
     */
    public function setCapacity($num_elements) {
        $res = $this->db->apply($this->key, $this->module, 'set_capacity', array($this->bin, $num_elements));
        $this->processStatusCode($res);
        return $res;
    }

    /**
     * Destroy the LDT at the key and bin the class was initialized to.
     * <code>
     * namespace Aerospike\LDT;
     * use Aerospike;
     * $config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
     * $db = new \Aerospike($config);
     * $key = $db->initKey("test", "user", 1);
     * $llist = new \Aerospike\LDT\LList($db, $key, "timeline2");
     * $res = $llist->destroy();
     * if ($res !== \Aerospike\LDT::OK) {
     *     var_dump($llist->error(), $llist->errorno());
     * }
     * </code>
     * @return int status code of the operation
     */
    public function destroy() {
        $res = $this->db->apply($this->key, $this->module, 'destroy', array($this->bin));
        $this->processStatusCode($res);
        return $res;
    }

    /**
     * Process the operation status code into error number and message
     *
     * @param int $status code of the operation
     */
    protected function processStatusCode($status) {
        if ($status !== Aerospike::OK) {
            $this->error = $this->db->error();
            $this->errorno = $this->db->errorno();
        } else {
            $this->error = '';
            $this->errorno = self::OK;
        }
    }

    private function getModuleName() {
        switch($this->type) {
            case self::LLIST:
                return 'llist';
            case self::LMAP:
                return 'lmap';
            case self::LSET:
                return 'lset';
            case self::LSTACK:
                return 'lstack';
            default:
                return '';
        }
    }

}

?>
