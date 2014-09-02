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
 **************************************************************************************
 *  SYNOPSIS
 *    This is the Aerospike PHP Client API Zend Engine extension implementation.
 *
 *    Please see "doc/apiref.md" for detailed information about the API and
 *    "doc/internals.md" for the internal architecture of the client.
 **************************************************************************************
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
#include "aerospike/as_query.h"
#include "aerospike/as_scan.h"

#include <stdbool.h>

#include "aerospike_common.h"
#include "aerospike_status.h"
#include "aerospike_policy.h"
#include "aerospike_logger.h"

/*
 ****************************************************************************
 * A wrapper for the two structs zend_fcall_info and zend_fcall_info_cache
 * that allows for userland function callbacks from within a C-callback
 * context, by having both passed within this struct as a void *udata.
 ****************************************************************************
 */
typedef struct _userland_callback {
    zend_fcall_info *fci_p;
    zend_fcall_info_cache *fcc_p;
} userland_callback;

bool record_stream_callback(const as_val* p_val, void* udata);

/*
 ********************************************************************
 * GLOBAL AEROSPIKE CLASS ENTRY
 * GLOBAL AEROSPIKE OBJECT HANDLERS
 ********************************************************************
 */
static zend_class_entry *Aerospike_ce;
static zend_object_handlers Aerospike_handlers;
/*
 ********************************************************************
 * END OF GLOBAL AEROSPIKE CLASS ENTRY
 * END OF GLOBAL AEROSPIKE OBJECT HANDLERS
 ********************************************************************
 */

static int persist;

PHP_INI_BEGIN()
   STD_PHP_INI_ENTRY("aerospike.nesting_depth", "3", PHP_INI_PERDIR|PHP_INI_SYSTEM, OnUpdateString, nesting_depth, zend_aerospike_globals, aerospike_globals)
   STD_PHP_INI_ENTRY("aerospike.connect_timeout", "1000", PHP_INI_PERDIR|PHP_INI_SYSTEM, OnUpdateString, connect_timeout, zend_aerospike_globals, aerospike_globals)
   STD_PHP_INI_ENTRY("aerospike.read_timeout", "1000", PHP_INI_PERDIR|PHP_INI_SYSTEM, OnUpdateString, read_timeout, zend_aerospike_globals, aerospike_globals)
   STD_PHP_INI_ENTRY("aerospike.write_timeout", "1000", PHP_INI_PERDIR|PHP_INI_SYSTEM, OnUpdateString, write_timeout, zend_aerospike_globals, aerospike_globals)
   STD_PHP_INI_ENTRY("aerospike.log_path", NULL, PHP_INI_PERDIR|PHP_INI_SYSTEM, OnUpdateString, log_path, zend_aerospike_globals, aerospike_globals)
   STD_PHP_INI_ENTRY("aerospike.log_level", NULL, PHP_INI_PERDIR|PHP_INI_SYSTEM, OnUpdateString, log_level, zend_aerospike_globals, aerospike_globals)
   STD_PHP_INI_ENTRY("aerospike.serializer", "4097", PHP_INI_PERDIR|PHP_INI_SYSTEM, OnUpdateString, serializer, zend_aerospike_globals, aerospike_globals)
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

/*
 ********************************************************************
 * Using "arginfo_first_by_ref" in zend_arg_info argument of a
 * zend_function_entry accepts first argument of the
 * corresponding functions by reference and rest by value.
 ********************************************************************
 */
ZEND_BEGIN_ARG_INFO(arginfo_first_by_ref, 0)
    ZEND_ARG_PASS_INFO(1)
ZEND_END_ARG_INFO()

/*
 ********************************************************************
 * Using "arginfo_sec_by_ref" in zend_arg_info argument of a
 * zend_function_entry accepts second argument of the
 * corresponding functions by reference and rest by value.
 ********************************************************************
 */
ZEND_BEGIN_ARG_INFO(arginfo_sec_by_ref, 0)
    ZEND_ARG_PASS_INFO(0)
    ZEND_ARG_PASS_INFO(1)
ZEND_END_ARG_INFO()

/*
 ********************************************************************
 * Using "arginfo_fifth_by_ref" in zend_arg_info argument of a
 * zend_function_entry accepts first argument of the
 * corresponding functions by reference and rest by value.
 ********************************************************************
 */
ZEND_BEGIN_ARG_INFO(arginfo_fifth_by_ref, 0)
    ZEND_ARG_PASS_INFO(0)
    ZEND_ARG_PASS_INFO(0)
    ZEND_ARG_PASS_INFO(0)
    ZEND_ARG_PASS_INFO(0)
    ZEND_ARG_PASS_INFO(1)
ZEND_END_ARG_INFO()

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
 ********************************************************************
 *  The function entries for the main Aerospike class.
 ********************************************************************
 */
static zend_function_entry Aerospike_class_functions[] =
{
    /*
     ********************************************************************
     *  Client Object APIs:
     ********************************************************************
     */
    PHP_ME(Aerospike, __construct, NULL, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, __destruct, NULL, ZEND_ACC_DTOR | ZEND_ACC_PUBLIC)
    /*
     ********************************************************************
     *  Cluster Management APIs:
     ********************************************************************
     */
    PHP_ME(Aerospike, isConnected, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, close, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, getNodes, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, info, NULL, ZEND_ACC_PUBLIC)
    /*
     ********************************************************************
     * Error Handling APIs:
     ********************************************************************
     */
    PHP_ME(Aerospike, error, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, errorno, NULL, ZEND_ACC_PUBLIC)
    /*
     ********************************************************************
     *  Key Value Store (KVS) APIs:
     ********************************************************************
     */
    PHP_ME(Aerospike, add, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, append, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, exists, arginfo_sec_by_ref, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, get, arginfo_sec_by_ref, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, getHeader, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, getMetadata, arginfo_sec_by_ref, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, increment, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, initKey, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, operate, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, prepend, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, put, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, remove, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, removeBin, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, setDeserializer, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Aerospike, setSerializer, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Aerospike, touch, NULL, ZEND_ACC_PUBLIC)
    /*
     ********************************************************************
     *  Logging APIs:
     ********************************************************************
     */
    PHP_ME(Aerospike, setLogLevel, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, setLogHandler, NULL, ZEND_ACC_PUBLIC)
    /*
     ********************************************************************
     * Query and Scan APIs:
     ********************************************************************
     */
    PHP_ME(Aerospike, predicateBetween, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, predicateEquals, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, query, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, scan, NULL, ZEND_ACC_PUBLIC)
    
    /*
     ********************************************************************
     * UDF APIs:
     ********************************************************************
     */
    PHP_ME(Aerospike, register, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, deregister, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, apply, arginfo_fifth_by_ref, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, listRegistered, arginfo_first_by_ref, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, getRegistered, arginfo_sec_by_ref, ZEND_ACC_PUBLIC)
#if 0 // TBD

    // Secondary Index APIs:
    // Large Data Type (LDT) APIs:
    // Shared Memory APIs:

#endif

    { NULL, NULL, NULL }
};

/*
 ********************************************************************
 * Aerospike class destructor
 ********************************************************************
 */
static void Aerospike_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
    Aerospike_object *intern_obj_p = (Aerospike_object *) object;
    as_error error;

    if (intern_obj_p && intern_obj_p->as_ref_p) {
        if (intern_obj_p->as_ref_p->ref_as_p > 1) {
            intern_obj_p->as_ref_p->ref_as_p--;
        } else {
            if (intern_obj_p->as_ref_p->as_p) {
                if (AEROSPIKE_OK != aerospike_close(intern_obj_p->as_ref_p->as_p, &error)) {
                    DEBUG_PHP_EXT_ERROR("Aerospike close returned error");
                }
                aerospike_destroy(intern_obj_p->as_ref_p->as_p);
            }
            intern_obj_p->as_ref_p->ref_as_p = 0;
    	    intern_obj_p->as_ref_p->as_p = NULL;
            if (intern_obj_p->as_ref_p) {
                efree(intern_obj_p->as_ref_p);
            }
            intern_obj_p->as_ref_p = NULL;
        }
        DEBUG_PHP_EXT_INFO("aerospike c sdk object destroyed");
    } else {
        DEBUG_PHP_EXT_ERROR("invalid aerospike object");
    }
}

