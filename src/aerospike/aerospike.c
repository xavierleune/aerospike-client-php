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

/*-----------------------------------------------------------*/
typedef struct Aerospike_object {
    zend_object std;
    int value;
    aerospike *as_p;
    u_int16_t is_conn_16;
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
    PHP_ME(Aerospike, exists, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, get, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, getHeader, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, getMetadata, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, increment, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, initKey, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, operate, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, prepend, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, put, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, remove, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, removeBin, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, touch, NULL, ZEND_ACC_PUBLIC)
    /*
     *  Logging APIs:
     */
    PHP_ME(Aerospike, setLogLevel, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, setLogHandler, NULL, ZEND_ACC_PUBLIC)
    /*
     * Query and Scan APIs:
     */
    PHP_ME(Aerospike, predicateBetween, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, predicateEquals, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, query, NULL, ZEND_ACC_PUBLIC)
    /*
     * Error Handling APIs:
     */
    PHP_ME(Aerospike, error, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, errorno, NULL, ZEND_ACC_PUBLIC)
#if 0 // TBD

    // Secondary Index APIs:
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
        DEBUG_PHP_EXT_INFO("aerospike zend object destroyed");
    } else {
        DEBUG_PHP_EXT_ERROR("invalid aerospike object");
    }

    return; 
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
    	DEBUG_PHP_EXT_ERROR("Could not allocate memory for aerospike object");
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
    int8_t*                persistence_alias_p = NULL;
    int16_t                persistence_alias_len = 0;
    as_error               error;
    as_status              status = AEROSPIKE_OK;
    as_config              config;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;
 
    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    /* initializing the connection flag */
    aerospike_obj_p->is_conn_16 = AEROSPIKE_CONN_STATE_FALSE;

    if (aerospike_obj_p->as_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Already created aerospike object");
        DEBUG_PHP_EXT_ERROR("Already created aerospike object");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|sa",
                &host_p, &persistence_alias_p, &persistence_alias_len, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for construct in zend");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for construct in zend");
        goto exit;
    }

    /*
     * TODO: persistence_alias is yet to be handled for cluster initialization
     */

    if (PHP_TYPE_ISNOTARR(host_p) || 
        ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for construct not proper"); 
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for construct not proper");
        goto exit;
    }

    /* configuration */
    as_config_init(&config);

    /* check for hosts */
    if (AEROSPIKE_OK != (aerospike_transform_iteratefor_hostkey(Z_ARRVAL_P(host_p), &host_arr_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to find host parameter");
        DEBUG_PHP_EXT_ERROR("Unable to find host parameter");
        goto exit;
    }

    /* check for name, port */
    if (AEROSPIKE_OK != (status = aerospike_transform_iteratefor_name_port(Z_ARRVAL_P(host_arr_p), &config))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to find name addr/port parameter");
        DEBUG_PHP_EXT_ERROR("Unable to find name addr/port parameter");
        goto exit;
    }

    /* check and set config policies */
    if (AEROSPIKE_OK != (set_general_policies(&config, options_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to set config read/ write policies");
        DEBUG_PHP_EXT_ERROR("Unable to set config read/ write policies");
        goto exit;
    }

    /* initialize the aerospike object */
    aerospike_obj_p->as_p = aerospike_new(&config);

    /* Connect to the cluster */
    if (AEROSPIKE_OK != (status = aerospike_connect(aerospike_obj_p->as_p, &error))) {
        DEBUG_PHP_EXT_ERROR("Unable to make connection");
        goto exit;
    }

    /* connection is established, set the connection flag now */
    aerospike_obj_p->is_conn_16 = AEROSPIKE_CONN_STATE_TRUE;

    DEBUG_PHP_EXT_INFO("Success in creating php-aerospike object")
exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
    RETURN_LONG(status);
}

/* PHP Method:  bool Aerospike::__destruct()
   Perform Aerospike object finalization. */
PHP_METHOD(Aerospike, __destruct)
{
    as_status              status = AEROSPIKE_OK;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    aerospike_destroy(aerospike_obj_p->as_p);
    aerospike_obj_p->as_p = NULL;

    DEBUG_PHP_EXT_INFO("Destruct method of aerospike object executed")
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
    as_error               error;
    as_key                 as_key_for_get_record;
    int16_t                initializeKey = 0;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }
 
    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_CLUSTER, "get: connection not established"); 
        DEBUG_PHP_EXT_ERROR("get: connection not established");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|aa",
        &key_record_p, &record_p, &bins_p, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse php parameters for get function");
        DEBUG_PHP_EXT_ERROR("Unable to parse php parameters for get function.");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p) || 
        ((bins_p) && (PHP_TYPE_ISNOTARR(bins_p))) ||
        ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for get function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for get function not proper.");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(record_p)) {
        zval*         record_arr_p = NULL;

        MAKE_STD_ZVAL(record_arr_p);
        array_init(record_arr_p);
        ZVAL_ZVAL(record_p, record_arr_p, 1, 1);
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                                                                                 &as_key_for_get_record,
                                                                                 &initializeKey))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse key parameters for get function");
        DEBUG_PHP_EXT_ERROR("Unable to parse key parameters for get function ");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_get_record(aerospike_obj_p->as_p,
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
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
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
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_CLUSTER, "put: connection not established"); 
        DEBUG_PHP_EXT_ERROR("put: connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "aa|la", &key_record_p, &record_p, &ttl_u64, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for put");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for put");
        goto exit;
    }

    if ((PHP_TYPE_ISNOTARR(key_record_p)) ||
        (PHP_TYPE_ISNOTARR(record_p)) ||
        ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for get function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for put function not proper");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p), &as_key_for_put_record, &initializeKey))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to iterate through put key params");
        DEBUG_PHP_EXT_ERROR("Unable to iterate through put key params");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_key_data_put(aerospike_obj_p->as_p, &record_p, &as_key_for_put_record, &error, options_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to put key data pair into database");
        DEBUG_PHP_EXT_ERROR("Unable to put key data pair into database");
        goto exit;
    }

exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_put_record);
    }
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
    RETURN_LONG(status);
}

/* PHP Method:  bool Aerospike::isConnected()
   Is the client connected to the Aerospike cluster? */
PHP_METHOD(Aerospike, isConnected)
{
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;
    as_error               error;

    if ((!aerospike_obj_p) || !(aerospike_obj_p->as_p)) {
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Invalid aerospike object");
        PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        RETURN_FALSE;
    }

   if (AEROSPIKE_CONN_STATE_TRUE == aerospike_obj_p->is_conn_16) {
       PHP_EXT_RESET_AS_ERR_IN_CLASS(Aerospike_ce);
       RETURN_TRUE;
   } else {
       PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
       RETURN_FALSE;
   }
}

/* PHP Method:  bool Aerospike::close()
   Disconnect the client to an Aerospike cluster. */
PHP_METHOD(Aerospike, close)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p || !(aerospike_obj_p->as_p)) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_close(aerospike_obj_p->as_p, &error))) {
        DEBUG_PHP_EXT_ERROR("Aerospike close returned error");
    }
   
    /* Now as connection is getting closed we need to set the connection flag to false */
    aerospike_obj_p->is_conn_16 = AEROSPIKE_CONN_STATE_FALSE;

exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
    RETURN_LONG(status);
}

/* PHP Method:  bool Aerospike::getNodes()
   Return an array of objects for the nodes in the Aerospike cluster. */
PHP_METHOD(Aerospike, getNodes)
{
    zval *object = getThis();
    Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

    /*** TO BE IMPLEMENTED ***/

    RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::info()
   Send an Info. request to an Aerospike cluster. */
PHP_METHOD(Aerospike, info)
{
    zval *object = getThis();
    Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

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

    /*** TO BE IMPLEMENTED ***/

    RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::append()
 *    Append bin string values to existing record bin values. */
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
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_CLUSTER, "append: connection not established"); 
        DEBUG_PHP_EXT_ERROR("append: connection not established");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zss|a",
        &key_record_p, &bin_name_p, &bin_name_len,
        &append_str_p, &append_str_len, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse php parameters for append function");
        DEBUG_PHP_EXT_ERROR("Unable to parse php parameters for append function");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p) ||
        (!bin_name_p) || (!append_str_p) ||
         ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for append function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for append function not proper.");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                                                                                  &as_key_for_get_record,
                                                                                  &initializeKey))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse php parameters for append function");
        DEBUG_PHP_EXT_ERROR("Unable to parse key parameters for append function");
        goto exit;
    }
    if (AEROSPIKE_OK != (status = aerospike_record_operations_ops(aerospike_obj_p->as_p,
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
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
    RETURN_LONG(status);
}


/* PHP Method:  bool Aerospike::remove()
   Delete record for specified key. */
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
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }
 
    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_CLUSTER, "remove: connection not established"); 
        DEBUG_PHP_EXT_ERROR("remove: connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|a", &key_record_p, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for remove");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for remove");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for remove function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for remove function not proper");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p), &as_key_for_put_record, &initializeKey))) {
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "unable to iterate through remove key params");
        DEBUG_PHP_EXT_ERROR("Unable to iterate through remove key params");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_record_operations_remove(aerospike_obj_p->as_p, &as_key_for_put_record, &error, options_p))) {
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to remove record");
        DEBUG_PHP_EXT_ERROR("Unable to remove record");
        goto exit;
    }

exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_put_record);
    }
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
    RETURN_LONG(status);
}

/* PHP Method:  bool Aerospike::exists()
   Check if record key(s) exist in one batch call. */
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
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_CLUSTER, "exists: connection not established"); 
        DEBUG_PHP_EXT_ERROR("exists: connection not established");
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|z", &key_record_p, &metadata_p, options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for exists");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for exists");
        goto exit;
    }

   status = aerospike_php_exists_metadata(aerospike_obj_p->as_p, key_record_p, metadata_p, options_p, &error);

exit:
    if (status != AEROSPIKE_OK) {
        PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
    } else {
        PHP_EXT_RESET_AS_ERR_IN_CLASS(Aerospike_ce);
    }
    RETURN_LONG(status);
}
    
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
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_CLUSTER, "getMetadata: connection not established"); 
        DEBUG_PHP_EXT_ERROR("getMetadata: connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|z", &key_record_p, &metadata_p, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for getMetadata");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for getMetadata");
        goto exit;
    }

    status = aerospike_php_exists_metadata(aerospike_obj_p->as_p, key_record_p, metadata_p, options_p, &error);

exit:
    if (status != AEROSPIKE_OK) {
        PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
    } else {
        PHP_EXT_RESET_AS_ERR_IN_CLASS(Aerospike_ce);
    }
    RETURN_LONG(status);
}


/* PHP Method:  bool Aerospike::getHeader()
   Read record generation and expiration for specified key(s) in one batch call. */
PHP_METHOD(Aerospike, getHeader)
{
    zval *object = getThis();
    Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

    /*** TO BE IMPLEMENTED ***/

    RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::operate()
   Perform multiple read/write operations on a single key in one batch call. */
PHP_METHOD(Aerospike, operate)
{
    zval *object = getThis();
    Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

    /*** TO BE IMPLEMENTED ***/

    RETURN_TRUE;
}

/* PHP Method:  bool Aerospike::prepend()
 *    Prepend bin string values to existing record bin values. */
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
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_CLUSTER, "prepend: connection not established"); 
        DEBUG_PHP_EXT_ERROR("prepend: connection not established");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zss|a",
        &key_record_p, &bin_name_p, &bin_name_len,
        &prepend_str_p, &prepend_str_len, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse php parameters for prepend function");
        DEBUG_PHP_EXT_ERROR("Unable to parse php parameters for prepend function");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p) ||
        (!bin_name_p) || (!prepend_str_p) ||
         ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for prepend function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for prepend function not proper");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                                                                                  &as_key_for_get_record,
                                                                                  &initializeKey))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse key parameters for prepend function");
        DEBUG_PHP_EXT_ERROR("Unable to parse key parameters for prepend function");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_record_operations_ops(aerospike_obj_p->as_p,
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
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
    RETURN_LONG(status);
}

/* PHP Method:  bool Aerospike::increment()
 *  *    Create record if it does not already exist. */
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
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object ");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_CLUSTER, "increment: connection not established"); 
        DEBUG_PHP_EXT_ERROR("increment: connection not established");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zsl|la",
        &key_record_p, &bin_name_p, &bin_name_len,
        &offset, &initial_value, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse php parameters for increment function");
        DEBUG_PHP_EXT_ERROR("Unable to parse php parameters for increment function");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p) ||
        (!bin_name_p) ||
        ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for increment function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for increment function not proper");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                                                                                  &as_key_for_get_record,
                                                                                  &initializeKey))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse key parameters for increment function");
        DEBUG_PHP_EXT_ERROR("Unable to parse key parameters for increment function");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_record_operations_ops(aerospike_obj_p->as_p,
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
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
    RETURN_LONG(status);
}

