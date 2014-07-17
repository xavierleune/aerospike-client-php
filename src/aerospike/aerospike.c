/*
 * src/aerospike/aerospike.c
 *
 * Copyright (C) 2014 Aerospike, Inc.
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
#include "aerospike/aerospike_key.h"
#include "aerospike/as_error.h"
#include "aerospike/as_record.h"
#include "aerospike/as_val.h"
#include "aerospike/as_boolean.h"
#include "aerospike/as_arraylist.h"
#include "aerospike/as_hashmap.h"
#include "aerospike/as_stringmap.h"
#include "aerospike/as_status.h"

#include <stdbool.h>

#include "aerospike_common.h"
#include "aerospike_status.h"
#include "aerospike_policy.h"
#include "aerospike_logger.h"
#include "aerospike_transform.h"

/*-----------------------------------------------------------*/
typedef struct Aerospike_object {
    zend_object std;
    int value;
    aerospike *as_p;
} Aerospike_object;

static zend_class_entry *Aerospike_ce;
static zend_object_handlers Aerospike_handlers;
/*-----------------------------------------------------------*/
PHP_INI_BEGIN()
    //PHP_INI_ENTRY()
PHP_INI_END()

ZEND_DECLARE_MODULE_GLOBALS(aerospike)

static void aerospike_globals_ctor(zend_aerospike_globals *globals)
{
}

static void aerospike_globals_dtor(zend_aerospike_globals *globals)
{
}

static PHP_GINIT_FUNCTION(aerospike)
{
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
    /*
     *  Logging APIs:
     */
    PHP_ME(Aerospike, setLogLevel, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, setLogHandler, NULL, ZEND_ACC_PUBLIC)
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

static void Aerospike_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
    Aerospike_object *intern_obj_p = (Aerospike_object *) object;

    if (intern_obj_p && (intern_obj_p->as_p)) {
        aerospike_destroy(intern_obj_p->as_p);
    	intern_obj_p->as_p = NULL;
        DEBUG_PHP_EXT_INFO("aerospike c sdk object destroyed");
    } else {
        DEBUG_PHP_EXT_ERROR("invalid aerospike object");
    }
}

static void Aerospike_object_free_storage(void *object TSRMLS_DC)
{
    Aerospike_object *intern_obj_p = (Aerospike_object *) object;

    if (intern_obj_p) {
    	zend_object_std_dtor(&intern_obj_p->std TSRMLS_CC);
        efree(intern_obj_p);
//        DEBUG_PHP_EXT_INFO("aerospike zend object destroyed");
    } else {
//        DEBUG_PHP_EXT_ERROR("invalid aerospike object");
        return;
    } 
}

zend_object_value Aerospike_object_new(zend_class_entry *ce TSRMLS_DC)
{
    zend_object_value retval;
    Aerospike_object *intern_obj_p;

    if (NULL != (intern_obj_p = ecalloc(1, sizeof(Aerospike_object)))) {
        zend_object_std_init(&(intern_obj_p->std), ce TSRMLS_CC);
        zend_hash_copy(intern_obj_p->std.properties, &ce->default_properties, (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval *));

        retval.handle = zend_objects_store_put(intern_obj_p, Aerospike_object_dtor, (zend_objects_free_object_storage_t) Aerospike_object_free_storage, NULL TSRMLS_CC);
        retval.handlers = &Aerospike_handlers;
        intern_obj_p->as_p = NULL;
        return (retval);
    } else {
    	DEBUG_PHP_EXT_ERROR("could not allocate memory for aerospike object");
        // Handle return error    
    }
}

/*
 *  Client Object APIs:
 */


/* PHP Method:  aerospike::__construct()
   Constructs a new "aerospike" object. */