/*
 ********************************************************************
 * Aerospike module freeing up
 ********************************************************************
 */
static void Aerospike_object_free_storage(void *object TSRMLS_DC)
{
    Aerospike_object *intern_obj_p = (Aerospike_object *) object;

    if (intern_obj_p) {
    	zend_object_std_dtor(&intern_obj_p->std TSRMLS_CC);
        efree(intern_obj_p);
        DEBUG_PHP_EXT_INFO("aerospike zend object destroyed");
    } else {
        DEBUG_PHP_EXT_ERROR("invalid aerospike object");
    }

    return; 
}

/*
 ********************************************************************
 * Aerospike class new method
 ********************************************************************
 */
zend_object_value Aerospike_object_new(zend_class_entry *ce TSRMLS_DC)
{
    zend_object_value retval;
    Aerospike_object *intern_obj_p;

    if (NULL != (intern_obj_p = ecalloc(1, sizeof(Aerospike_object)))) {
        zend_object_std_init(&(intern_obj_p->std), ce TSRMLS_CC);
#if PHP_VERSION_ID < 50399
        zend_hash_copy(intern_obj_p->std.properties, &ce->default_properties, (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval *));
#else
        object_properties_init((zend_object*) &(intern_obj_p->std), ce);
#endif

        retval.handle = zend_objects_store_put(intern_obj_p, Aerospike_object_dtor, (zend_objects_free_object_storage_t) Aerospike_object_free_storage, NULL TSRMLS_CC);
        retval.handlers = &Aerospike_handlers;
        intern_obj_p->as_ref_p = NULL;
        return (retval);
    } else {
    	DEBUG_PHP_EXT_ERROR("Could not allocate memory for aerospike object");
    }
}

/*
 ********************************************************************
 *  Aerospike Client Object APIs:
 ********************************************************************
 */

/*
 ********************************************************************
 *  Lifecycle APIs:
 ********************************************************************
 */
/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::__construct()
 *******************************************************************************************************
 * Constructs a new Aerospike object.
 * Method prototype for PHP userland:
 * public int Aerospike::__construct ( array $config [, string $persistence_alias [, array $options]] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, __construct)
{
    zval*                  config_p = NULL;
    zval*                  options_p = NULL;
    as_error               error;
    as_status              status = AEROSPIKE_OK;
    as_config              config;
    zend_bool              persistent_connection = true;
    HashTable              persistent_list;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;
 
    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    /* initializing the connection flag */
    aerospike_obj_p->is_conn_16 = AEROSPIKE_CONN_STATE_FALSE;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|ba",
                &config_p, &persistent_connection, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for construct in zend");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for construct in zend");
        goto exit;
    }

    aerospike_obj_p->is_persistent = persistent_connection;

    if (PHP_TYPE_ISNOTARR(config_p) || 
        ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) { 
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for construct not proper"); 
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for construct not proper");
        goto exit;
    }

    /* configuration */
    as_config_init(&config);

    /* check for hosts, user and pass within config*/
    if (AEROSPIKE_OK != (aerospike_transform_check_and_set_config(Z_ARRVAL_P(config_p), NULL, &config))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to find host parameter");
        DEBUG_PHP_EXT_ERROR("Unable to find host parameter");
        goto exit;
    }

    /* check and set config policies */
    set_general_policies(&config, options_p, &error);
    if (AEROSPIKE_OK != (error.code)) {
        status = error.code;
        DEBUG_PHP_EXT_ERROR("Unable to set policies");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_helper_object_from_alias_hash(aerospike_obj_p,
                    persistent_connection, &config, persistent_list, persist))){
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to find object from alias");
        DEBUG_PHP_EXT_ERROR("Unable to find object from alias");
        goto exit;
    }

    /* Connect to the cluster */
    if (aerospike_obj_p->as_ref_p && aerospike_obj_p->is_conn_16 == AEROSPIKE_CONN_STATE_FALSE &&
            (AEROSPIKE_OK != (status = aerospike_connect(aerospike_obj_p->as_ref_p->as_p, &error)))) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "Unable to connect to server");
        DEBUG_PHP_EXT_WARNING("Unable to connect to server");
        goto exit;
    }

    /* connection is established, set the connection flag now */
    aerospike_obj_p->is_conn_16 = AEROSPIKE_CONN_STATE_TRUE;

    DEBUG_PHP_EXT_INFO("Success in creating php-aerospike object")
exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    RETURN_LONG(status);
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::__destruct()
 *******************************************************************************************************
 * Perform Aerospike object finalization.
 * Method prototype for PHP userland:
 * public void Aerospike::__destruct ( void )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, __destruct)
{
    as_status              status = AEROSPIKE_OK;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }


    DEBUG_PHP_EXT_INFO("Destruct method of aerospike object executed")
exit:
    RETURN_LONG(status);
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::isConnected()
 *******************************************************************************************************
 * Tests the connection to the Aerospike DB.
 * Method prototype for PHP userland:
 * public boolean Aerospike::isConnected ( void )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, isConnected)
{
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;
    as_error               error;

    if (!(aerospike_obj_p && aerospike_obj_p->as_ref_p && aerospike_obj_p->as_ref_p->as_p)) {
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        RETURN_FALSE;
    }

    if (AEROSPIKE_CONN_STATE_TRUE == aerospike_obj_p->is_conn_16) {
        RETURN_TRUE;
    } else {
        RETURN_FALSE;
    }
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::__close()
 *******************************************************************************************************
 * Close all connections to the Aerospike DB.
 * Method prototype for PHP userland:
 * public void Aerospike::close ( void )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, close)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p || !(aerospike_obj_p->as_ref_p->as_p)) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if (aerospike_obj_p->is_persistent == false) {
        if (AEROSPIKE_OK != (status = aerospike_close(aerospike_obj_p->as_ref_p->as_p, &error))) {
            DEBUG_PHP_EXT_ERROR("Aerospike close returned error");
            PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
        }
        /* Now as connection is getting closed we need to set the connection flag to false */
        aerospike_obj_p->is_conn_16 = AEROSPIKE_CONN_STATE_FALSE;
    } else {
        PHP_EXT_RESET_AS_ERR_IN_CLASS(Aerospike_ce);
    }
   
    /* Now as connection is getting closed we need to set the connection flag to false */
    aerospike_obj_p->is_conn_16 = AEROSPIKE_CONN_STATE_FALSE;

exit:
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 *  Key Value Store (KVS) APIs:
 *******************************************************************************************************
 */

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::get()
 *******************************************************************************************************
 * Gets a record from the Aerospike database.
 * Method prototype for PHP userland:
 * public int Aerospike::get ( array $key, array $record [, array $filter [,array $options]] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, get)
{
    as_status              status = AEROSPIKE_OK;
    zval*                  key_record_p = NULL;
    zval*                  record_p = NULL;
    zval*                  options_p = NULL;
    zval*                  bins_p = NULL;
    as_error               error;
    as_key                 as_key_for_get_record;
    int16_t                initializeKey = 0;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }
 
    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "get: connection not established"); 
        DEBUG_PHP_EXT_ERROR("get: connection not established");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|zz",
        &key_record_p, &record_p, &bins_p, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse php parameters for get function");
        DEBUG_PHP_EXT_ERROR("Unable to parse php parameters for get function.");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p) ||
        ((bins_p) && ((PHP_TYPE_ISNOTARR(bins_p)) && (PHP_TYPE_ISNOTNULL(bins_p)))) ||
        ((options_p) && ((PHP_TYPE_ISNOTARR(options_p)) && (PHP_TYPE_ISNOTNULL(options_p))))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for get function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for get function not proper.");
        goto exit;
    }

    if (bins_p && PHP_TYPE_ISNULL(bins_p)) {
       bins_p = NULL;
    }

    if (options_p && PHP_TYPE_ISNULL(options_p)) {
        options_p = NULL;
    }

    zval_dtor(record_p);
    array_init(record_p);

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                                                                                 &as_key_for_get_record,
                                                                                 &initializeKey))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse key parameters for get function");
        DEBUG_PHP_EXT_ERROR("Unable to parse key parameters for get function ");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_get_record(aerospike_obj_p->as_ref_p->as_p,
                                                                 &as_key_for_get_record,
                                                                 options_p,
                                                                 &error,
                                                                 record_p,
                                                                 bins_p))) {
        DEBUG_PHP_EXT_ERROR("get function returned an error");
        goto exit;
    }

exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_get_record);
    }
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    RETURN_LONG(status);
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::put()
 *******************************************************************************************************
 * Writes a record to the Aerospike database.
 * Method prototype for PHP userland:
 * public int Aerospike::put ( array $key, array $record [, int $ttl = 0 [, array $options ]] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, put)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    zval*                  key_record_p = NULL;
    zval*                  record_p = NULL;
    zval*                  options_p = NULL;
    u_int32_t              ttl_u32 = AS_RECORD_NO_EXPIRE_TTL;
    as_key                 as_key_for_put_record;
    int16_t                initializeKey = 0;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "put: connection not established"); 
        DEBUG_PHP_EXT_ERROR("put: connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "aa|la", &key_record_p, &record_p, &ttl_u32, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for put");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for put");
        goto exit;
    }

    if ((PHP_TYPE_ISNOTARR(key_record_p)) ||
        (PHP_TYPE_ISNOTARR(record_p)) ||
        ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for get function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for put function not proper");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                    &as_key_for_put_record, &initializeKey))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to iterate through put key params");
        DEBUG_PHP_EXT_ERROR("Unable to iterate through put key params");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_key_data_put(aerospike_obj_p->as_ref_p->as_p,
                    &record_p, &as_key_for_put_record, &error, ttl_u32, options_p))) {
        DEBUG_PHP_EXT_ERROR("put function returned an error");
        goto exit;
    }

exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_put_record);
    }
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    RETURN_LONG(status);
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::getNodes()
 *******************************************************************************************************
 * Return an array of objects for the nodes in the Aerospike cluster.
 * Method prototype for PHP userland:
 * public int Aerospike::getNodes ( array $metadata [, array $options ] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, getNodes)
{
    zval *object = getThis();
    Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

    /*** TO BE IMPLEMENTED ***/

    RETURN_TRUE;
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::info()
 *******************************************************************************************************
 * Send an Info. request to an Aerospike cluster.
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, info)
{
    zval *object = getThis();
    Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

    /*** TO BE IMPLEMENTED ***/

    RETURN_TRUE;
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::add()
 * Add integer bin values to existing bin values.
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, add)
{
    zval *object = getThis();
    Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

    /*** TO BE IMPLEMENTED ***/

    RETURN_TRUE;
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::append()
 *******************************************************************************************************
 * Appends a string to the string value in a bin.
 * Method prototype for PHP userland:
 * public int Aerospike::append ( array $key, string $bin, string $value [,array $options ] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, append)
{
    as_status              status = AEROSPIKE_OK;
    zval*                  key_record_p = NULL;
    zval*                  record_p = NULL;
    zval*                  options_p = NULL;
    as_error               error;
    as_key                 as_key_for_get_record;
    int16_t                initializeKey = 0;
    char*                  bin_name_p;
    char*                  append_str_p;
    long                   bin_name_len;
    long                   append_str_len;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "append: connection not established"); 
        DEBUG_PHP_EXT_ERROR("append: connection not established");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zss|a",
        &key_record_p, &bin_name_p, &bin_name_len,
        &append_str_p, &append_str_len, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse php parameters for append function");
        DEBUG_PHP_EXT_ERROR("Unable to parse php parameters for append function");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p) ||
        (!bin_name_p) || (!append_str_p) ||
         ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for append function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for append function not proper.");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                                                                                  &as_key_for_get_record,
                                                                                  &initializeKey))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse php parameters for append function");
        DEBUG_PHP_EXT_ERROR("Unable to parse key parameters for append function");
        goto exit;
    }
    if (AEROSPIKE_OK != (status = aerospike_record_operations_ops(aerospike_obj_p->as_ref_p->as_p,
                                                                  &as_key_for_get_record,
                                                                  options_p,
                                                                  &error,
                                                                  bin_name_p,
                                                                  append_str_p,
                                                                  0,
                                                                  0,
                                                                  0,
                                                                  AS_OPERATOR_APPEND))) {
        DEBUG_PHP_EXT_ERROR("Append function returned an error");
        goto exit;
    }

exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_get_record);
    }
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    RETURN_LONG(status);
}


/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::remove()
 *******************************************************************************************************
 * Removes a record from the Aerospike database
 * Method prototype for PHP userland:
 * public int Aerospike::remove ( array $key [, array $options ] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, remove)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    zval*                  key_record_p = NULL;
    zval*                  options_p = NULL;
    as_key                 as_key_for_put_record;
    int16_t                initializeKey = 0;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }
 
    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "remove: connection not established"); 
        DEBUG_PHP_EXT_ERROR("remove: connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|a", &key_record_p, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for remove");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for remove");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for remove function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for remove function not proper");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p), &as_key_for_put_record, &initializeKey))) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "unable to iterate through remove key params");
        DEBUG_PHP_EXT_ERROR("Unable to iterate through remove key params");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_record_operations_remove(aerospike_obj_p->as_ref_p->as_p, &as_key_for_put_record, &error, options_p))) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to remove record");
        DEBUG_PHP_EXT_ERROR("Unable to remove record");
        goto exit;
    }

exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_put_record);
    }
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    RETURN_LONG(status);
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::exists()
 *******************************************************************************************************
 * Check if a record exists in the Aerospike database.
 * Method prototype for PHP userland:
 * public int Aerospike::exists ( array $key, array $metadata [, array $options] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, exists)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    zval*                  key_record_p = NULL;
    zval*                  metadata_p = NULL;
    zval*                  options_p = NULL;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "exists: connection not established"); 
        DEBUG_PHP_EXT_ERROR("exists: connection not established");
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|z", &key_record_p, &metadata_p, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for exists");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for exists");
        goto exit;
    }

   status = aerospike_php_exists_metadata(aerospike_obj_p->as_ref_p->as_p, key_record_p, metadata_p, options_p, &error);

exit:
    if (status != AEROSPIKE_OK) {
        PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    } else {
        PHP_EXT_RESET_AS_ERR_IN_CLASS(Aerospike_ce);
    }
    RETURN_LONG(status);
}
    
/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::getMetadata
 *******************************************************************************************************
 * Check if a record exists in the Aerospike database.
 * Method prototype for PHP userland:
 * public int Aerospike::getMetadata ( array $key, array $metadata [, array $options ] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, getMetadata)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    zval*                  key_record_p = NULL;
    zval*                  metadata_p = NULL;
    zval*                  options_p = NULL;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;
 
    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "getMetadata: connection not established"); 
        DEBUG_PHP_EXT_ERROR("getMetadata: connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|z", &key_record_p, &metadata_p, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for getMetadata");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for getMetadata");
        goto exit;
    }

    status = aerospike_php_exists_metadata(aerospike_obj_p->as_ref_p->as_p, key_record_p, metadata_p, options_p, &error);

exit:
    if (status != AEROSPIKE_OK) {
        PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    } else {
        PHP_EXT_RESET_AS_ERR_IN_CLASS(Aerospike_ce);
    }
    RETURN_LONG(status);
}


