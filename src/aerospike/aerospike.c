/*
 * src/aerospike/aerospike.c
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
 *    This is the Aerospike PHP Client API Zend Engine extension implementation.
 *
 *    Please see "doc/apiref.md" for detailed information about the API and
 *    "doc/internals.md" for the internal architecture of the client.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_aerospike.h"

#include "aerospike/aerospike.h"

PHP_INI_BEGIN()
//PHP_INI_ENTRY()
PHP_INI_END()

ZEND_DECLARE_MODULE_GLOBALS(aerospike)

static void aerospike_globals_ctor(zend_aerospike_globals *globals)
{
	// Initialize globals.

	// [None for now.]
}

static void aerospike_globals_dtor(zend_aerospike_globals *globals)
{
	// Release any allocated globals.

	// [None for now.]
}

static PHP_GINIT_FUNCTION(aerospike)
{
	// Perform extension global initializations.

	// [None for now.]
}

zend_module_entry aerospike_module_entry =
{
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	PHP_AEROSPIKE_EXTNAME,
	NULL, /* N.B.:  No functions provided by this extension, only classes. */
	PHP_MINIT(aerospike),
	PHP_MSHUTDOWN(aerospike),
	PHP_RINIT(aerospike),
	PHP_RSHUTDOWN(aerospike),
	PHP_MINFO(aerospike),
#if  ZEND_MODULE_API_NO >= 20010901
	PHP_AEROSPIKE_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_AEROSPIKE
ZEND_GET_MODULE(aerospike)
#endif

/*
 *  Define the "Aerospike" class.
 */

//static ZEND_BEGIN_ARG_INFO()

static zend_function_entry Aerospike_class_functions[] =
{
	/*
	 *  Client Object APIs:
	 */

	PHP_ME(Aerospike, __construct, NULL, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, __destruct, NULL, ZEND_ACC_DTOR | ZEND_ACC_PUBLIC)

	/*
	 *  Cluster Management APIs:
	 */

	PHP_ME(Aerospike, isConnected, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, close, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, getNodes, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, info, NULL, ZEND_ACC_PUBLIC)

	/*
	 *  Key Value Store (KVS) APIs:
	 */

	PHP_ME(Aerospike, add, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, append, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, delete, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, exists, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, get, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, getHeader, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, operate, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, prepend, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, put, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, touch, NULL, ZEND_ACC_PUBLIC)

#if 0 // TBD

	// Scan APIs:
	// Secondary Index APIs:
	// Query APIs:
	// User Defined Function (UDF) APIs:
	// Large Data Type (LDT) APIs:
	// Logging APIs:
	// Shared Memory APIs:

	PHP_ME(Aerospike, , NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, , NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, , NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, , NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, , NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Aerospike, , NULL, ZEND_ACC_PUBLIC)
#endif

	{ NULL, NULL, NULL }
};

static zend_class_entry *Aerospike_ce;

static zend_object_handlers Aerospike_handlers;

typedef struct Aerospike_object {
	zend_object std;
	int value;
} Aerospike_object;

static void Aerospike_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	Aerospike_object *intern = (Aerospike_object *) object;
	zend_object_std_dtor(&(intern->std) TSRMLS_CC);
	efree(object);
}

static void Aerospike_object_free_storage(void *object TSRMLS_DC)
{
	Aerospike_object *intern = (Aerospike_object *) object;

	if (!intern) {
		return;
	}

	zend_object_std_dtor(&intern->std TSRMLS_CC);
	efree(intern);
}