/* PHP Method:  bool Aerospike::touch()
 *    Create record if it does not already exist. */
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
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object ");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_CLUSTER, "touch: connection not established"); 
        DEBUG_PHP_EXT_ERROR("touch: connection not established");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl|a",
        &key_record_p, &time_to_live, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse php parameters for touch function");
        DEBUG_PHP_EXT_ERROR("Unable to parse php parameters for touch function");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p) ||
         ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for touch function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for touch function not proper");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                                                                                  &as_key_for_get_record,
                                                                                  &initializeKey))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse key parameters for touch function");
        DEBUG_PHP_EXT_ERROR("Unable to parse key parameters for touch function");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_record_operations_ops(aerospike_obj_p->as_p,
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
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
    RETURN_LONG(status);
}

/* PHP Method:  bool Aerospike::initKey()
   helper method for building the key array. */
PHP_METHOD(Aerospike, initKey)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    char                   *ns_p = NULL;
    int                    ns_p_length = 0;
    char                   *set_p = NULL;
    int                    set_p_length = 0;
    zval                   *pk_p ;

    array_init(return_value);

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssz", &ns_p, &ns_p_length, &set_p, &set_p_length, &pk_p)) {
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Aerospike::initKey() expects parameter 1-3 to be non-empty strings");
        PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
        DEBUG_PHP_EXT_ERROR("Aerospike::initKey() expects parameter 1-3 to be non-empty strings");
        RETURN_NULL();
    }
    if (ns_p_length == 0 || set_p_length == 0 || PHP_TYPE_ISNULL(pk_p)) {
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Aerospike::initKey() expects parameter 1-3 to be non-empty strings");
        PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
        DEBUG_PHP_EXT_ERROR("Aerospike::initKey() expects parameter 1-3 to be non-empty strings");
        RETURN_NULL();
    }

    add_assoc_stringl(return_value, "ns", ns_p, ns_p_length, 1);
    add_assoc_stringl(return_value, "set", set_p, set_p_length, 1);

    switch(Z_TYPE_P(pk_p)) {
        case IS_LONG:
            add_assoc_long(return_value, "key", Z_LVAL_P(pk_p));
            break;
        case IS_STRING:
            if (strlen(Z_STRVAL_P(pk_p)) == 0) {
                PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Aerospike::initKey() expects parameter 1-3 to be non-empty strings");
                PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
                DEBUG_PHP_EXT_ERROR("Aerospike::initKey() expects parameter 1-3 to be non-empty strings");
                RETURN_NULL();
            }
            add_assoc_string(return_value, "key", Z_STRVAL_P(pk_p), 1);
            break;
        default:
            PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Aerospike::initKey() expects parameter 1-3 to be non-empty strings");
            PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
            DEBUG_PHP_EXT_ERROR("Aerospike::initKey() expects parameter 1-3 to be non-empty strings");
            RETURN_NULL();
    }
    PHP_EXT_RESET_AS_ERR_IN_CLASS(Aerospike_ce);
}

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
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_CLUSTER, "removeBin: connection not established"); 
        DEBUG_PHP_EXT_ERROR("removeBin: connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "za|a", &key_record_p, &bins_p, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for removeBin");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for removeBin");
        goto exit;
    }
   
    if (PHP_TYPE_ISNOTARR(key_record_p) || PHP_TYPE_ISNOTARR(bins_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for removeBin function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for removeBin function not proper");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                                                                                  &as_key_for_put_record,
                                                                                  &initializeKey))) {
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse key parameters for removeBin function");
        DEBUG_PHP_EXT_ERROR("Unable to parse key parameters for removeBin function");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_record_operations_remove_bin(aerospike_obj_p->as_p, &as_key_for_put_record, bins_p, &error, options_p))) {
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Unable to remove bin");
        DEBUG_PHP_EXT_ERROR("Unable to remove bin");
        goto exit;
    }
    
exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
    RETURN_LONG(status);
}

/*
 *  Scan and Query APIs:
 */

/* PHP Method: array Aerospike::predicateEquals ( string $bin, int|string $val )
   helper method for building the EQUALS predicate. */