PHP_METHOD(Aerospike, __construct)
{
    zval*                  host_p = NULL;
    zval*                  options_p = NULL;
    zval*                  host_arr_p = NULL;
    as_error               error;
    as_config              config;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        error.code = AEROSPIKE_ERR;
        DEBUG_PHP_EXT_ERROR("invalid aerospike object");
        goto exit;
    }

    if (aerospike_obj_p->as_p) {
        error.code = AEROSPIKE_ERR;
		DEBUG_PHP_EXT_ERROR("already created aerospike object");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|a", &host_p, &options_p) == FAILURE) {
        error.code = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("unable to parse parameters for construct in zend");
        goto exit;
    }

    /* configuration */
    as_config_init(&config);

    /* check for hosts */
    if (AEROSPIKE_OK != (aerospike_transform_iteratefor_hostkey(Z_ARRVAL_P(host_p), &host_arr_p))) {
        error.code = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("unable to find host parameter");
        goto exit;
    }

    /* check for name, port */
    if (AEROSPIKE_OK != (aerospike_transform_iteratefor_name_port(Z_ARRVAL_P(host_arr_p), &config))) {
        error.code = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("unable to find name name/port parameter");
        goto exit;
    }

    /* check and set config policies */
    if (AEROSPIKE_OK != (set_general_policies(&config, options_p))) {
        error.code = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("unable to set config read/ write policies");
        goto exit;
    }

    /* initialize the aerospike object */
    aerospike_obj_p->as_p = aerospike_new(&config);

    /* Connect to the cluster */
    if (AEROSPIKE_OK != (aerospike_connect(aerospike_obj_p->as_p, &error))) {
        DEBUG_PHP_EXT_ERROR("unable to make connection");
        goto exit;
    }

    DEBUG_PHP_EXT_INFO("success in creating php-aerospike object")
exit:
    /* config + write + read policy not being destroyed*/
    fprintf(stderr, "error(%d) %s at [%s:%d]\n", error.code, error.message, error.file, error.line);
    RETURN_LONG(error.code);
}

/* PHP Method:  bool Aerospike::__destruct()
   Perform Aerospike object finalization. */
PHP_METHOD(Aerospike, __destruct)
{
    as_status              status = AEROSPIKE_OK;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        DEBUG_PHP_EXT_ERROR("invalid aerospike object");
        goto exit;
    }

    aerospike_destroy(aerospike_obj_p->as_p);
    aerospike_obj_p->as_p = NULL;

    DEBUG_PHP_EXT_INFO("destruct method of aerospike object executed")
exit:
    /*RETURN_LONG(Z_LVAL(class_constant));*/
    RETURN_LONG(status);
}

/* PHP Method:  bool Aerospike::get()
   Read record header(s) and bin(s) for specified key(s) in one batch call. */
PHP_METHOD(Aerospike, get)
{
    as_status              status = AEROSPIKE_OK;
    zval*                  key_record_p = NULL;
    zval*                  record_p = NULL;
    zval*                  options_p = NULL;
    zval*                  bins_p = NULL;
    as_error               err;
    as_key                 as_key_for_get_record;
    int16_t                initializeKey = 0;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        DEBUG_PHP_EXT_ERROR("invalid aerospike object");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "za|aa",
        &key_record_p, &record_p, &bins_p, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("unable to parse php parameters for get function");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                                                                                 &as_key_for_get_record,
                                                                                 &initializeKey))) {
        status = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("unable to parse key parameters for get function ");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_get_record(aerospike_obj_p->as_p,
                                                                 &as_key_for_get_record,
                                                                 options_p,
                                                                 &err,
                                                                 record_p,
                                                                 bins_p))) {
        DEBUG_PHP_EXT_ERROR("get function returned an error");
        goto exit;
    }

exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_get_record);
    }

    RETURN_LONG(status);
}

PHP_METHOD(Aerospike, put)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    zval*                  key_record_p = NULL;
    zval*                  record_p = NULL;
    zval*                  options_p = NULL;
    u_int64_t              ttl_u64;
    as_key                 as_key_for_put_record;
    int16_t                initializeKey = 0;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        DEBUG_PHP_EXT_ERROR("invalid aerospike object");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "aa|la", &key_record_p, &record_p, &ttl_u64, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("unable to parse parameters for put");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p), &as_key_for_put_record, &initializeKey))) {
        status = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("unable to iterate through put key params");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_key_data_put(aerospike_obj_p->as_p, Z_ARRVAL_P(record_p), &as_key_for_put_record, &error, options_p))) {
        status = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("unable to put key data pair into database");
        goto exit;
    }

exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_put_record);
    }
    RETURN_LONG(status);
}

/* PHP Method:  bool Aerospike::isConnected()
   Is the client connected to the Aerospike cluster? */
