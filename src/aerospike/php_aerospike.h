/*
 * src/aerospike/php_aerospike.h
 *
 * Copyright (C) 2014-2105 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
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
#ifdef ZTS
#include "TSRM.h"
#endif
#include "aerospike/as_error.h"
#include "pthread.h"

typedef struct global_error {
    int reset;
    as_error error;
} aerospike_global_error;

ZEND_BEGIN_MODULE_GLOBALS(aerospike)
    int nesting_depth;
    int connect_timeout;
    int read_timeout;
    int write_timeout;
    char *log_path;
    char *log_level;
    int serializer;
    char *lua_system_path;
    char *lua_user_path;
    int key_policy;
    int key_gen;
    zend_bool shm_use;
    int shm_max_nodes;
    int shm_max_namespaces;
    int shm_takeover_threshold_sec;
    aerospike_global_error error_g;
    HashTable *persistent_list_g;
    int persistent_ref_count;
    pthread_rwlock_t aerospike_mutex;
    pthread_rwlock_t query_cb_mutex;
ZEND_END_MODULE_GLOBALS(aerospike)

ZEND_EXTERN_MODULE_GLOBALS(aerospike);

#ifdef ZTS
#define AEROSPIKE_G(v) (((zend_aerospike_globals*)(*((void ***)tsrm_ls))[(aerospike_globals_id)-1])->v)
zend_aerospike_globals *globals;
#else
#define AEROSPIKE_G(v) (aerospike_globals.v)
#endif


PHP_MINIT_FUNCTION(aerospike);
PHP_MSHUTDOWN_FUNCTION(aerospike);
PHP_RINIT_FUNCTION(aerospike);
PHP_RSHUTDOWN_FUNCTION(aerospike);
PHP_MINFO_FUNCTION(aerospike);

/*
 * Client Object APIs:
 */

PHP_METHOD(Aerospike, __construct);
PHP_METHOD(Aerospike, __destruct);

/*
 * Cluster Management APIs:
 */

PHP_METHOD(Aerospike, isConnected);
PHP_METHOD(Aerospike, close);
PHP_METHOD(Aerospike, reconnect);
PHP_METHOD(Aerospike, getNodes);
PHP_METHOD(Aerospike, info);
PHP_METHOD(Aerospike, infoMany);

/*
 * Error Handling APIs:
 */

PHP_METHOD(Aerospike, error);
PHP_METHOD(Aerospike, errorno);

/*
 * Key Value Store (KVS) APIs:
 */

PHP_METHOD(Aerospike, append);
PHP_METHOD(Aerospike, exists);
PHP_METHOD(Aerospike, get);
PHP_METHOD(Aerospike, getMany);
PHP_METHOD(Aerospike, getMetadata);
PHP_METHOD(Aerospike, getHeader);
PHP_METHOD(Aerospike, getHeaderMany);
PHP_METHOD(Aerospike, initKey);
PHP_METHOD(Aerospike, getKeyDigest);
PHP_METHOD(Aerospike, increment);
PHP_METHOD(Aerospike, operate);
PHP_METHOD(Aerospike, prepend);
PHP_METHOD(Aerospike, put);
PHP_METHOD(Aerospike, remove);
PHP_METHOD(Aerospike, removeBin);
PHP_METHOD(Aerospike, setDeserializer);
PHP_METHOD(Aerospike, setSerializer);
PHP_METHOD(Aerospike, touch);

/*
 * Logging APIs:
 */

PHP_METHOD(Aerospike, setLogLevel);
PHP_METHOD(Aerospike, setLogHandler);

/*
 * Secondary Index APIs:
 */

PHP_METHOD(Aerospike, createIndex);
PHP_METHOD(Aerospike, addIndex);
PHP_METHOD(Aerospike, dropIndex);

/*
 * Query and Scan APIs:
 */

PHP_METHOD(Aerospike, predicateBetween);
PHP_METHOD(Aerospike, predicateEquals);
PHP_METHOD(Aerospike, predicateContains);
PHP_METHOD(Aerospike, predicateRange);
PHP_METHOD(Aerospike, query);
PHP_METHOD(Aerospike, aggregate);
PHP_METHOD(Aerospike, scan);
PHP_METHOD(Aerospike, scanApply);
PHP_METHOD(Aerospike, scanInfo);

/*
 * User Defined Function (UDF) APIs:
 */

PHP_METHOD(Aerospike, register);
PHP_METHOD(Aerospike, deregister);
PHP_METHOD(Aerospike, apply);
PHP_METHOD(Aerospike, listRegistered);
PHP_METHOD(Aerospike, getRegistered);

/*
 * Batch Operations APIs:
 */
PHP_METHOD(Aerospike, existsMany);
PHP_METHOD(Aerospike, operate);

/*
 * TBD
 * Large Data Type (LDT) APIs:
 * Shared Memory APIs:
 */

extern zend_module_entry aerospike_module_entry;
#define phpext_aerospike_ptr &aerospike_module_entry

#endif // defined(PHP_AEROSPIKE_H)