PHP_METHOD(Aerospike, predicateEquals)
{
    as_status              status = AEROSPIKE_OK;
    char                   *bin_name_p  =  NULL;
    int                    bin_name_len = 0;
    zval                   *val_p;

    array_init(return_value);

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &bin_name_p, &bin_name_len, &val_p)) {
        RETURN_NULL();
    }
    if (bin_name_len == 0) {
        zend_error(E_WARNING, "Aerospike::predicateEquals() expects parameter 1 to be a non-empty string.");
        RETURN_NULL();
    }
    if (PHP_TYPE_ISNULL(val_p)) {
        zend_error(E_WARNING, "Aerospike::predicateEquals() expects parameter 2 to be a non-empty string or an integer.");
        RETURN_NULL();
    }

    add_assoc_stringl(return_value, "bin", bin_name_p, bin_name_len, 1);
    add_assoc_stringl(return_value, "op", "=", sizeof("="), 1);

    switch(Z_TYPE_P(val_p)) {
        case IS_LONG:
            add_assoc_long(return_value, "val", Z_LVAL_P(val_p));
            break;
        case IS_STRING:
            if (strlen(Z_STRVAL_P(val_p)) == 0) {
                zend_error(E_WARNING, "Aerospike::predicateEquals() expects parameter 2 to be a non-empty string or an integer.");
                RETURN_NULL();
            }
            add_assoc_string(return_value, "val", Z_STRVAL_P(val_p), 1);
            break;
        default:
            zend_error(E_WARNING, "Aerospike::predicateEquals() expects parameter 2 to be a non-empty string or an integer.");
            RETURN_NULL();
    }
}

/* PHP Method: array Aerospike::predicateBetween ( string $bin, int $min, int $max )
   helper method for building the BETWEEN predicate. */
PHP_METHOD(Aerospike, predicateBetween)
{
    as_status              status = AEROSPIKE_OK;
    char                   *bin_name_p  =  NULL;
    int                    bin_name_len = 0;
    long                   *min_p;
    long                   *max_p;
    zval                   *minmax_arr;

    array_init(return_value);

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sll", &bin_name_p, &bin_name_len, &min_p, &max_p)) {
        RETURN_NULL();
    }
    if (bin_name_len == 0) {
        zend_error(E_WARNING, "Aerospike::predicateBetween() expects parameter 1 to be a non-empty string.");
        RETURN_NULL();
    }

    add_assoc_stringl(return_value, "bin", bin_name_p, bin_name_len, 1);
    add_assoc_stringl(return_value, "op", "BETWEEN", sizeof("BETWEEN"), 1);
    array_init_size(minmax_arr, 2);
    add_next_index_long(minmax_arr, min_p);
    add_next_index_long(minmax_arr, max_p);
    add_assoc_zval(return_value, "val", minmax_arr);
}

PHP_METHOD(Aerospike, query)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;
    char                   *ns_p = NULL;
    int                    ns_p_length = 0;
    char                   *set_p = NULL;
    int                    set_p_length = 0;
    zval*                  predicate_p = NULL;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }
    if (PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_CLUSTER, "get: connection not established"); 
        DEBUG_PHP_EXT_ERROR("get: connection not established");
        goto exit;
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssa",
        &ns_p, &ns_p_length, &set_p, &set_p_length, &predicate_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse php parameters for query()");
        goto exit;
    }
    if (ns_p_length == 0 || set_p_length == 0) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Aerospike::query() expects parameter 1 & 2 to be a non-empty strings.");
        zend_error(E_WARNING, "Aerospike::query() expects parameter 1 & 2 to be a non-empty strings.");
        goto exit;
    }
    if (PHP_TYPE_ISNOTARR(predicate_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Aerospike::query() expects parameter 3 to be an array.");
        zend_error(E_WARNING, "Aerospike::query() expects parameter 3 to be an array.");
        goto exit;
    }

exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
    RETURN_LONG(status);
}

/*** TBD ***/