/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::getHeader()
 *******************************************************************************************************
 * Read record generation and expiration for specified key(s) in one batch call.
 * Method prototype for PHP userland:
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, getHeader)
{
    zval *object = getThis();
    Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

    /*** TO BE IMPLEMENTED ***/

    RETURN_TRUE;
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::operate()
 *******************************************************************************************************
 * Perform multiple operations on a single record.
 * Method prototype for PHP userland:
 * public int Aerospike::operate ( array $key, array $operations [, array &$returned ] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, operate)
{
    zval *object = getThis();
    Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

    /*** TO BE IMPLEMENTED ***/

    RETURN_TRUE;
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::prepend()
 *******************************************************************************************************
 * Prepends a string to the string value in a bin.
 * Method prototype for PHP userland:
 * public int Aerospike::prepend ( array $key, string $bin, string $value [,array $options ] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, prepend)
{
    as_status              status = AEROSPIKE_OK;
    zval*                  key_record_p = NULL;
    zval*                  record_p = NULL;
    zval*                  options_p = NULL;
    as_error               error;
    as_key                 as_key_for_get_record;
    int16_t                initializeKey = 0;
    char*                  bin_name_p;
    char*                  prepend_str_p;
    long                   bin_name_len;
    long                   prepend_str_len;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "prepend: connection not established"); 
        DEBUG_PHP_EXT_ERROR("prepend: connection not established");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zss|a",
        &key_record_p, &bin_name_p, &bin_name_len,
        &prepend_str_p, &prepend_str_len, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse php parameters for prepend function");
        DEBUG_PHP_EXT_ERROR("Unable to parse php parameters for prepend function");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p) ||
        (!bin_name_p) || (!prepend_str_p) ||
         ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for prepend function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for prepend function not proper");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                                                                                  &as_key_for_get_record,
                                                                                  &initializeKey))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse key parameters for prepend function");
        DEBUG_PHP_EXT_ERROR("Unable to parse key parameters for prepend function");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_record_operations_ops(aerospike_obj_p->as_ref_p->as_p,
                                                                  &as_key_for_get_record,
                                                                  options_p,
                                                                  &error,
                                                                  bin_name_p,
                                                                  prepend_str_p,
                                                                  0,
                                                                  0,
                                                                  0,
                                                                  AS_OPERATOR_PREPEND))) {
        DEBUG_PHP_EXT_ERROR("Prepend function returned an error");
        goto exit;
    }

exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_get_record);
    }
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    RETURN_LONG(status);
}

/* 
 **************************************************************************************************************************
 * PHP Method:  Aerospike::increment()
 **************************************************************************************************************************
 * Increments a numeric value in a bin.
 * Method prototype for PHP userland:
 * public int Aerospike::increment ( array $key, string $bin, int $offset [, int $initial_value = 0 [, array $options ]] )
 **************************************************************************************************************************
 */
PHP_METHOD(Aerospike, increment)
{
    as_status              status = AEROSPIKE_OK;
    zval*                  key_record_p = NULL;
    zval*                  record_p = NULL;
    zval*                  options_p = NULL;
    as_error               error;
    as_key                 as_key_for_get_record;
    int16_t                initializeKey = 0;
    char*                  bin_name_p;
    int                    bin_name_len;
    long                   offset = 0;
    long                   initial_value = 0;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object ");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "increment: connection not established"); 
        DEBUG_PHP_EXT_ERROR("increment: connection not established");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zsl|la",
        &key_record_p, &bin_name_p, &bin_name_len,
        &offset, &initial_value, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse php parameters for increment function");
        DEBUG_PHP_EXT_ERROR("Unable to parse php parameters for increment function");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p) ||
        (!bin_name_p) ||
        ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for increment function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for increment function not proper");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                                                                                  &as_key_for_get_record,
                                                                                  &initializeKey))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse key parameters for increment function");
        DEBUG_PHP_EXT_ERROR("Unable to parse key parameters for increment function");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_record_operations_ops(aerospike_obj_p->as_ref_p->as_p,
                                                                  &as_key_for_get_record,
                                                                  options_p,
                                                                  &error,
                                                                  bin_name_p,
                                                                  NULL,
                                                                  offset,
                                                                  initial_value,
                                                                  0,
                                                                  AS_OPERATOR_INCR))) {
        DEBUG_PHP_EXT_ERROR("Increment function returned an error");
        goto exit;
    }

exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_get_record);
    }
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    RETURN_LONG(status);
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::touch()
 *******************************************************************************************************
 * Touch a record in the Aerospike DB
 * Method prototype for PHP userland:
 * public int Aerospike::touch ( array $key, int $ttl = 0 [, array $options ] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, touch)
{
    as_status              status = AEROSPIKE_OK;
    zval*                  key_record_p = NULL;
    zval*                  record_p = NULL;
    zval*                  options_p = NULL;
    as_error               error;
    as_key                 as_key_for_get_record;
    int16_t                initializeKey = 0;
    long                   time_to_live;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object ");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "touch: connection not established"); 
        DEBUG_PHP_EXT_ERROR("touch: connection not established");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl|a",
        &key_record_p, &time_to_live, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse php parameters for touch function");
        DEBUG_PHP_EXT_ERROR("Unable to parse php parameters for touch function");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p) ||
         ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for touch function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for touch function not proper");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                                                                                  &as_key_for_get_record,
                                                                                  &initializeKey))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse key parameters for touch function");
        DEBUG_PHP_EXT_ERROR("Unable to parse key parameters for touch function");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_record_operations_ops(aerospike_obj_p->as_ref_p->as_p,
                                                                  &as_key_for_get_record,
                                                                  options_p,
                                                                  &error,
                                                                  NULL,
                                                                  NULL,
                                                                  0,
                                                                  0,
                                                                  time_to_live,
                                                                  AS_OPERATOR_TOUCH))) {
        DEBUG_PHP_EXT_ERROR("Touch function returned an error");
        goto exit;
    }

exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_get_record);
    }
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    RETURN_LONG(status);
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::initKey()
 *******************************************************************************************************
 * Helper method for building the key array
 * Method prototype for PHP userland:
 * public array Aerospike::initKey ( string $ns, string $set, int|string $pk )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, initKey)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    char                   *ns_p = NULL;
    int                    ns_p_length = 0;
    char                   *set_p = NULL;
    int                    set_p_length = 0;
    zval                   *pk_p ;


    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssz", &ns_p, &ns_p_length, &set_p, &set_p_length, &pk_p)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Aerospike::initKey() expects parameter 1-3 to be a non-empty strings");
        DEBUG_PHP_EXT_ERROR("Aerospike::initKey() expects parameter 1-3 to be non-empty strings");
        RETURN_NULL();
    }
    if (ns_p_length == 0 || set_p_length == 0 || PHP_TYPE_ISNULL(pk_p)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Aerospike::initKey() expects parameter 1-3 to be a non-empty strings");
        DEBUG_PHP_EXT_ERROR("Aerospike::initKey() expects parameter 1-3 to be non-empty strings");
        RETURN_NULL();
    }

    array_init(return_value);
    add_assoc_stringl(return_value, "ns", ns_p, ns_p_length, 1);
    add_assoc_stringl(return_value, "set", set_p, set_p_length, 1);

    switch(Z_TYPE_P(pk_p)) {
        case IS_LONG:
            add_assoc_long(return_value, "key", Z_LVAL_P(pk_p));
            break;
        case IS_STRING:
            if (strlen(Z_STRVAL_P(pk_p)) == 0) {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "Aerospike::initKey() expects parameter 1-3 to be a non-empty strings");
                DEBUG_PHP_EXT_ERROR("Aerospike::initKey() expects parameter 1-3 to be non-empty strings");
                RETURN_NULL();
            }
            add_assoc_string(return_value, "key", Z_STRVAL_P(pk_p), 1);
            break;
        default:
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Aerospike::initKey() expects parameter 1-3 to be a non-empty strings");
            DEBUG_PHP_EXT_ERROR("Aerospike::initKey() expects parameter 1-3 to be non-empty strings");
            RETURN_NULL();
    }
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::setDeserializer
 *******************************************************************************************************
 * Sets a php userland callback for deserialization
 * of datatypes which are not supported by aerospike db
 * Method prototype for PHP userland:
 * public static void Aerospike::setDeserializer ( callback $unserialize_cb )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, setDeserializer)
{
    as_status              status = AEROSPIKE_OK;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f*",
                             &user_deserializer_call_info,
                             &user_deserializer_call_info_cache,
                             &user_deserializer_call_info.params,
                             &user_deserializer_call_info.param_count) == FAILURE) {
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for setDeserializer");
        RETURN_FALSE;
    }
	
    is_user_deserializer_registered = 1;
    Z_ADDREF_P(user_deserializer_call_info.function_name);
    RETURN_TRUE;
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::setSerializer()
 *******************************************************************************************************
 * Sets a php userland callback for serialization
 * of datatypes which are not supported by aerospike db
 * Method prototype for PHP userland:
 * public static void Aerospike::setSerializer ( callback $serialize_cb )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, setSerializer)
{
    as_status              status = AEROSPIKE_OK;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f*",
                             &user_serializer_call_info,
                             &user_serializer_call_info_cache,
                             &user_serializer_call_info.params,
                             &user_serializer_call_info.param_count) == FAILURE) {
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for setSerializer");
        RETURN_FALSE;
    }
	
    is_user_serializer_registered = 1;
    Z_ADDREF_P(user_serializer_call_info.function_name);
    RETURN_TRUE;
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::removeBin()
 *******************************************************************************************************
 * Removes a bin from a record.
 * Method prototype for PHP userland:
 * public int Aerospike::removeBin ( array $key, array $bins [, array $options ])
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, removeBin)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    zval*                  key_record_p = NULL;
    zval*                  bins_p;
    zval*                  options_p = NULL;
    as_key                 as_key_for_put_record;
    int16_t                initializeKey = 0;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "removeBin: connection not established"); 
        DEBUG_PHP_EXT_ERROR("removeBin: connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "za|a", &key_record_p, &bins_p, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for removeBin");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for removeBin");
        goto exit;
    }
   
    if (PHP_TYPE_ISNOTARR(key_record_p) || PHP_TYPE_ISNOTARR(bins_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for removeBin function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for removeBin function not proper");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                                                                                  &as_key_for_put_record,
                                                                                  &initializeKey))) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse key parameters for removeBin function");
        DEBUG_PHP_EXT_ERROR("Unable to parse key parameters for removeBin function");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_record_operations_remove_bin(aerospike_obj_p->as_ref_p->as_p, &as_key_for_put_record, bins_p, &error, options_p))) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Unable to remove bin");
        DEBUG_PHP_EXT_ERROR("Unable to remove bin");
        goto exit;
    }
    
exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 *  Scan and Query APIs:
 *******************************************************************************************************
 */

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::predicateEquals()
 *******************************************************************************************************
 * 
 * Method prototype for PHP userland:
 * Helper method for building an equals WHERE predicate.
 * public array Aerospike::predicateEquals ( string $bin, int|string $val )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, predicateEquals)
{
    as_status              status = AEROSPIKE_OK;
    char                   *bin_name_p  =  NULL;
    int                    bin_name_len = 0;
    zval                   *val_p;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &bin_name_p, &bin_name_len, &val_p)) {
        RETURN_NULL();
    }

    if (bin_name_len == 0) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Aerospike::predicateEquals() expects parameter 1 to be a non-empty string.");
        RETURN_NULL();
    }

    if (PHP_TYPE_ISNULL(val_p)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Aerospike::predicateEquals() expects parameter 2 to be a non-empty string or an integer.");
        RETURN_NULL();
    }

    array_init(return_value);
    add_assoc_stringl(return_value, "bin", bin_name_p, bin_name_len, 1);
    add_assoc_stringl(return_value, "op", "=", sizeof("="), 1);

    switch(Z_TYPE_P(val_p)) {
        case IS_LONG:
            add_assoc_long(return_value, "val", Z_LVAL_P(val_p));
            break;
        case IS_STRING:
            if (strlen(Z_STRVAL_P(val_p)) == 0) {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "Aerospike::predicateEquals() expects parameter 2 to be a non-empty string or an integer.");
                RETURN_NULL();
            }
            add_assoc_string(return_value, "val", Z_STRVAL_P(val_p), 1);
            break;
        default:
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Aerospike::predicateEquals() expects parameter 2 to be a non-empty string or an integer.");
            RETURN_NULL();
    }
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::predicateBetween()
 *******************************************************************************************************
 * Helper method for building the between WHERE predicate.
 * Method prototype for PHP userland:
 * public array Aerospike::predicateBetween ( string $bin, int $min, int $max )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, predicateBetween)
{
    as_status              status = AEROSPIKE_OK;
    char                   *bin_name_p  =  NULL;
    int                    bin_name_len = 0;
    long                   min_p;
    long                   max_p;
    zval                   *minmax_arr;

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sll", &bin_name_p, &bin_name_len, &min_p, &max_p)) {
        RETURN_NULL();
    }
    if (bin_name_len == 0) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Aerospike::predicateBetween() expects parameter 1 to be a non-empty string.");
        RETURN_NULL();
    }

    array_init(return_value);
    add_assoc_stringl(return_value, "bin", bin_name_p, bin_name_len, 1);
    add_assoc_stringl(return_value, "op", "BETWEEN", sizeof("BETWEEN"), 1);
    ALLOC_ZVAL(minmax_arr);
    array_init_size(minmax_arr, 2);
    add_next_index_long(minmax_arr, min_p);
    add_next_index_long(minmax_arr, max_p);
    add_assoc_zval(return_value, "val", minmax_arr);
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::query()
 *******************************************************************************************************
 * Queries a secondary index on a set in the Aerospike database.
 * Method prototype for PHP userland:
 * public int Aerospike::query ( string $ns, string $set, array $where, callback
 * $record_cb [, array $bins [, array $options ]] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, query)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    int                    e_level = 0;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;
    char                   *ns_p = NULL;
    int                    ns_p_length = 0;
    char                   *set_p = NULL;
    int                    set_p_length = 0;
    zval                   *predicate_p = NULL;
    char                   *bin_name = NULL;
    zend_fcall_info        fci = empty_fcall_info;
    zend_fcall_info_cache  fcc = empty_fcall_info_cache;
    zval                   *retval_ptr = NULL;
    zval                   *bins_p = NULL;

    /* initialized to 'no error' (status AEROSPIKE_OK, empty message) */
    as_error_init(&error);
    if (!aerospike_obj_p) {
        e_level = E_WARNING;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Aerospike::query() has no valid aerospike object");
        goto exit;
    }
    if (PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        e_level = E_WARNING;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "Aerospike::query() has no connection to the database");
        goto exit;
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssaf|a",
        &ns_p, &ns_p_length, &set_p, &set_p_length, &predicate_p,
        &fci, &fcc, &bins_p) == FAILURE) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Aerospike::query() unable to parse parameters");
        goto exit;
    }
    if (ns_p_length == 0 || set_p_length == 0) {
        e_level = E_WARNING;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Aerospike::query() expects parameter 1 & 2 to be a non-empty strings.");
        goto exit;
    }
    if (PHP_TYPE_ISNOTARR(predicate_p)) {
        e_level = E_WARNING;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Aerospike::query() expects parameter 3 to be an array.");
        goto exit;
    }
    if ((!zend_hash_exists(Z_ARRVAL_P(predicate_p), "bin", sizeof("bin"))) ||
        (!zend_hash_exists(Z_ARRVAL_P(predicate_p), "op", sizeof("op")))  ||
        (!zend_hash_exists(Z_ARRVAL_P(predicate_p), "val", sizeof("val")))) {
        e_level = E_WARNING;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Aerospike::query() expects parameter 3 to include the keys 'bin','op', and 'val'.");
        goto exit;
    }
    zval **op_pp = NULL;
    zval **bin_pp = NULL;
    zval **val_pp = NULL;
    as_query query;
    as_query_init(&query, ns_p, set_p);
    as_query_where_inita(&query, 1);
    if (bins_p) {
        as_query_select_inita(&query, zend_hash_num_elements(Z_ARRVAL_P(bins_p)));
        HashPosition pos;
        zval **data;
        for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(bins_p), &pos);
            zend_hash_get_current_data_ex(Z_ARRVAL_P(bins_p), (void **) &data, &pos) == SUCCESS;
            zend_hash_move_forward_ex(Z_ARRVAL_P(bins_p), &pos)) {
            if (Z_TYPE_PP(data) != IS_STRING) {
                convert_to_string_ex(data);
            }
            as_query_select(&query, Z_STRVAL_PP(data));
        }
    }
    zend_hash_find(Z_ARRVAL_P(predicate_p), "op", sizeof("op"), (void **) &op_pp);
    convert_to_string_ex(op_pp);
    zend_hash_find(Z_ARRVAL_P(predicate_p), "bin", sizeof("bin"), (void **) &bin_pp);
    convert_to_string_ex(bin_pp);
    zend_hash_find(Z_ARRVAL_P(predicate_p), "val", sizeof("val"), (void **) &val_pp);
    if (strncmp(Z_STRVAL_PP(op_pp), "=", 1) == 0) {
        switch(Z_TYPE_PP(val_pp)) {
            case IS_STRING:
                convert_to_string_ex(val_pp);
                as_query_where(&query, Z_STRVAL_PP(bin_pp), string_equals(Z_STRVAL_PP(val_pp)));
                break;
            case IS_LONG:
                convert_to_long_ex(val_pp);
                as_query_where(&query, Z_STRVAL_PP(bin_pp), integer_equals(Z_LVAL_PP(val_pp)));
                break;
            default:
                e_level = E_WARNING;
                PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Aerospike::query() predicate 'val' must be either string or integer.");
                goto exit;
                break;
        }
    } else if (strncmp(Z_STRVAL_PP(op_pp), "BETWEEN", 7) == 0) {
        bool between_unpacked = false;
        if (Z_TYPE_PP(val_pp) == IS_ARRAY) {
            convert_to_array_ex(val_pp);
            zval **min_pp;
            zval **max_pp;
            if ((zend_hash_index_find(Z_ARRVAL_PP(val_pp), 0, (void **) &min_pp) == SUCCESS) &&
                (zend_hash_index_find(Z_ARRVAL_PP(val_pp), 1, (void **) &max_pp) == SUCCESS)) {
                convert_to_long_ex(min_pp);
                convert_to_long_ex(max_pp);
                if (Z_TYPE_PP(min_pp) == IS_LONG && Z_TYPE_PP(max_pp) == IS_LONG) {
                    between_unpacked = true;
                    as_query_where(&query, Z_STRVAL_PP(bin_pp), integer_range(Z_LVAL_PP(min_pp), Z_LVAL_PP(max_pp)));
                }
            }
        }
        if (!between_unpacked) {
            e_level = E_WARNING;
            PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Aerospike::query() the BETWEEN 'op' requires an array of (min,max) integers.");
            goto exit;
        }
    } else {
        e_level = E_WARNING;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Aerospike::query() unsupported 'op' in parameter 3.");
        goto exit;
    }

    userland_callback user_func;
    user_func.fci_p =  &fci;
    user_func.fcc_p = &fcc;
    if (aerospike_query_foreach(aerospike_obj_p->as_ref_p->as_p, &error, NULL, &query, record_stream_callback, &user_func) != AEROSPIKE_OK) {
        e_level = E_WARNING;
        PHP_EXT_SET_AS_ERR(&error, error.code, error.message);
        goto exit;
    }