PHP_METHOD(Aerospike, isConnected)
{
    zval *object = getThis();
    Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

    // DEBUG
    log_info("**In Aerospike::isConnected() method**\n");

    /*** TO BE IMPLEMENTED ***/

    RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::close()
   Disconnect the client to an Aerospike cluster. */
PHP_METHOD(Aerospike, close)
{
    as_status              status = AEROSPIKE_OK;
    as_error               err;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        DEBUG_PHP_EXT_ERROR("invalid aerospike object");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_close(aerospike_obj_p->as_p, &err))) {
        DEBUG_PHP_EXT_ERROR("aerospike close returned error ");
    }

exit:
    RETURN_LONG(status);
}

/* PHP Method:  bool Aerospike::getNodes()
   Return an array of objects for the nodes in the Aerospike cluster. */
PHP_METHOD(Aerospike, getNodes)
{
    zval *object = getThis();
    Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

    // DEBUG
    log_info("**In Aerospike::getNodes() method**\n");

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
    log_info("**In Aerospike::info() method**\n");

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
    log_info("**In Aerospike::add() method**\n");

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
    log_info("**In Aerospike::append() method**\n");

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
    log_info("**In Aerospike::delete() method**\n");

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
    log_info("**In Aerospike::exists() method**\n");

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
    log_info("**In Aerospike::getHeader() method**\n");

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
    log_info("**In Aerospike::operate() method**\n");

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
    log_info("**In Aerospike::prepend() method**\n");

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
    log_info("**In Aerospike::touch() method**\n");

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

PHP_METHOD(Aerospike, setLogLevel)
{
    as_status              status = AEROSPIKE_OK;
    long                   log_level;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        DEBUG_PHP_EXT_ERROR("invalid aerospike object");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &log_level)) {
        status = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("unable to parse parameters for setLogLevel");
        goto exit;
    }

    if (!as_log_set_level(&aerospike_obj_p->as_p->log, log_level)) {
        status = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("unable to set log level");
        goto exit;
    }

exit:
    RETURN_LONG(status);
}

PHP_METHOD(Aerospike, setLogHandler)
{
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;
    uint32_t ret_val = -1;
    is_callback_registered = 0;

    //ret_val = parseLogParameters(&aerospike_obj_p->as_p->log);
    
    /*if (ret_val == 1)
	RETURN_TRUE;
    if(ret_val == 0)
 	RETURN_FALSE;*/
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f*",
                             &func_call_info, &func_call_info_cache,
                             &func_call_info.params, &func_call_info.param_count) == FAILURE) {
        DEBUG_PHP_EXT_ERROR("invalid aerospike object");
        RETURN_FALSE;
    }
	
    if (as_log_set_callback(&aerospike_obj_p->as_p->log, &aerospike_helper_log_callback)) {
	is_callback_registered = 1;
        Z_ADDREF_P(func_call_info.function_name);
        RETURN_TRUE;
    } else {printf("\n*********\n");
        RETURN_FALSE;
    }
}

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
    log_info("**In aerospike minit**\n");

    REGISTER_INI_ENTRIES();

    ZEND_INIT_MODULE_GLOBALS(aerospike, aerospike_globals_ctor, aerospike_globals_dtor);

    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "AerospikeException", NULL);

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

    /* Refer aerospike_policy.h
     * This will expose the policy values for PHP
     * as well as CSDK to PHP client.
     */
    declare_policy_constants_php(Aerospike_ce);
    /* Refer aerospike_status.h
     * This will expose the status code from CSDK
     * to PHP client.
     */
    EXPOSE_LOGGER_CONSTANTS_STR_ZEND(Aerospike_ce);
    EXPOSE_STATUS_CODE_ZEND(Aerospike_ce);
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(aerospike)
{
    log_info("**In aerospike mshutdown**\n");

#ifndef ZTS
    aerospike_globals_dtor(&aerospike_globals TSRMLS_CC);
#endif
    UNREGISTER_INI_ENTRIES();
    return SUCCESS;
}

PHP_RINIT_FUNCTION(aerospike)
{
    log_info("**In aerospike rinit**\n");

    /*** TO BE IMPLEMENTED ***/

    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(aerospike)
{
    log_info("**In aerospike rshutdown**\n");

    /*** TO BE IMPLEMENTED ***/

    return SUCCESS;
}

PHP_MINFO_FUNCTION(aerospike)
{
    log_info("**In aerospike info**\n");

    php_info_print_table_start();
    php_info_print_table_row(2, "aerospike support", "enabled");
    php_info_print_table_row(2, "aerospike version", PHP_AEROSPIKE_VERSION);
    php_info_print_table_end();
}
