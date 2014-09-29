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

require 'LDT.php';

/**
 * Large Ordered List (llist) is optimized for searching and updating sorted
 * lists. It can access data at any point in the collection, while still being
 * capable of growing the collection to virtually any size.
 *
 * @package    Aerospike
 * @subpackage LDT
 * @link       http://www.aerospike.com/docs/guide/llist.html
 * @author     Ronen Botzer <rbotzer@aerospike.com>
 */
class LList extends LDT
{
    // error messages
    const MSG_TYPE_NOT_SUPPORTED = "\$value must be a supported type (string|integer|array)";
    const MSG_TYPE_NOT_ATOMIC = "\$value must be an atomic type (string|integer)";
    const MSG_RANGE_TYPE_INVALID = "\$min and \$max must be of type (string|integer|null)";

    /**
     * Constructor for the \Aerospike\LDT\LList class.
     *
     * @param Aerospike $db
     * @param array $key initialized with Aerospike::initKey()
     * @param string $bin name
     * @see LDT::__construct()
     */
    public function __construct(Aerospike $db, array $key, $bin) {
        parent::__construct($db, $key, $bin, LDT::LLIST);
    }

    /**
     * Adds a value of a supported type to the LList.
     * The elements added must be consistently the same type
     * (string, integer, array).
     *
     * @param int|string|array $value
     * @return int status code of the operation
     */
    public function add($value) {
        if (!is_string($value) && !is_int($value) && !is_array($value)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_SUPPORTED;
            return $this->errorno;
        }
        $res = $this->db->apply($this->key, 'llist', 'add', array($this->bin, $value));
        $this->processStatusCode($res);
        return $res;
    }

    /**
     * Adds values of a supported type to the LList.
     * The elements added must be consistently the same type
     * (string, integer, array).
     *
     * @param array $values
     * @return int status code of the operation
     */
    public function addMany(array $values) {
        $res = $this->db->apply($this->key, 'llist', 'add_all', array($this->bin, $values));
        $this->processStatusCode($res);
        return $res;
    }

    /**
     * Finds the elements matching the given value in the LList.
     * Atomic elements (integer, string) will be directly compared. In complex
     * types (array) the value of a key named 'key' is used for comparison.
     *
     * @param int|string $value
     * @param array $elements matched
     * @return int status code of the operation
     */
    public function find($value, &$elements) {
        if (!is_string($value) && !is_int($value)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_ATOMIC;
            return $this->errorno;
        }
        $elements = array();
        $res = $this->db->apply($this->key, 'llist', 'find', array($this->bin, $value), $elements);
        $this->processStatusCode($res);
        return $res;
    }

    /**
     * Find and remove an element matching the given value in the LList.
     * Atomic elements (integer, string) will be directly compared. In complex
     * types (array) the value of a key named 'key' is used to identify the
     * element which is to be removed.
     *
     * @param int|string $value
     * @return int status code of the operation
     */
    public function remove($value) {
        if (!is_string($value) && !is_int($value)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_ATOMIC;
            return $this->errorno;
        }
        $res = $this->db->apply($this->key, 'llist', 'remove', array($this->bin, $value));
        $this->processStatusCode($res);
        return $res;
    }

    /**
     * Returns the elements in the range between $min and $max the LList.
     * A null $min gets all elements less than or equal to $max.
     * A null $max gets all elements greater than or equal to $min.
     * If both $min and $max are null all elements in the LList are returned.
     *
     * @param array $elements returned
     * @param int|string $min
     * @param int|string $max
     * @return int status code of the operation
     */
    public function scan(&$elements, $min=null, $max=null) {
        $elements = array();
        if (is_null($min) && is_null($max)) {
            $res = $this->db->apply($this->key, 'llist', 'scan', array($this->bin), $elements);
        } else {
            if ((!is_string($min) && !is_int($min) && !is_null($min)) ||
                (!is_string($max) && !is_int($max) && !is_null($max))) {
                $this->errorno = self::ERR_INPUT_PARAM;
                $this->error = self::MSG_RANGE_TYPE_INVALID;
                return $this->errorno;
            }
            $res = $this->db->apply($this->key, 'llist', 'range', array($this->bin, $min, $max), $elements);
        }
        $this->processStatusCode($res);
        return $res;
    }

    /**
     * Scan the LList for elements and apply a UDF to the ones found.
     * If the UDF returns null the element will be filtered out of the results.
     * Otherwise, the UDF may transform the value of the element prior to
     * returning it to the result set.
     *
     * @todo To be implemented.
     * @param string $module name of the UDF library
     * @param string $function name of the UDF
     * @param array $args passed to the UDF
     * @param array $elements returned
     * @param int|string $min optionally find a range of elements to apply the UDF to
     * @param int|string $max
     * @return int status code of the operation
     */
    public function filter($module, $function, array $args, array &$elements, $min=null, $max=null) {
        $this->error = "Method not implemented";
        $this->errorno = self::ERR_LDT;
        return $this->errorno;
    }

}

?>
