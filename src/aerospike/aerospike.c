/*
    File:   src/aerospike/aerospike.c

    Description:
       Aerospike PHP Client API Zend Engine extension implementation.

       The extension has the following functions:
	      string aerospike()
		  bool aerospike_set_cb(string cb, string userdata)
		  bool aerospike_invoke_cb()

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

/* PHP Function:  string aerospike()
   Returns a aerospike string. */
PHP_FUNCTION(aerospike)
{
	php_printf("**In aerospike aerospike() (count = %ld)**\n", AEROSPIKE_G(count));

	RETURN_STRING("This is a aerospike!", 1);
}

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
	PHP_ME(aerospike, __construct, NULL, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
	PHP_ME(aerospike, aerospike, NULL, ZEND_ACC_PUBLIC)
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

/* PHP Method:  bool aerospike::aerospike()
   Simple aerospike method. */
PHP_METHOD(aerospike, aerospike)
{
	zval *object = getThis();
	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	php_printf("**In aerospike method**\n");

	RETURN_TRUE;
}

/* PHP Method:  aerospike::__construct()
   Constructs a new "aerospike" object. */
PHP_METHOD(aerospike, __construct)
{
//	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);

	aerospike_object *intern = (aerospike_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

//	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
}

#if 0
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
