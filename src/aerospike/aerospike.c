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

PHP_INI_BEGIN()
//PHP_INI_ENTRY()
PHP_INI_END()

ZEND_DECLARE_MODULE_GLOBALS(aerospike)

static void aerospike_globals_ctor(zend_aerospike_globals *globals)
{
	// Initialize globals.
	globals->cb = NULL;
	globals->cb_len = 0;
	globals->priv = NULL;
	globals->priv_len = 0;
	globals->count = 0;
}

static void aerospike_globals_dtor(zend_aerospike_globals *globals)
{
	// Release any allocated globals.

	if (globals->cb) {
		efree(globals->cb);
		globals->cb = NULL;
		globals->cb_len = 0;
	}

	if (globals->priv) {
		efree(globals->priv);
		globals->priv = NULL;
		globals->priv_len = 0;
	}
}

ZEND_BEGIN_ARG_INFO_EX(aerospike_set_cb_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, function)
	ZEND_ARG_INFO(0, userdata)
ZEND_END_ARG_INFO();

static function_entry aerospike_functions[] =
{
	PHP_FE(aerospike, NULL)
	PHP_FE(aerospike_set_cb, aerospike_set_cb_arginfo)
	PHP_FE(aerospike_invoke_cb, NULL)
	{NULL, NULL, NULL}
};

static PHP_GINIT_FUNCTION(aerospike)
{
	aerospike_globals->count = 0;
}

zend_module_entry aerospike_module_entry =
{
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	PHP_AEROSPIKE_EXTNAME,
	aerospike_functions,
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

// XXX -- Test function to be removed:

/* PHP Function:  string aerospike()
   Returns a aerospike string. */
PHP_FUNCTION(aerospike)
{
	php_printf("**In aerospike aerospike() (count = %ld)**\n", AEROSPIKE_G(count));

	RETURN_STRING("This is a aerospike!", 1);
}

// XXX -- Test function to be removed:

/* PHP Function:  bool aerospike_set_cb(string cb, string userdata)
   Returns a aerospike string. */
PHP_FUNCTION(aerospike_set_cb)
{
	char *cb, *priv;
	int cb_len, priv_len;

	php_printf("**In aerospike aerospike_set_cb()**\n");

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &cb, &cb_len, &priv, &priv_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (AEROSPIKE_G(cb)) {
		efree(AEROSPIKE_G(cb));
	}
	AEROSPIKE_G(cb) = estrndup(cb, cb_len);
	AEROSPIKE_G(cb_len) = cb_len;

	if (AEROSPIKE_G(priv)) {
		efree(AEROSPIKE_G(priv));
	}
	AEROSPIKE_G(priv) = estrndup(priv, priv_len);
	AEROSPIKE_G(priv_len) = priv_len;

	RETURN_TRUE;
}

// XXX -- Test function to be removed:

/* PHP Function:  bool aerospike_invoke_cb()
   Invoke the callback function. */
PHP_FUNCTION(aerospike_invoke_cb)
{
	php_printf("**In aerospike aerospike_invoke_cb()**\n");

	char *func;
	if ((func = AEROSPIKE_G(cb))) {
		zval function_name;
		INIT_ZVAL(function_name);
		ZVAL_STRING(&function_name, func, AEROSPIKE_G(cb_len));

		php_printf("**In aerospike aerospike_invoke_cb 2**\n");

		char *arg = AEROSPIKE_G(priv);
		zval argument;
		INIT_ZVAL(argument);
		ZVAL_STRING(&argument, arg, AEROSPIKE_G(priv_len));
		zval *params[] = {
			&argument
		};
		zend_uint param_count = 1;
		zval retval_ptr;

		php_printf("**In aerospike aerospike_invoke_cb 3**\n");

		if (call_user_function(CG(function_table), NULL, &function_name, &retval_ptr, param_count, params TSRMLS_CC) == SUCCESS) {
			php_printf("***Successfully invoked function in aerospike_invoke_cb!!!\n");
			RETURN_TRUE;
		} else {
			php_printf("!!!Failed to call user function in aerospike_invoke_cb!!!\n");
			RETURN_FALSE;
		}
	} else {
		php_printf("***No callback defined ~~ Not invoking!***\n");
	}

	RETURN_FALSE;
}

/*
 *  Define the "aerospike" class.
 */

//static ZEND_BEGIN_ARG_INFO()

static zend_function_entry aerospike_class_functions[] =
{
	/*
	 *  Client Object APIs:
	 */

	PHP_ME(aerospike, __construct, NULL, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, __destruct, NULL, ZEND_ACC_DTOR | ZEND_ACC_PUBLIC)

	/*
	 *  Cluster Management APIs:
	 */

	PHP_ME(aerospike, connect, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, isConnected, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, close, NULL, ZEND_ACC_PUBLIC)

	PHP_ME(aerospike, getNodeNames, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, getNodes, NULL, ZEND_ACC_PUBLIC)

	PHP_ME(aerospike, info, NULL, ZEND_ACC_PUBLIC)

	/*
	 *  Key Value Store (KVS) APIs:
	 */

	PHP_ME(aerospike, add, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, append, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, delete, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, exists, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, get, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, getHeader, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, operate, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, prepend, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, put, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, touch, NULL, ZEND_ACC_PUBLIC)

#if 0 // TBD

	// Scan APIs:
	// Secondary Index APIs:
	// Query APIs:
	// User Defined Function (UDF) APIs:
	// Large Data Type (LDT) APIs:
	// Logging APIs:
	// Shared Memory APIs:

	PHP_ME(aerospike, , NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, , NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, , NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, , NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, , NULL, ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, , NULL, ZEND_ACC_PUBLIC)
#endif

	{ NULL, NULL, NULL }
};

static zend_class_entry *aerospike_ce;

static zend_object_handlers aerospike_handlers;

typedef struct aerospike_object {
	zend_object std;
	int value;
} aerospike_object;

static void aerospike_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	aerospike_object *intern = (aerospike_object *) object;
	zend_object_std_dtor(&(intern->std) TSRMLS_CC);
	efree(object);
}

