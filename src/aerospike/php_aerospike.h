/*
 * src/aerospike/php_aerospike.h
 *
 * Copyright (C) 2013-2014 Aerospike, Inc.
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */

/*
 *  SYNOPSIS
 *    This is the Aerospike PHP Client API Zend Engine extension declarations file.
 *
 *    Please see "doc/apiref.md" for detailed information about the API and
 *    "doc/internals.md" for the internal architecture of the client.
 */

#ifndef PHP_AEROSPIKE_H
#define PHP_AEROSPIKE_H 1

#define PHP_AEROSPIKE_VERSION "3.0"
#define PHP_AEROSPIKE_EXTNAME "aerospike"

ZEND_BEGIN_MODULE_GLOBALS(aerospike)
/* N.B.:  No globals defined for now. */
ZEND_END_MODULE_GLOBALS(aerospike)

#ifdef ZTS
#define AEROSPIKE_G(v) TSRMG(aerospike_globals_id, zend_aerospike_globals *v)
extern int aerospike_globals_id;
#else
#define AEROSPIKE_G(v) (aerospike_globals.v)
extern zend_aerospike_globals aerospike_globals;
#endif

PHP_MINIT_FUNCTION(aerospike);
PHP_MSHUTDOWN_FUNCTION(aerospike);
PHP_RINIT_FUNCTION(aerospike);
PHP_RSHUTDOWN_FUNCTION(aerospike);
PHP_MINFO_FUNCTION(aerospike);

// Client Object APIs:

PHP_METHOD(Aerospike, __construct);
PHP_METHOD(Aerospike, __destruct);

// Cluster Management APIs:

PHP_METHOD(Aerospike, isConnected);
PHP_METHOD(Aerospike, close);
PHP_METHOD(Aerospike, getNodes);
PHP_METHOD(Aerospike, info);

// Key Value Store (KVS) APIs:

PHP_METHOD(Aerospike, add);
PHP_METHOD(Aerospike, append);
PHP_METHOD(Aerospike, delete);
PHP_METHOD(Aerospike, exists);
PHP_METHOD(Aerospike, get);
PHP_METHOD(Aerospike, getMany);
PHP_METHOD(Aerospike, getHeader);
PHP_METHOD(Aerospike, getHeaderMany);
PHP_METHOD(Aerospike, operate);
PHP_METHOD(Aerospike, prepend);
PHP_METHOD(Aerospike, put);
PHP_METHOD(Aerospike, touch);

// TBD

// Scan APIs:
// Secondary Index APIs:
// Query APIs:
// User Defined Function (UDF) APIs:
// Large Data Type (LDT) APIs:
// Logging APIs:
// Shared Memory APIs:

extern zend_module_entry aerospike_module_entry;
#define phpext_aerospike_ptr &aerospike_module_entry

#endif // defined(PHP_AEROSPIKE_H)