exit:
    if (e_level > 0) {
        php_error_docref(NULL TSRMLS_CC, e_level, error.message);
    }
    status = error.code;
    if (status != AEROSPIKE_OK) {
        DEBUG_PHP_EXT_ERROR(error.message);
    }
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    as_query_destroy(&query);
    RETURN_LONG(status);
}

/* 
 *******************************************************************************************************
 * Callback for record stream.
 *******************************************************************************************************
 */
bool record_stream_callback(const as_val* p_val, void* udata)
{
    as_error                error;
    userland_callback       *user_func_p;
    zend_fcall_info         *fci_p = NULL;
    zend_fcall_info_cache   *fcc_p = NULL;
    zval                    *record_p = NULL;
    zval                    **args[1];
    zval                    *retval = NULL;
    bool                    do_continue = true;
    foreach_callback_udata  foreach_record_callback_udata;

    if (!p_val) {
        DEBUG_PHP_EXT_INFO("callback is null; stream complete.");
        return true;
    }
    as_record* current_as_rec = as_record_fromval(p_val);
    if (!current_as_rec) {
        DEBUG_PHP_EXT_WARNING("stream returned a non-as_record object to the callback.");
        return true;
    }
    MAKE_STD_ZVAL(record_p);
    array_init(record_p);
    foreach_record_callback_udata.udata_p = record_p;
    foreach_record_callback_udata.error_p = &error;
    if (!as_record_foreach(current_as_rec, (as_rec_foreach_callback) AS_DEFAULT_GET,
        &foreach_record_callback_udata)) {
        DEBUG_PHP_EXT_WARNING("stream callback failed to transform the as_record to an array zval.");
        zval_ptr_dtor(&record_p);
        return true;
    }

    /* call the userland function with the array representing the record */
    user_func_p = (userland_callback *) udata;
    fci_p = user_func_p->fci_p;
    fcc_p = user_func_p->fcc_p;
    args[0] = &record_p;
    fci_p->param_count = 1;
    fci_p->params = args;
    fci_p->retval_ptr_ptr = &retval;
    if (zend_call_function(fci_p, fcc_p TSRMLS_CC) == FAILURE) {
        DEBUG_PHP_EXT_WARNING("stream callback could not invoke the userland function.");
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "stream callback could not invoke userland function.");
        zval_ptr_dtor(&record_p);
        return true;
    }
    zval_ptr_dtor(&record_p);
    if (retval) {
        if ((Z_TYPE_P(retval) == IS_BOOL) && !Z_BVAL_P(retval)) {
                do_continue = false;
        } else {
                do_continue = true;
        }
        zval_ptr_dtor(&retval);
    }
    return do_continue;
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::scan()
 *******************************************************************************************************
 * Scans a set in the Aerospike database.
 * Method prototype for PHP userland:
 * public int Aerospike::scan ( string $ns, string $set, callback $record_cb [,
 * array $bins [, array $options ]] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, scan)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    int                    e_level = 0;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;
    char                   *ns_p = NULL;
    int                    ns_p_length = 0;
    char                   *set_p = NULL;
    int                    set_p_length = 0;
    char                   *bin_name = NULL;
    zend_fcall_info        fci = empty_fcall_info;
    zend_fcall_info_cache  fcc = empty_fcall_info_cache;
    zval                   *retval_ptr = NULL;
    zval                   *bins_p = NULL;

    /* initialized to 'no error' (status AEROSPIKE_OK, empty message) */
    as_error_init(&error);
    if (!aerospike_obj_p) {
        e_level = E_WARNING;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Aerospike::scan() has no valid aerospike object");
        goto exit;
    }
    if (PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        e_level = E_WARNING;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "Aerospike::scan() has no connection to the database");
        goto exit;
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssf|a",
        &ns_p, &ns_p_length, &set_p, &set_p_length,
        &fci, &fcc, &bins_p) == FAILURE) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Aerospike::scan() unable to parse parameters");
        goto exit;
    }
    if (ns_p_length == 0 || set_p_length == 0) {
        e_level = E_WARNING;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Aerospike::scan() expects parameter 1 & 2 to be a non-empty strings.");
        goto exit;
    }
    as_scan scan;
    as_scan_init(&scan, ns_p, set_p);
    if (bins_p) {
        as_scan_select_inita(&scan, zend_hash_num_elements(Z_ARRVAL_P(bins_p)));
        HashPosition pos;
        zval **data;
        for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(bins_p), &pos);
            zend_hash_get_current_data_ex(Z_ARRVAL_P(bins_p), (void **) &data, &pos) == SUCCESS;
            zend_hash_move_forward_ex(Z_ARRVAL_P(bins_p), &pos)) {
            if (Z_TYPE_PP(data) != IS_STRING) {
                convert_to_string_ex(data);
            }
            as_scan_select(&scan, Z_STRVAL_PP(data));
        }
    }

    userland_callback user_func;
    user_func.fci_p =  &fci;
    user_func.fcc_p = &fcc;
    if (aerospike_scan_foreach(aerospike_obj_p->as_ref_p->as_p, &error, NULL, &scan, record_stream_callback, &user_func) != AEROSPIKE_OK) {
        e_level = E_WARNING;
        PHP_EXT_SET_AS_ERR(&error, error.code, error.message);
        goto exit;
    }

