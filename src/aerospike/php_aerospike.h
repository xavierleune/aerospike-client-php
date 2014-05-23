/*
    File:   src/aerospike/php_aerospike.h

    Description:
       Aerospike PHP Client API Zend Engine extension declarations file.

	Copyright (C) 2014 Aerospike, Inc.. Portions may be licensed
	to Aerospike, Inc. under one or more contributor license agreements.

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#ifndef PHP_AEROSPIKE_H
#define PHP_AEROSPIKE_H 1

#define PHP_AEROSPIKE_VERSION "3.0"
#define PHP_AEROSPIKE_EXTNAME "aerospike"

ZEND_BEGIN_MODULE_GLOBALS(aerospike)
	char *cb;
	int cb_len;
	char *priv;
	int priv_len;
	long count;
ZEND_END_MODULE_GLOBALS(aerospike)

#ifdef ZTS
#define AEROSPIKE_G(v) TSRMG(aerospike_globals_id, zend_aerospike_globals *v)
extern int aerospike_globals_id;
#else
#define AEROSPIKE_G(v) (aerospike_globals.v)
extern zend_aerospike_globals aerospike_globals;
#endif

PHP_FUNCTION(aerospike);
PHP_FUNCTION(aerospike_set_cb);
PHP_FUNCTION(aerospike_invoke_cb);

PHP_MINIT_FUNCTION(aerospike);
PHP_MSHUTDOWN_FUNCTION(aerospike);
PHP_RINIT_FUNCTION(aerospike);
PHP_RSHUTDOWN_FUNCTION(aerospike);
PHP_MINFO_FUNCTION(aerospike);

PHP_METHOD(aerospike, __construct);
PHP_METHOD(aerospike, aerospike);

extern zend_module_entry aerospike_module_entry;
#define phpext_aerospike_ptr &aerospike_module_entry

#endif // defined(PHP_AEROSPIKE_H)