zend_object_value Aerospike_object_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	Aerospike_object *intern;

	intern = ecalloc(1, sizeof(Aerospike_object));

	zend_object_std_init(&(intern->std), ce TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &ce->default_properties, (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, Aerospike_object_dtor, (zend_objects_free_object_storage_t) Aerospike_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = &Aerospike_handlers;

	return retval;
}

/*
 *  Client Object APIs:
 */

aerospike as;

/* PHP Method:  aerospike::__construct()
   Constructs a new "aerospike" object. */
PHP_METHOD(Aerospike, __construct)
{
//	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);

	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	// DEBUG
	php_printf("**In Aerospike::__construct() method**\n");

	// XXX -- Temporary implementation based on globals.

	// Errors populate this object.
	as_error err;

	// Configuration for the client.
	as_config config;
	as_config_init(&config);

	// XXX -- Should process constructor arguments here.

    // XXX -- For now, if none are supplied, default to localhost.

	// Add a seed host for cluster discovery.
#if 0
	// XXX -- This doesn't work with the current build options.
	config.hosts[0] = { .addr = "127.0.0.1", .port = 3000 };
#else
	config.hosts[0].addr = "127.0.0.1";
	config.hosts[0].port = 3000;
#endif

	// The Aerospike client instance, initialized with the configuration.
	aerospike_init(&as, &config);

	// Connect to the cluster.
	if (aerospike_connect(&as, &err) != AEROSPIKE_OK) {
		// An error occurred, so we log it.
		fprintf(stderr, "error(%d) %s at [%s:%d]\n", err.code, err.message, err.file, err.line);

		// XXX -- Release any allocated resources and throw and exception.

	} else {
		fprintf(stderr, "It worked!\n");
	}

	/*** TO BE IMPLEMENTED ***/

//	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}

/* PHP Method:  bool Aerospike::__destruct()
   Perform Aerospike object finalization. */
PHP_METHOD(Aerospike, __destruct)
{
	zval *object = getThis();
	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In Aerospike::__destruct() method**\n");

	// XXX -- Temporary implementation based on globals.

	// Cleanup the resources used by the client
	aerospike_destroy(&as);

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/*
 *  Cluster Management APIs:
 */

/* PHP Method:  bool Aerospike::isConnected()
   Is the client connected to the Aerospike cluster? */
PHP_METHOD(Aerospike, isConnected)
{
	zval *object = getThis();
	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In Aerospike::isConnected() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::close()
   Disconnect the client to an Aerospike cluster. */
PHP_METHOD(Aerospike, close)
{
	zval *object = getThis();
	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In Aerospike::close() method**\n");

	// Errors populate this object.
	as_error err;

    // We are finished with the client.
    if (aerospike_close(&as, &err) != AEROSPIKE_OK) {
		// An error occurred, so we log it.
		fprintf(stderr, "error(%d) %s at [%s:%d]\n", err.code, err.message, err.file, err.line);
	} else {
		fprintf(stderr, "It worked!\n");
	}

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::getNodes()
   Return an array of objects for the nodes in the Aerospike cluster. */
PHP_METHOD(Aerospike, getNodes)
{
	zval *object = getThis();
	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In Aerospike::getNodes() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::info()
   Send an Info. request to an Aerospike cluster. */
PHP_METHOD(Aerospike, info)
{
	zval *object = getThis();
	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In Aerospike::info() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/*
 *  Key Value Store (KVS) APIs:
 */

/* PHP Method:  bool Aerospike::()
    Add integer bin values to existing bin values. */
PHP_METHOD(Aerospike, add)
{
	zval *object = getThis();
	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In Aerospike::add() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::append()
    Append bin string values to existing record bin values. */
PHP_METHOD(Aerospike, append)
{
	zval *object = getThis();
	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In Aerospike::append() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::delete()
    Delete record for specified key. */
PHP_METHOD(Aerospike, delete)
{
	zval *object = getThis();
	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In Aerospike::delete() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::exists()
    Check if record key(s) exist in one batch call. */
PHP_METHOD(Aerospike, exists)
{
	zval *object = getThis();
	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In Aerospike::exists() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::get()
    Read record header(s) and bin(s) for specified key(s) in one batch call. */
PHP_METHOD(Aerospike, get)
{
	zval *object = getThis();
	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In Aerospike::get() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::getHeader()
    Read record generation and expiration for specified key(s) in one batch call. */
PHP_METHOD(Aerospike, getHeader)
{
	zval *object = getThis();
	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In Aerospike::getHeader() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::operate()
    Perform multiple read/write operations on a single key in one batch call. */
PHP_METHOD(Aerospike, operate)
{
	zval *object = getThis();
	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In Aerospike::operate() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::prepend()
    Prepend bin string values to existing record bin values. */
PHP_METHOD(Aerospike, prepend)
{
	zval *object = getThis();
	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In Aerospike::prepend() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::put()
    Write record bin(s). */
PHP_METHOD(Aerospike, put)
{
	zval *object = getThis();
	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In Aerospike::put() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::touch()
    Create record if it does not already exist. */
PHP_METHOD(Aerospike, touch)
{
	zval *object = getThis();
	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In Aerospike::touch() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/*
 *  Scan APIs:
 */

/*** TBD ***/

/*
 *  Secondary Index APIs:
 */

/*** TBD ***/

/*
 *  Query APIs:
 */

/*** TBD ***/

/*
 *  User Defined Function (UDF) APIs:
 */

/*** TBD ***/

/*
 *  Large Data Type (LDT) APIs:
 */

/*** TBD ***/

/*
 *  Logging APIs:
 */

/*** TBD ***/

/*
 *  Shared Memory APIs:
 */

/*** TBD ***/

#if 0 // XXX -- Currently unused.  Delete???
static int zend_std_cast_object_tostring(zval *readobj, zval *writeobj, int type TSRMLS_DC)
{
	zval *retval = NULL;
	if (type == IS_STRING) {
		zend_call_method_wiht_0_params(&readobj, NULL, NULL, "__tostring", &retval);
		if (retval) {
			if (Z_TYPE_P(retval) != IS_STRING) {
				zend_error(E_ERROR, "Method %s::__toString() must return a string value", Z_OBJCE_P(readobj)->name);
			}
		} else {
			MAKE_STD_ZVAL(retval);
			ZVAL_EMPTY_STRING(retval);
		}
		ZVAL_ZVAL(writeobj, retval, 1, 1);
		INT_PZVAL(writeobj);
	}

	return retval ? SUCCESS : FAILURE;
}
#endif

PHP_MINIT_FUNCTION(aerospike)
{
	php_printf("**In aerospike minit**\n");

	REGISTER_INI_ENTRIES();

	ZEND_INIT_MODULE_GLOBALS(aerospike, aerospike_globals_ctor, aerospike_globals_dtor);

	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Aerospike", Aerospike_class_functions);

	if (!(Aerospike_ce = zend_register_internal_class(&ce TSRMLS_CC))) {
		return FAILURE;
	}

	Aerospike_ce->create_object = Aerospike_object_new;

	memcpy(&Aerospike_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
//	Aerospike_handlers.clone_obj = Aerospike_object_clone;

//	zend_class_implements(Aerospike_ce TSRMLS_CC, 1, zend_ce_iterator);

	Aerospike_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;
//	Aerospike_ce->get_iterator = Aerospike_get_iterator;

	// Define constants.
	// [Note:  Negative status values come from the client;
	//         positive status values come from the server.]

	// Client status codes:

	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("COMMAND_REJECTED"), -8);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("QUERY_TERMINATED"), -7);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("SCAN_TERMINATED"), -6);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("NO_HOSTS"), -5);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("INVALID_API_PARAM"), -4);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("FAIL_ASYNCQ_FULL"), -3);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("FAIL_TIMEOUT"), -2);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("FAIL_CLIENT"), -1);

	// Server status codes:

	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("OK"), 0);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("SERVER_ERROR"), 1);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("KEY_NOT_FOUND_ERROR"), 2);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("GENERATION_ERROR"), 3);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("PARAMETER_ERROR"), 4);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("KEY_FOUND_ERROR"), 5);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("BIN_FOUND_ERROR"), 6);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("CLUSTER_KEY_MISMATCH"), 7);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("PARTITION_OUT_OF_SPACE"), 8);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("SERVERSIDE_TIMEOUT"), 9);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("NO_XDR"), 10);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("SERVER_UNAVAILABLE"), 11);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("INCOMPATIBLE_TYPE"), 12);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("RECORD_TOO_BIG"), 13);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("KEY_BUSY"), 14);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("SCAN_ABORT"), 15);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("UNSUPPORTED_FEATURE"), 16);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("BIN_NOT_FOUND"), 17);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("DEVICE_OVERLOAD"), 18);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("KEY_MISMATCH"), 19);

	// UDF status codes:

	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("UDF_BAD_RESPONSE"), 100);

	// Secondary Index status codes:

	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("INDEX_FOUND"), 200);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("INDEX_NOTFOUND"), 201);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("INDEX_OOM"), 202);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("INDEX_NOTREADABLE"), 203);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("INDEX_GENERIC"), 204);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("NAME_MAXLEN"), 205);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("MAXCOUNT"), 206);

	// Query statue codes:

	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("ABORTED"), 210);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("QUERY_QUEUEFULL"), 211);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("QUERY_TIMEOUT"), 212);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("QUERY_GENERIC"), 213);

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(aerospike)
{
	php_printf("**In aerospike mshutdown**\n");

#ifndef ZTS
	aerospike_globals_dtor(&aerospike_globals TSRMLS_CC);
#endif

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}

PHP_RINIT_FUNCTION(aerospike)
{
	php_printf("**In aerospike rinit**\n");

	/*** TO BE IMPLEMENTED ***/

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(aerospike)
{
	php_printf("**In aerospike rshutdown**\n");

	/*** TO BE IMPLEMENTED ***/

	return SUCCESS;
}

PHP_MINFO_FUNCTION(aerospike)
{
	php_printf("**In aerospike info**\n");

	php_info_print_table_start();
	php_info_print_table_row(2, "aerospike support", "enabled");
	php_info_print_table_row(2, "aerospike version", PHP_AEROSPIKE_VERSION);
	php_info_print_table_end();
}