exit:
    if (e_level > 0) {
        php_error_docref(NULL TSRMLS_CC, e_level, error.message);
    }
    status = error.code;
    if (status != AEROSPIKE_OK) {
        DEBUG_PHP_EXT_ERROR(error.message);
    }
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    as_scan_destroy(&scan);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 *  User Defined Function (UDF) APIs:
 *******************************************************************************************************
 */

/*
 *******************************************************************************************************
 * PHP Method : Aerospike::register()
 *******************************************************************************************************
 * Registers a UDF module with the Aerospike DB.
 * Method prototype for PHP userland:
 * public int Aerospike::register ( string $path, string $module [, int
 *                                  $language = Aerospike::UDF_TYPE_LUA] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, register)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    char*                  path_p = NULL;
    char*                  module_p = NULL;
    long                   path_len = 0;
    long                   module_len = 0;
    long                   language = UDF_TYPE_LUA;
    zval*                  options_p = NULL;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if (PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER,
                "register: connection not established");
        DEBUG_PHP_EXT_ERROR("register: connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|la",
                &path_p, &path_len, &module_p, &module_len,
                &language, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to parse parameters for register function");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for register function");
        goto exit;
    }

    if (path_len == 0 || module_len == 0) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Expects parameter 1 & 2 to be non-empty strings");
        goto exit;
    }

    if ((options_p) && (PHP_TYPE_ISNOTARR(options_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Input parameters (type) for register function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for reegister function not proper");
    }

    if (AEROSPIKE_OK !=
            (status = aerospike_udf_register(aerospike_obj_p, &error,
                                             path_p, language, options_p))) {
        DEBUG_PHP_EXT_ERROR("register function returned an error");
        goto exit;
    }
exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 * PHP Method : Aerospike::deregister()
 *******************************************************************************************************
 * Removes a UDF module from the Aerospike DB.
 * Method prototype for PHP userland:
 * public int Aerospike::deregister ( string $module )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, deregister)
{
    as_status              status = AEROSPIKE_OK;
    char*                  module_p = NULL;
    long                   module_len = 0;
    long                   language = UDF_TYPE_LUA;
    as_error               error;
    zval*                  options_p = NULL;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT; 

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if (PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER,
                "deregister: connection not established");
        DEBUG_PHP_EXT_ERROR("deregister: connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|la",
                &module_p, &module_len, &language, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to parse parameters for deregister function");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for deregister function");
        goto exit;
    }

    if (module_len == 0) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Expects parameter 1 to be non-empty string");
        goto exit;
    }

    if ((options_p) && (PHP_TYPE_ISNOTARR(options_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Input parameters (type) for deregister function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for deregister function not proper");
    }

    if (AEROSPIKE_OK !=
            (status = aerospike_udf_deregister(aerospike_obj_p, &error,
                                               module_p, module_len,
                                               language, options_p))) {
        DEBUG_PHP_EXT_ERROR("deregister function returned an error");
        goto exit;
    }
exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 * PHP Method : Aerospike::apply()
 *******************************************************************************************************
 * Applies UDF on record in the Aerospike DB.
 * Method prototype for PHP userland:
 * public int Aerospike::apply ( array $key, string $module, string $function[,
 *                               array $args [, mixed &$returned ]] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, apply)
{
    as_status              status = AEROSPIKE_OK;
    char*                  module_p = NULL;
    char*                  function_name_p = NULL;
    zval*                  args_p = NULL;
    zval*                  key_record_p = NULL;
    as_error               error;
    as_key                 as_key_for_apply_udf;
    zval*                  return_value_of_udf_p = NULL;
    zval*                  module_zval_p = NULL;
    zval*                  function_zval_p = NULL;
    long                   module_len = 0;
    long                   function_len = 0;
    int16_t                initializeKey = 0;
    zval*                  options_p = NULL;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if (PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "apply: Connection not established");
        DEBUG_PHP_EXT_ERROR("apply: Connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz|zza",
                &key_record_p, &module_zval_p, &function_zval_p, &args_p,
                &return_value_of_udf_p, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to parse parameters for apply()");
        DEBUG_PHP_EXT_ERROR("Unable to parse the parameters for apply()");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p) || ((args_p) &&
                (PHP_TYPE_ISNOTARR(args_p)) &&
                (PHP_TYPE_ISNOTNULL(args_p))) ||
            (PHP_TYPE_ISNOTSTR(module_zval_p)) ||
            (PHP_TYPE_ISNOTSTR(function_zval_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Input parameters (type) for apply function are not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for apply function are not proper");
        goto exit;
    }

    if (args_p && PHP_TYPE_ISNULL(args_p)) {
        args_p = NULL;
    }
    if (return_value_of_udf_p) {
        zval_dtor(return_value_of_udf_p);
    }
    module_p = Z_STRVAL_P(module_zval_p);
    function_name_p = Z_STRVAL_P(function_zval_p);

    module_len = Z_STRLEN_P(module_zval_p);
    function_len = Z_STRLEN_P(function_zval_p);

    if (module_len == 0 || function_len == 0) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Expects parameter 2 and 3 to be non-empty strings");
        goto exit;
    }

    if ((options_p) && (PHP_TYPE_ISNOTARR(options_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Input parameters (type) for apply function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for apply function not proper");
    }

    if (AEROSPIKE_OK !=
            (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                                                                     &as_key_for_apply_udf,
                                                                     &initializeKey))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to iterate through apply key params");
        DEBUG_PHP_EXT_ERROR("Unable to iterate through apply key params");
        goto exit;
    }

    if (AEROSPIKE_OK !=
            (status = aerospike_udf_apply(aerospike_obj_p, 
                                          &as_key_for_apply_udf, &error,
                                          module_p, function_name_p, &args_p,
                                          return_value_of_udf_p, options_p))) {
        DEBUG_PHP_EXT_ERROR("apply function returned an error");
        goto exit;
    }
exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_apply_udf);
    }
//    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 * PHP Method : Aerospike::listRegistered()
 *******************************************************************************************************
 * Lists the UDF modules registered with the server.
 * Method prototype for PHP userland:
 * public int Aerospike::listRegistered ( array &$modules [, int $language [,
 *                                        array $options ]] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, listRegistered)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    zval*                  array_of_modules_p = NULL;
    long                   language = -1;
    zval*                  options_p = NULL;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if (PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "listRegistered: Connection not established");
        DEBUG_PHP_EXT_ERROR("listRegistered: Connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|lz",
                &array_of_modules_p, &language, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to parse parameters for listRegistered()");
        DEBUG_PHP_EXT_ERROR("Unable to parse the parameters for listRegistered()");
        goto exit;
    }

    if ((options_p) && (PHP_TYPE_ISNOTARR(options_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Input parameters (type) for listRegistered function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for listRegistered function not proper");
    }

    zval_dtor(array_of_modules_p);
    array_init(array_of_modules_p);

    if (AEROSPIKE_OK !=
            (status = aerospike_list_registered_udf_modules(aerospike_obj_p,
                                                            &error,
                                                            array_of_modules_p,
                                                            language,
                                                            options_p))) {
                DEBUG_PHP_EXT_ERROR("listRegistered function returned an error");
                goto exit;
    }
exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 * PHP Method : Aerospike::getRegistered()
 *******************************************************************************************************
 * Get the code for a UDF module registered with the server.
 * Method prototype for PHP userland:
 * public int Aerospike::getRegistered ( string $module, string &$code [, array
 *                                       $options ] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, getRegistered)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    char*                  module_p = NULL;
    long                   module_len = 0;
    long                   language = -1;
    zval*                  udf_code_p = NULL;
    zval*                  options_p = NULL;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if (PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "getRegistered: Connection not established");
        DEBUG_PHP_EXT_ERROR("getRegistered: Connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szl|z",
                &module_p, &module_len, &udf_code_p, &language, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to parse parameters for getRegistered()");
        DEBUG_PHP_EXT_ERROR("Unable to parse the parameters for getRegistered()");
        goto exit;
    }

    if(module_len == 0) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Expects parameter 1 to be non-empty string");
        goto exit;
    }

    if ((options_p) && (PHP_TYPE_ISNOTARR(options_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Input parameters (type) for getRegistered function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for getRegistered function not proper");
    }

    zval_dtor(udf_code_p);
    ZVAL_EMPTY_STRING(udf_code_p);

    if (AEROSPIKE_OK !=
            (status = aerospike_get_registered_udf_module_code(aerospike_obj_p,
                                                               &error, module_p,
                                                               module_len,
                                                               udf_code_p,
                                                               language, 
                                                               options_p))) {
        DEBUG_PHP_EXT_ERROR("getRegistered function returned an error");
        goto exit;
    }
exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    RETURN_LONG(status);
}

/*** TBD ***/