/*
 *  Secondary Index APIs:
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
    as_error               error;
    long                   log_level;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_CLUSTER, "setLogLevel: connection not established"); 
        DEBUG_PHP_EXT_ERROR("setLogLevel: connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &log_level)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for setLogLevel");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for setLogLevel");
        goto exit;
    }

    if (!as_log_set_level(&aerospike_obj_p->as_p->log, log_level)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to set log level");
        DEBUG_PHP_EXT_ERROR("Unable to set log level");
        goto exit;
    }

exit:
    if (status != AEROSPIKE_OK) {
        PHP_EXT_SET_AS_ERR_IN_CLASS(Aerospike_ce, error);
    } else {
        PHP_EXT_RESET_AS_ERR_IN_CLASS(Aerospike_ce);
    }
    RETURN_LONG(status);
}

PHP_METHOD(Aerospike, setLogHandler)
{
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    uint32_t ret_val = -1;
    is_callback_registered = 0;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        RETURN_FALSE;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_CLUSTER, "setLogHandler: connection not established"); 
        DEBUG_PHP_EXT_ERROR("setLogHandler: connection not established");
        RETURN_FALSE;
    }

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f*",
                             &func_call_info, &func_call_info_cache,
                             &func_call_info.params, &func_call_info.param_count) == FAILURE) {
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for setLogHandler");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for setLogHandler");
        RETURN_FALSE;
    }
	
    if (as_log_set_callback(&aerospike_obj_p->as_p->log, &aerospike_helper_log_callback)) {
	is_callback_registered = 1;
        Z_ADDREF_P(func_call_info.function_name);
        PHP_EXT_RESET_AS_ERR_IN_CLASS(Aerospike_ce);
        RETURN_TRUE;
    } else {
        PHP_EXT_SET_AS_ERR(error, AEROSPIKE_ERR_PARAM, "Unable to set LogHandler");
        DEBUG_PHP_EXT_ERROR("Unable to set LogHandler");
        RETURN_FALSE;
    }
}

/*
 * Error handling APIs:
 */

/* PHP Method: string Aerospike::error
   Return latest error message */
PHP_METHOD(Aerospike, error)
{
    char *error_msg = Z_STRVAL_P(zend_read_property(Aerospike_ce, getThis(), "error", strlen("error"), 1 TSRMLS_CC));
    RETURN_STRINGL(error_msg, strlen(error_msg), 1);
}

/* PHP Method: string Aerospike::errorno
   Return latest error number */
PHP_METHOD(Aerospike, errorno)
{
    int error_code = Z_LVAL_P(zend_read_property(Aerospike_ce, getThis(), "errorno", strlen("errorno"), 1 TSRMLS_CC));
    RETURN_LONG(error_code);
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
    DEBUG_PHP_EXT_INFO("In aerospike minit");

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
    zend_declare_property_long(Aerospike_ce, "errorno", strlen("errorno"), DEFAULT_ERRORNO, ZEND_ACC_PRIVATE TSRMLS_DC);
    zend_declare_property_string(Aerospike_ce, "error", strlen("error"), DEFAULT_ERROR, ZEND_ACC_PRIVATE TSRMLS_DC);

    EXPOSE_LOGGER_CONSTANTS_STR_ZEND(Aerospike_ce);
    EXPOSE_STATUS_CODE_ZEND(Aerospike_ce);
    // Query predicate operators
    zend_declare_class_constant_string(Aerospike_ce, "OP_EQ", sizeof("OP_EQ") - 1, "=" TSRMLS_CC);
    zend_declare_class_constant_string(Aerospike_ce, "OP_BETWEEN", sizeof("OP_BETWEEN") - 1, "BETWEEN" TSRMLS_CC);
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(aerospike)
{
    DEBUG_PHP_EXT_INFO("In aerospike mshutdown");

#ifndef ZTS
    aerospike_globals_dtor(&aerospike_globals TSRMLS_CC);
#endif
    UNREGISTER_INI_ENTRIES();
    return SUCCESS;
}

PHP_RINIT_FUNCTION(aerospike)
{
    DEBUG_PHP_EXT_INFO("In aerospike rinit");

    /*** TO BE IMPLEMENTED ***/

    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(aerospike)
{
    DEBUG_PHP_EXT_INFO("In aerospike rshutdown");

    /*** TO BE IMPLEMENTED ***/

    return SUCCESS;
}

PHP_MINFO_FUNCTION(aerospike)
{
    DEBUG_PHP_EXT_INFO("In aerospike info");

    php_info_print_table_start();
    php_info_print_table_row(2, "aerospike support", "enabled");
    php_info_print_table_row(2, "aerospike version", PHP_AEROSPIKE_VERSION);
    php_info_print_table_end();
}