static void aerospike_object_free_storage(void *object TSRMLS_DC)
{
	aerospike_object *intern = (aerospike_object *) object;

	if (!intern) {
		return;
	}

	zend_object_std_dtor(&intern->std TSRMLS_CC);
	efree(intern);
}

zend_object_value aerospike_object_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	aerospike_object *intern;

	intern = ecalloc(1, sizeof(aerospike_object));

	zend_object_std_init(&(intern->std), ce TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &ce->default_properties, (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, aerospike_object_dtor, (zend_objects_free_object_storage_t) aerospike_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = &aerospike_handlers;

	return retval;
}

/*
 *  Client Object APIs:
 */

/* PHP Method:  aerospike::__construct()
   Constructs a new "aerospike" object. */
PHP_METHOD(aerospike, __construct)
{
//	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);

	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::__construct() method**\n");

	/*** TO BE IMPLEMENTED ***/

//	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}

/* PHP Method:  bool aerospike::__destruct()
   Simple aerospike method. */
PHP_METHOD(aerospike, __destruct)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::__destruct() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/*
 *  Cluster Management APIs:
 */

/* PHP Method:  bool aerospike::connect()
   Connect the client to an Aerospike cluster. */
PHP_METHOD(aerospike, connect)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::connect() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool aerospike::is_connected()
   Is the client connected to the Aerospike cluster? */
PHP_METHOD(aerospike, is_connected)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::is_connected() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool aerospike::close()
   Disconnect the client to an Aerospike cluster. */
PHP_METHOD(aerospike, close)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::close() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool aerospike::getNodeNames()
   Return an array of the names of the nodes in the Aerospike cluster. */
PHP_METHOD(aerospike, getNodeNames)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::getNodeNames() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool aerospike::getNodes()
   Return an array of objects for the nodes in the Aerospike cluster. */
PHP_METHOD(aerospike, getNodes)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::getNodes() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool aerospike::info()
   Send an Info. request to an Aerospike cluster. */
PHP_METHOD(aerospike, info)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::info() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/*
 *  Key Value Store (KVS) APIs:
 */

/* PHP Method:  bool aerospike::()
    Add integer bin values to existing bin values. */
PHP_METHOD(aerospike, add)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::add() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool aerospike::append()
    Append bin string values to existing record bin values. */
PHP_METHOD(aerospike, append)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::append() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool aerospike::delete()
    Delete record for specified key. */
PHP_METHOD(aerospike, delete)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::delete() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool aerospike::exists()
    Check if record key(s) exist in one batch call. */
PHP_METHOD(aerospike, exists)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::exists() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool aerospike::get()
    Read record header(s) and bin(s) for specified key(s) in one batch call. */
PHP_METHOD(aerospike, get)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::get() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool aerospike::getHeader()
    Read record generation and expiration for specified key(s) in one batch call. */
PHP_METHOD(aerospike, getHeader)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::getHeader() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool aerospike::operate()
    Perform multiple read/write operations on a single key in one batch call. */
PHP_METHOD(aerospike, operate)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::operate() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool aerospike::prepend()
    Prepend bin string values to existing record bin values. */
PHP_METHOD(aerospike, prepend)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::prepend() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool aerospike::put()
    Write record bin(s). */
PHP_METHOD(aerospike, put)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::put() method**\n");

	/*** TO BE IMPLEMENTED ***/

	RETURN_TRUE;
}

/* PHP Method:  bool aerospike::touch()
    Create record if it does not already exist. */
PHP_METHOD(aerospike, touch)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	// DEBUG
	php_printf("**In aerospike::touch() method**\n");

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
	INIT_CLASS_ENTRY(ce, "aerospike", aerospike_class_functions);

	if (!(aerospike_ce = zend_register_internal_class(&ce TSRMLS_CC))) {
		return FAILURE;
	}

	aerospike_ce->create_object = aerospike_object_new;

	memcpy(&aerospike_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
//	aerospike_handlers.clone_obj = aerospike_object_clone;

//	zend_class_implements(aerospike_ce TSRMLS_CC, 1, zend_ce_iterator);

	aerospike_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;
//	aerospike_ce->get_iterator = aerospike_get_iterator;

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

	AEROSPIKE_G(count)++;

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(aerospike)
{
	php_printf("**In aerospike rshutdown**\n");

	if (AEROSPIKE_G(cb)) {
		efree(AEROSPIKE_G(cb));
		AEROSPIKE_G(cb) = NULL;
		AEROSPIKE_G(cb_len) = 0;
	}

	if (AEROSPIKE_G(priv)) {
		efree(AEROSPIKE_G(priv));
		AEROSPIKE_G(priv) = NULL;
		AEROSPIKE_G(priv_len) = 0;
	}

	php_printf(">>>count = %ld<<<\n", AEROSPIKE_G(count));

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