/*
 *******************************************************************************************************
 *  Secondary Index APIs:
 *******************************************************************************************************
 */

/*** TBD ***/

/*
 *******************************************************************************************************
 *  Large Data Type (LDT) APIs:
 *******************************************************************************************************
 */

/*
 *******************************************************************************************************
 *  Logging APIs:
 *******************************************************************************************************
 */

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::setLogLevel()
 *******************************************************************************************************
 * Sets the logging threshold of the Aerospike object.
 * Method prototype for PHP userland:
 * public void Aerospike::setLogLevel ( int $log_level )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, setLogLevel)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    long                   log_level;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "setLogLevel: connection not established"); 
        DEBUG_PHP_EXT_ERROR("setLogLevel: connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &log_level)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for setLogLevel");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for setLogLevel");
        goto exit;
    }

    if (!as_log_set_level(&aerospike_obj_p->as_ref_p->as_p->log, log_level)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to set log level");
        DEBUG_PHP_EXT_ERROR("Unable to set log level");
        goto exit;
    }

exit:
    if (status != AEROSPIKE_OK) {
        PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, &error);
    } else {
        PHP_EXT_RESET_AS_ERR_IN_CLASS(Aerospike_ce);
    }
    RETURN_LONG(status);
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::setLogHandler()
 *******************************************************************************************************
 * Sets a handler for log events.
 * Method prototype for PHP userland:
 * public static void Aerospike::setLogHandler ( callback $log_handler )
 * where callback must follow the signature:
 * public function log_handler ( int $level, string $file, string $function, int $line )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, setLogHandler)
{
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    uint32_t               ret_val = -1;
    is_callback_registered = 0;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        RETURN_FALSE;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "setLogHandler: connection not established"); 
        DEBUG_PHP_EXT_ERROR("setLogHandler: connection not established");
        RETURN_FALSE;
    }

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f*",
                             &func_call_info, &func_call_info_cache,
                             &func_call_info.params, &func_call_info.param_count) == FAILURE) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for setLogHandler");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for setLogHandler");
        RETURN_FALSE;
    }
	
    if (as_log_set_callback(&aerospike_obj_p->as_ref_p->as_p->log, &aerospike_helper_log_callback)) {
	    is_callback_registered = 1;
        Z_ADDREF_P(func_call_info.function_name);
        PHP_EXT_RESET_AS_ERR_IN_CLASS(Aerospike_ce);
        RETURN_TRUE;
    } else {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to set LogHandler");
        DEBUG_PHP_EXT_ERROR("Unable to set LogHandler");
        RETURN_FALSE;
    }
}

/*
 *******************************************************************************************************
 * Error handling APIs:
 *******************************************************************************************************
 */

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::error()
 *******************************************************************************************************
 * Display an error message associated with the last operation
 * Method prototype for PHP userland:
 * public string Aerospike::error ( void )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, error)
{
    char *error_msg = Z_STRVAL_P(zend_read_property(Aerospike_ce, getThis(), "error", strlen("error"), 1 TSRMLS_CC));
    RETURN_STRINGL(error_msg, strlen(error_msg), 1);
}

/* 
 *******************************************************************************************************
 * PHP Method:  Aerospike::errorno()
 *******************************************************************************************************
 * Display an error code associated with the last operation.
 * Method prototype for PHP userland:
 * public int Aerospike::errorno ( void )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, errorno)
{
    int error_code = Z_LVAL_P(zend_read_property(Aerospike_ce, getThis(), "errorno", strlen("errorno"), 1 TSRMLS_CC));
    RETURN_LONG(error_code);
}

/*
 *******************************************************************************************************
 *  Shared Memory APIs:
 *******************************************************************************************************
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

/*
 ********************************************************************
 * Aerospike module init.
 ********************************************************************
 */
PHP_MINIT_FUNCTION(aerospike)
{
    REGISTER_INI_ENTRIES();

    ZEND_INIT_MODULE_GLOBALS(aerospike, aerospike_globals_ctor, aerospike_globals_dtor);

    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Aerospike", Aerospike_class_functions);

    if (!(Aerospike_ce = zend_register_internal_class(&ce TSRMLS_CC))) {
        return FAILURE;
    }

    if(!(persist = zend_register_list_destructors_ex(NULL, NULL, "Persistent resource",  module_number))){
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
    zend_declare_property_long(Aerospike_ce, "errorno", strlen("errorno"), DEFAULT_ERRORNO, ZEND_ACC_PRIVATE TSRMLS_DC);
    zend_declare_property_string(Aerospike_ce, "error", strlen("error"), DEFAULT_ERROR, ZEND_ACC_PRIVATE TSRMLS_DC);

    EXPOSE_LOGGER_CONSTANTS_STR_ZEND(Aerospike_ce);
    EXPOSE_STATUS_CODE_ZEND(Aerospike_ce);
    // Query predicate operators
    zend_declare_class_constant_string(Aerospike_ce, "OP_EQ", sizeof("OP_EQ") - 1, "=" TSRMLS_CC);
    zend_declare_class_constant_string(Aerospike_ce, "OP_BETWEEN", sizeof("OP_BETWEEN") - 1, "BETWEEN" TSRMLS_CC);
    return SUCCESS;
}

/*
 ********************************************************************
 * Aerospike module shutdown.
 ********************************************************************
 */
PHP_MSHUTDOWN_FUNCTION(aerospike)
{
#ifndef ZTS
    aerospike_globals_dtor(&aerospike_globals TSRMLS_CC);
#endif
    UNREGISTER_INI_ENTRIES();
    return SUCCESS;
}

/*
 ********************************************************************
 * Aerospike request init.
 ********************************************************************
 */
PHP_RINIT_FUNCTION(aerospike)
{
    /*** TO BE IMPLEMENTED ***/

    return SUCCESS;
}

/*
 ********************************************************************
 * Aerospike request shutdown.
 ********************************************************************
 */
PHP_RSHUTDOWN_FUNCTION(aerospike)
{
    /*** TO BE IMPLEMENTED ***/

    return SUCCESS;
}

/*
 ********************************************************************
 * Aerospike module info.
 ********************************************************************
 */
PHP_MINFO_FUNCTION(aerospike)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "aerospike support", "enabled");
    php_info_print_table_row(2, "aerospike version", PHP_AEROSPIKE_VERSION);
    php_info_print_table_end();
}

