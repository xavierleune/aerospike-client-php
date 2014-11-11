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
#include "ext/standard/info.h"

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
#include "aerospike_general_constants.h"

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
   STD_PHP_INI_ENTRY("aerospike.udf.lua_system_path", "/opt/aerospike/client-php/sys-lua", PHP_INI_PERDIR|PHP_INI_SYSTEM, OnUpdateString, lua_system_path, zend_aerospike_globals, aerospike_globals)
   STD_PHP_INI_ENTRY("aerospike.udf.lua_user_path", "/opt/aerospike/client-php/usr-lua", PHP_INI_PERDIR|PHP_INI_SYSTEM, OnUpdateString, lua_user_path, zend_aerospike_globals, aerospike_globals)
   STD_PHP_INI_ENTRY("aerospike.key_policy", "0", PHP_INI_PERDIR|PHP_INI_SYSTEM, OnUpdateString, key_policy, zend_aerospike_globals, aerospike_globals)
   STD_PHP_INI_ENTRY("aerospike.key_gen", "0", PHP_INI_PERDIR|PHP_INI_SYSTEM, OnUpdateString, key_gen, zend_aerospike_globals, aerospike_globals)
PHP_INI_END()


ZEND_DECLARE_MODULE_GLOBALS(aerospike)

static void aerospike_check_close_and_destroy(void *hashtable_element) {
    TSRMLS_FETCH();
    DEBUG_PHP_EXT_DEBUG("In destructor function");
    aerospike_ref *as_ref_p = ((zend_rsrc_list_entry *) hashtable_element)->ptr;
    as_error error;
    if (as_ref_p) {
        if (as_ref_p->ref_hosts_entry > 1) {
            as_ref_p->ref_hosts_entry--;
        } else {
            if (as_ref_p->as_p) {
                if (AEROSPIKE_OK != aerospike_close(as_ref_p->as_p, &error)) {
                    DEBUG_PHP_EXT_ERROR("Aerospike close returned error");
                }
                aerospike_destroy(as_ref_p->as_p);
            }
            as_ref_p->ref_hosts_entry = 0;
            as_ref_p->as_p = NULL;
            if (as_ref_p) {
                pefree(as_ref_p, 1);
            }
            as_ref_p = NULL;
        }
        DEBUG_PHP_EXT_INFO("aerospike c sdk object destroyed");
    } else {
        DEBUG_PHP_EXT_ERROR("invalid aerospike object");
    }
}

static void aerospike_globals_ctor(zend_aerospike_globals *globals TSRMLS_DC)
{
    DEBUG_PHP_EXT_DEBUG("In ctor");
    pthread_rwlock_init(&AEROSPIKE_G(aerospike_mutex), NULL);
    if ((!(AEROSPIKE_G(persistent_list_g))) || (AEROSPIKE_G(persistent_ref_count) < 1)) {
        AEROSPIKE_G(persistent_list_g) = (HashTable *)pemalloc(sizeof(HashTable), 1);
        zend_hash_init(AEROSPIKE_G(persistent_list_g), 1000, NULL, &aerospike_check_close_and_destroy, 1);
        AEROSPIKE_G(persistent_ref_count) = 1;
    } else {
        AEROSPIKE_G(persistent_ref_count)++;
    }
}

static void aerospike_globals_dtor(zend_aerospike_globals *globals TSRMLS_DC)
{
    if (globals->persistent_list_g) {
        if (AEROSPIKE_G(persistent_ref_count) == 1) {
            DEBUG_PHP_EXT_DEBUG("Ref count is working");
            zend_hash_clean(AEROSPIKE_G(persistent_list_g));
            zend_hash_destroy(AEROSPIKE_G(persistent_list_g));
            AEROSPIKE_G(persistent_list_g) = NULL;
            AEROSPIKE_G(persistent_ref_count) = 0;
        } else {
            AEROSPIKE_G(persistent_ref_count)--;
        }
    }
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
 * Using "arginfo_third_by_ref" in zend_arg_info argument of a
 * zend_function_entry accepts third argument of the
 * corresponding functions by reference and rest by value.
 ********************************************************************
 */
ZEND_BEGIN_ARG_INFO(arginfo_third_by_ref, 0)
ZEND_ARG_PASS_INFO(0)
ZEND_ARG_PASS_INFO(0)
ZEND_ARG_PASS_INFO(1)
ZEND_END_ARG_INFO()
/*
 ********************************************************************
 * Using "arginfo_fifth_by_ref" in zend_arg_info argument of a
 * zend_function_entry accepts fifth argument of the
 * corresponding functions by reference and rest by value.
 ********************************************************************
 */
ZEND_BEGIN_ARG_INFO(arginfo_fifth_by_ref, 0)
ZEND_ARG_PASS_INFO(0)
ZEND_ARG_PASS_INFO(0)
ZEND_ARG_PASS_INFO(0)
ZEND_ARG_PASS_INFO(0)
ZEND_ARG_PASS_INFO(1)
ZEND_ARG_PASS_INFO(0)
ZEND_END_ARG_INFO()

/*
 ********************************************************************
 * Using "arginfo_sixth_by_ref" in zend_arg_info argument of a
 * zend_function_entry accepts sixth argument of the
 * corresponding functions by reference and rest by value.
 ********************************************************************
 */
ZEND_BEGIN_ARG_INFO(arginfo_sixth_by_ref, 0)
    ZEND_ARG_PASS_INFO(0)
    ZEND_ARG_PASS_INFO(0)
    ZEND_ARG_PASS_INFO(0)
    ZEND_ARG_PASS_INFO(0)
    ZEND_ARG_PASS_INFO(0)
    ZEND_ARG_PASS_INFO(1)
ZEND_END_ARG_INFO()

/*
 ********************************************************************
 * Using "arginfo_seventh_by_ref" in zend_arg_info argument of a
 * zend_function_entry accepts seventh argument of the
 * corresponding functions by reference and rest by value.
 ********************************************************************
 */
ZEND_BEGIN_ARG_INFO(arginfo_seventh_by_ref, 0)
    ZEND_ARG_PASS_INFO(0)
    ZEND_ARG_PASS_INFO(0)
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
    PHP_ME(Aerospike, info, arginfo_sec_by_ref, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, infoMany, NULL, ZEND_ACC_PUBLIC)
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
    PHP_ME(Aerospike, operate, arginfo_third_by_ref, ZEND_ACC_PUBLIC)
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
     *  Secondary Index APIs:
     ********************************************************************
     */
    PHP_ME(Aerospike, createIndex, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, dropIndex, NULL, ZEND_ACC_PUBLIC)
    /*
     ********************************************************************
     * Query and Scan APIs:
     ********************************************************************
     */
    PHP_ME(Aerospike, predicateBetween, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Aerospike, predicateEquals, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Aerospike, query, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, aggregate, arginfo_seventh_by_ref, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, scan, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, scanApply, arginfo_sixth_by_ref, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, scanInfo, arginfo_sec_by_ref, ZEND_ACC_PUBLIC)

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

    /*
     ********************************************************************
     * Batch Operations:
     ********************************************************************
     */
    PHP_ME(Aerospike, existsMany, arginfo_sec_by_ref, ZEND_ACC_PUBLIC)
    PHP_ME(Aerospike, getMany, arginfo_sec_by_ref, ZEND_ACC_PUBLIC)

#if 0 // TBD

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
    /*Aerospike_object *intern_obj_p;
      intern_obj_p = aerospike_object; */

    /* if (intern_obj_p && intern_obj_p->as_ref_p) {
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
       }*/
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
        intern_obj_p->as_ref_p = NULL;
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
    //zend_object_value retval;
    zend_object_value retval = {0};
    Aerospike_object *intern_obj_p;

    if (NULL != (intern_obj_p = ecalloc(1, sizeof(Aerospike_object)))) {
      //  printf("intern_obj_p = %u \n", intern_obj_p);
        zend_object_std_init(&(intern_obj_p->std), ce TSRMLS_CC);
#if PHP_VERSION_ID < 50399
        zend_hash_copy(intern_obj_p->std.properties, &ce->default_properties, (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval *));
#else
        object_properties_init((zend_object*) &(intern_obj_p->std), ce);
#endif

        retval.handle = zend_objects_store_put(intern_obj_p, Aerospike_object_dtor, (zend_objects_free_object_storage_t) Aerospike_object_free_storage, NULL TSRMLS_CC);
        retval.handlers = &Aerospike_handlers;
        intern_obj_p->as_ref_p = NULL;
        //return (retval);
    } else {
        DEBUG_PHP_EXT_ERROR("Could not allocate memory for aerospike object");
    }
    return (retval);
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
    char*                  ini_value = NULL;
    HashTable              *persistent_list;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;
    persistent_list = (AEROSPIKE_G(persistent_list_g));

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
    strcpy(config.lua.system_path, ini_value = LUA_SYSTEM_PATH_PHP_INI);
    strcpy(config.lua.user_path, ini_value = LUA_USER_PATH_PHP_INI);

    /* check for hosts, user and pass within config*/
    transform_zval_config_into transform_zval_config_into_as_config;
    transform_zval_config_into_as_config.transform_result.as_config_p = &config;
    transform_zval_config_into_as_config.transform_result_type = TRANSFORM_INTO_AS_CONFIG;

    if (AEROSPIKE_OK != (aerospike_transform_check_and_set_config(Z_ARRVAL_P(config_p),
                    NULL, &transform_zval_config_into_as_config/*&config*/))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to find host parameter");
        DEBUG_PHP_EXT_ERROR("Unable to find host parameter");
        goto exit;
    }

    /* check and set config policies */
    set_general_policies(&config, options_p, &error TSRMLS_CC);
    if (AEROSPIKE_OK != (error.code)) {
        status = error.code;
        DEBUG_PHP_EXT_ERROR("Unable to set policies");
        goto exit;
    }
    if (AEROSPIKE_OK != (status = aerospike_helper_object_from_alias_hash(aerospike_obj_p,
                    persistent_connection, &config, persistent_list, persist TSRMLS_CC))){
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

    DEBUG_PHP_EXT_INFO("Success in creating php-aerospike object");
exit:
    // pthread_mutex_unlock(&aerospike_mutex);
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
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


    DEBUG_PHP_EXT_INFO("Destruct method of aerospike object executed");
exit:
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
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

    if (!aerospike_obj_p || !(aerospike_obj_p->as_ref_p) ||
            !(aerospike_obj_p->as_ref_p->as_p)) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if (aerospike_obj_p->is_persistent == false) {
        if (AEROSPIKE_OK != (status = aerospike_close(aerospike_obj_p->as_ref_p->as_p, &error))) {
            DEBUG_PHP_EXT_ERROR("Aerospike close returned error");
            PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
        }
    } else {
        if (AEROSPIKE_OK ==
                aerospike_helper_close_php_connection(aerospike_obj_p,
                    &error TSRMLS_CC)) {
            DEBUG_PHP_EXT_ERROR("Aerospike close returned error");
            PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
        }
        PHP_EXT_RESET_AS_ERR_IN_CLASS();
    }

    /* Now as connection is getting closed we need to set the connection flag to false */
    aerospike_obj_p->is_conn_16 = AEROSPIKE_CONN_STATE_FALSE;

exit:
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
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

    if (AEROSPIKE_OK != (status = aerospike_transform_get_record(aerospike_obj_p,
                    &as_key_for_get_record,
                    options_p,
                    &error,
                    record_p,
                    bins_p TSRMLS_CC))) {
        DEBUG_PHP_EXT_ERROR("get function returned an error");
        goto exit;
    }

exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_get_record);
    }
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
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
                    &record_p, &as_key_for_put_record, &error, ttl_u32, options_p TSRMLS_CC))) {
        DEBUG_PHP_EXT_ERROR("put function returned an error");
        goto exit;
    }

exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_put_record);
    }
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 * PHP Method:  Aerospike::getNodes()
 *******************************************************************************************************
 * Get the addresses of the cluster nodes
 * Method prototype for PHP userland:
 * public array Aerospike::getNodes ( void )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, getNodes)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        status = AEROSPIKE_ERR;
        goto exit;
    }

    if (PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER,
                "getNodes: connection not established");
        DEBUG_PHP_EXT_ERROR("getNodes: connection not established");
        status = AEROSPIKE_ERR;
        goto exit;
    }

    array_init(return_value);

    if (AEROSPIKE_OK !=
            (status = aerospike_info_get_cluster_nodes(aerospike_obj_p->as_ref_p->as_p,
                                                       &error, return_value,
                                                       NULL, NULL TSRMLS_CC))) {
        DEBUG_PHP_EXT_ERROR("getNodes function returned an error");
        goto exit;
    }

exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    if (AEROSPIKE_OK != status) {
        zval_dtor(return_value);
        RETURN_NULL();
    }
}

/*
 *******************************************************************************************************
 * PHP Method:  Aerospike::info()
 *******************************************************************************************************
 * Send an Info request to an Aerospike cluster.
 * Method prototype for PHP userland:
 * public int Aerospike::info ( string $request, string &$response [, array $host [, array options ]] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, info)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    char*                  request = NULL;
    zval*                  response_p = NULL;
    long                   request_len = 0;
    zval*                  host = NULL;
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
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "Info: connection not established");
        DEBUG_PHP_EXT_ERROR("Info: connection not established");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|zz",
                &request, &request_len, &response_p,
                &host, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse php parameters for Info function");
        DEBUG_PHP_EXT_ERROR("Unable to parse php parameters for Info function.");
        goto exit;
    }

    if (!request || (host && PHP_TYPE_ISNOTARR(host)) || ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for Info function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for Info function not proper.");
        goto exit;
    }

    zval_dtor(response_p);

    if (AEROSPIKE_OK !=
            (status = aerospike_info_specific_host(aerospike_obj_p->as_ref_p->as_p, &error,
                    request, response_p, host, options_p TSRMLS_CC))) {
        DEBUG_PHP_EXT_ERROR("Info function returned an error");
        goto exit;
    }

exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 * PHP Method:  Aerospike::infoMany()
 *******************************************************************************************************
 * Aerospike::info - send an info request to multiple cluster nodes.
 * Method prototype for PHP userland:
 * public array Aerospike::infoMany ( string $request [, array $config [, array options ]] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, infoMany)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    char*                  request_p = NULL;
    long                   request_len = 0;
    zval*                  config_p = NULL;
    zval*                  options_p = NULL;
    Aerospike_object*      aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        status = AEROSPIKE_ERR;
        goto exit;
    }

    if (PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER,
                "InfoMany: connection not established");
        DEBUG_PHP_EXT_ERROR("InfoMany: connection not established");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|aa",
                &request_p, &request_len, &config_p, &options_p) == FAILURE) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to parse php parameters for InfoMany function");
        DEBUG_PHP_EXT_ERROR("Unable to parse php parameters for InfoMany function.");
        goto exit;
    }

    if (!request_p || (config_p && PHP_TYPE_ISNOTARR(config_p))
            || ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))){
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Input parameters (type) for InfoMany function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for InfoMany function not proper.");
        goto exit;
    }

    array_init(return_value);

    if (AEROSPIKE_OK !=
            (status = aerospike_info_request_multiple_nodes(aerospike_obj_p->as_ref_p->as_p,
                    &error, request_p, config_p, return_value, options_p TSRMLS_CC))) {
        DEBUG_PHP_EXT_ERROR("InfoMany function returned an error");
        goto exit;
    }

exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    if (AEROSPIKE_OK != status) {
        zval_dtor(return_value);
        RETURN_NULL();
    }
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
 * PHP Method:  Aerospike::existsMany()
 *******************************************************************************************************
 * Appends a string to the string value in a bin.
 * Method prototype for PHP userland:
 * public int Aerospike::existsMany ( array $keys, array &$metadata
 * [, array $options] )
 *
 * @param $keys             An array of initialized keys, Each An array
 *                          with keys ['ns', 'set', 'key'] or
 *                          ['ns', 'set', 'digest'] 
 * @param metadata          Filled by an array of metadata
 * @param options           Optional parameter.           
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, existsMany)
{
    as_status               status = AEROSPIKE_OK;
    as_error                error;
    zval*                   keys_p = NULL;
    zval*                   metadata_p = NULL;
    zval*                   options_p = NULL;
    Aerospike_object*       aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "existsMany : connection not established"); 
        DEBUG_PHP_EXT_ERROR("existsMany : connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "az|a", &keys_p, &metadata_p, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse parameters for existsMany");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for existsMany");
        goto exit;
    }

    if ((PHP_TYPE_ISNOTARR(keys_p)) ||
            ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for existsMany function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for existsMany function not proper");
        goto exit;
    }

    zval_dtor(metadata_p);
    array_init(metadata_p);

    if (AEROSPIKE_OK !=
            (status = aerospike_batch_operations_exists_many(aerospike_obj_p->as_ref_p->as_p,
                                                             &error, keys_p, metadata_p,
                                                             options_p TSRMLS_CC))) {
        DEBUG_PHP_EXT_ERROR("existsMany() function returned an error");
        goto exit;
    }

exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 * PHP Method:  Aerospike::getMany()
 *******************************************************************************************************
 * Aerospike::getMany - gets a batch of record from the Aerospike database
 * Method prototype for PHP userland:
 * public int Aerospike::getMany ( array $keys, array &$records [, array $filter [, array $options ]] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, getMany)
{
    as_status               status = AEROSPIKE_OK;
    as_error                error;
    zval*                   keys_p = NULL;
    zval*                   records_p = NULL;
    zval*                   filter_bins_p = NULL;
    zval*                   options_p = NULL;
    Aerospike_object*       aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if(PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER,
                "getMany : connection not established"); 
        DEBUG_PHP_EXT_ERROR("getMany : connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "az|za", &keys_p,
                &records_p, &filter_bins_p, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to parse parameters for getMany");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for getMany");
        goto exit;
    }

    if (filter_bins_p && PHP_TYPE_ISNULL(filter_bins_p)) {
        filter_bins_p = NULL;
    }

    zval_dtor(records_p);
    array_init(records_p);

    if (AEROSPIKE_OK != (status = aerospike_batch_operations_get_many(aerospike_obj_p->as_ref_p->as_p,
                    &error, keys_p, records_p, filter_bins_p, options_p TSRMLS_CC))) {
        DEBUG_PHP_EXT_ERROR("existsMany() function returned an error");
        goto exit;
    }

exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 * PHP Method:  Aerospike::operate()
 *******************************************************************************************************
 * Perform multiple operations on a record
 * Method prototype for PHP userland:
 * public int Aerospike::operate ( array $key, array $operations [,array &$returned [,array $options ]] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, operate)
{
    as_status              status = AEROSPIKE_OK;
    zval*                  key_record_p = NULL;
    zval*                  operations_p = NULL;
    zval*                  returned_p = NULL;
    zval*                  options_p = NULL;
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
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "operate: connection not established"); 
        DEBUG_PHP_EXT_ERROR("operate: connection not established");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "za|za",
                &key_record_p, &operations_p, &returned_p, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse php parameters for operate function");
        DEBUG_PHP_EXT_ERROR("Unable to parse php parameters for operate function");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p) ||
            PHP_TYPE_ISNOTARR(operations_p) ||
            ((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Input parameters (type) for operate function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for operate function not proper");
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
                    &as_key_for_get_record,
                    &initializeKey))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to parse key parameters for operate function");
        DEBUG_PHP_EXT_ERROR("Unable to parse key parameters for operate function");
        goto exit;
    }

    if(returned_p) {
        zval_dtor(returned_p);
        array_init(returned_p);
    }

    if (AEROSPIKE_OK !=
            (status = aerospike_record_operations_operate(aerospike_obj_p,
                    &as_key_for_get_record,
                    options_p,
                    &error,
                    returned_p,
                    Z_ARRVAL_P(operations_p)))) {
        DEBUG_PHP_EXT_ERROR("Operate function returned an error");
        goto exit;
    }

exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_get_record);
    }
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    RETURN_LONG(status);
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

    if (AEROSPIKE_OK != (status = aerospike_record_operations_general(aerospike_obj_p,
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
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
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

    if (AEROSPIKE_OK != (status = aerospike_record_operations_remove(aerospike_obj_p, &as_key_for_put_record, &error, options_p))) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Unable to remove record");
        DEBUG_PHP_EXT_ERROR("Unable to remove record");
        goto exit;
    }

exit:
    if (initializeKey) {
        as_key_destroy(&as_key_for_put_record);
    }
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
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

    status = aerospike_php_exists_metadata(aerospike_obj_p, key_record_p, metadata_p, options_p, &error);

exit:
    if (status != AEROSPIKE_OK) {
        PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    } else {
        PHP_EXT_RESET_AS_ERR_IN_CLASS();
    }
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
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

    status = aerospike_php_exists_metadata(aerospike_obj_p, key_record_p, metadata_p, options_p, &error);

exit:
    if (status != AEROSPIKE_OK) {
        PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    } else {
        PHP_EXT_RESET_AS_ERR_IN_CLASS();
    }
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
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

    if (AEROSPIKE_OK != (status = aerospike_record_operations_general(aerospike_obj_p,
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
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
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

    if (AEROSPIKE_OK != (status = aerospike_record_operations_general(aerospike_obj_p,
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
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
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

    if (AEROSPIKE_OK != (status = aerospike_record_operations_general(aerospike_obj_p,
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
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 * PHP Method:  Aerospike::initKey()
 *******************************************************************************************************
 * Helper method for building the key array
 * Method prototype for PHP userland:
 * public array Aerospike::initKey ( string $ns, string $set, int|string $pk [,
 *                                   boolean $digest = false ])
 *
 * @param ns                The namespace
 * @param set               The name of set within the namespace
 * @param pk                It can be primary key or digest value
 *                          that identifies the record uniquely.
 * @param digest            It can be true or false.
 *                          false - Indicates pk contains primary key(default)
 *                          true - Indicates pk contains digest value.
 *
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
    zend_bool              is_digest = false;


    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssz|b", &ns_p, &ns_p_length,
                                         &set_p, &set_p_length, &pk_p, &is_digest)) {
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

    if (AEROSPIKE_OK != aerospike_init_php_key(ns_p, ns_p_length, set_p, set_p_length, pk_p,
                is_digest, return_value, NULL, NULL TSRMLS_CC)) {
        DEBUG_PHP_EXT_ERROR("initkey() function returned an error");
        zval_dtor(return_value); 
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

    if (user_deserializer_call_info.function_name &&
            (Z_ISREF_P(user_deserializer_call_info.function_name))) {
        RETURN_TRUE;
    }
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

    if (user_serializer_call_info.function_name &&
        (Z_ISREF_P(user_serializer_call_info.function_name))) {
        /*
         * once set the same serializer would be used. Incase a new 
         * serializer is to be given, then we would have to unref the older
         * serialiser function and then attach the new serializer callback
         */
        RETURN_TRUE;
    }

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

    as_error_init(&error);
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

    if (AEROSPIKE_OK != (status = aerospike_record_operations_remove_bin(aerospike_obj_p, &as_key_for_put_record, bins_p, &error, options_p))) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Unable to remove bin");
        DEBUG_PHP_EXT_ERROR("Unable to remove bin");
        goto exit;
    }

exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
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

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz",
                &bin_name_p, &bin_name_len, &val_p)) {
        DEBUG_PHP_EXT_ERROR("Invalid parameters for predicateEquals");
        RETURN_NULL();
    }

    if (bin_name_len == 0) {
        DEBUG_PHP_EXT_ERROR("Aerospike::predicateEquals() expects parameter 1 to be a non-empty string.");
        RETURN_NULL();
    }

    if (PHP_TYPE_ISNULL(val_p)) {
        DEBUG_PHP_EXT_ERROR("Aerospike::predicateEquals() expects parameter 2 to be a non-empty string or an integer.");
        RETURN_NULL();
    }

    array_init(return_value);
    add_assoc_stringl(return_value, BIN, bin_name_p, bin_name_len, 1);
    add_assoc_stringl(return_value, OP, "=", sizeof("=") - 1, 1);

    switch(Z_TYPE_P(val_p)) {
        case IS_LONG:
            add_assoc_long(return_value, VAL, Z_LVAL_P(val_p));
            break;
        case IS_STRING:
            if (strlen(Z_STRVAL_P(val_p)) == 0) {
                zval_dtor(return_value);
                DEBUG_PHP_EXT_ERROR("Aerospike::predicateEquals() expects parameter 2 to be a non-empty string or an integer.");
                RETURN_NULL();
            }
            add_assoc_string(return_value, VAL, Z_STRVAL_P(val_p), 1);
            break;
        default:
            zval_dtor(return_value);
            DEBUG_PHP_EXT_ERROR("Aerospike::predicateEquals() expects parameter 2 to be a non-empty string or an integer.");
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

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sll",
                &bin_name_p, &bin_name_len, &min_p, &max_p)) {
        DEBUG_PHP_EXT_ERROR("Invalid parameters for predicateBetween");
        RETURN_NULL();
    }
    if (bin_name_len == 0) {
        DEBUG_PHP_EXT_ERROR("Aerospike::predicateBetween() expects parameter 1 to be a non-empty string.");
        RETURN_NULL();
    }

    array_init(return_value);
    add_assoc_stringl(return_value, BIN, bin_name_p, bin_name_len, 1);
    add_assoc_stringl(return_value, OP, "BETWEEN", sizeof("BETWEEN") - 1, 1);
    MAKE_STD_ZVAL(minmax_arr);
    array_init_size(minmax_arr, 2);
    add_next_index_long(minmax_arr, min_p);
    add_next_index_long(minmax_arr, max_p);
    add_assoc_zval(return_value, VAL, minmax_arr);
}

/*
 *******************************************************************************************************
 * PHP Method:  Aerospike::query()
 *******************************************************************************************************
 * Queries a secondary index on a set in the Aerospike database.
 * Method prototype for PHP userland:
 * public int Aerospike::query ( string $ns, string $set, array $where, callback
 * $record_cb [, array $select [, array $options ]] )
 *
 * @param ns                The namespace
 * @param set               The set
 * @param where             The predicate for the query, conforming to one of the following:
 *                          Associative Array:
 *                              bin => bin name
 *                              op => one of Aerospike::OP_EQ, Aerospike::OP_BETWEEN
 *                              val => scalar integer/string for OP_EQ or array($min, $max) for
 *                                     OP_BETWEEN
 * @param record_cb         A callback function invoked for each record streaming back from
 *                          the server.
 * @param select            An array of bin names to be returned.
 * @param options           Options including Aerospike::OPT_READ_TIMEOUT.
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, query)
{
    as_status               status = AEROSPIKE_OK;
    as_error                error;
    char*                   ns_p = NULL;
    int                     ns_p_length = 0;
    char*                   set_p = NULL;
    int                     set_p_length = 0;
    zval*                   predicate_p = NULL;
    char*                   bin_name = NULL;
    zval*                   options_p = NULL;
    zend_fcall_info         fci = empty_fcall_info;
    zend_fcall_info_cache   fcc = empty_fcall_info_cache;
    zval*                   bins_p = NULL;
    HashTable*              bins_ht_p = NULL;
    Aerospike_object*       aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    PHP_EXT_SET_AS_ERR(&error, DEFAULT_ERRORNO, DEFAULT_ERROR);

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        DEBUG_PHP_EXT_ERROR("Aerospike::query() has no valid aerospike object");
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR,
                "Aerospike::query() has no valid aerospike object");
        goto exit;
    }

    if (PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        DEBUG_PHP_EXT_ERROR("Aerospike::query() has no connection to the database");
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER,
                "Aerospike::query() has no connection to the database");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssaf|aa",
        &ns_p, &ns_p_length, &set_p, &set_p_length, &predicate_p,
        &fci, &fcc, &bins_p, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("Aerospike::query() unable to parse parameters");
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Aerospike::query() unable to parse parameters");
        goto exit;
    }
    if (ns_p_length == 0 || set_p_length == 0) {
        status = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("Aerospike::query() expects parameter 1 & 2 to be a non-empty strings.");
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Aerospike::query() expects parameter 1 & 2 to be a non-empty strings.");
        goto exit;
    }
    if (PHP_TYPE_ISNOTARR(predicate_p)) {
        status = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("Aerospike::query() expects parameter 3 to be an array.");
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Aerospike::query() expects parameter 3 to be an array.");
        goto exit;
    }

    userland_callback user_func;
    user_func.fci_p =  &fci;
    user_func.fcc_p = &fcc;
    user_func.obj = aerospike_obj_p;

    bins_ht_p = (bins_p ? Z_ARRVAL_P(bins_p) : NULL);

    if (AEROSPIKE_OK !=
            (status = aerospike_query_run(aerospike_obj_p->as_ref_p->as_p,
                                          &error, ns_p, set_p, &user_func,
                                          bins_ht_p, Z_ARRVAL_P(predicate_p),
                                          options_p TSRMLS_CC))) {
        DEBUG_PHP_EXT_ERROR("scan returned an error");
        goto exit;
    }

exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 * PHP Method:  Aerospike::aggregate()
 *******************************************************************************************************
 * Applies a stream UDF to a secondary index query.
 * Method prototype for PHP userland:
 * public int Aerospike::aggregate ( string $ns, string $set, array $where, string $module,
 * string $function, array $args, mixed &$returned [, array $options ] )
 *
 * @param ns                The namespace
 * @param set               The set
 * @param where             The predicate for the query, conforming to one of the following:
 *                          Associative Array:
 *                              bin => bin name
 *                              op => one of Aerospike::OP_EQ, Aerospike::OP_BETWEEN
 *                              val => scalar integer/string for OP_EQ or array($min, $max) for
 *                                     OP_BETWEEN
 * @param module            The name of the UDF module registered against the Aerospike DB.
 * @param function          The name of the function to be applied to the records.
 * @param args              An array of arguments for the UDF.
 * @param returned          The aggregated return value to be populated by this
 *                          method.
 * @param options           Options including Aerospike::OPT_WRITE_TIMEOUT and
 *                          Aerospike::OPT_READ_TIMEOUT.
 * @return                  Returns an integer status code. Compare to the Aerospike class status
 *                          constants.  When non-zero the Aerospike::error() and
 *                          Aerospike::errorno() methods can be used.
 *
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, aggregate)
{
    as_status               status = AEROSPIKE_OK;
    as_error                error;
    char*                   module_p = NULL;
    char*                   function_name_p = NULL;
    char*                   namespace_p = NULL;
    char*                   set_p = NULL;
    zval*                   module_zval_p = NULL;
    zval*                   function_zval_p = NULL;
    zval*                   namespace_zval_p = NULL;
    zval*                   set_zval_p = NULL;
    zval*                   predicate_p = NULL;
    long                    module_len = 0;
    long                    function_len = 0;
    long                    namespace_len = 0;
    long                    set_len = 0;
    zval*                   returned_p = NULL;
    zval*                   bins_p = NULL;
    zval*                   args_p = NULL;
    zval*                   options_p = NULL;
    HashTable*              bins_ht_p = NULL;
    Aerospike_object*       aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    /*
     * TODO:
     * Add support for optional filter bins by simply adding it as second last
     * parameter in zend_parse_parameters().
     */

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if (PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR,
                "aggregate: Connection not established");
        DEBUG_PHP_EXT_ERROR("aggregate: Connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                "zzzzzzz|z", &namespace_zval_p, &set_zval_p, &predicate_p,
                &module_zval_p, &function_zval_p, &args_p, &returned_p,
                &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to parse parameters for aggregate()");
        DEBUG_PHP_EXT_ERROR("Unable to parse the parameters for aggregate()");
        goto exit;
    }

    if (((args_p) && (PHP_TYPE_ISNOTARR(args_p)) &&
                (PHP_TYPE_ISNOTNULL(args_p))) || ((options_p) &&
                (PHP_TYPE_ISNOTARR(options_p)) &&
                (PHP_TYPE_ISNOTNULL(options_p))) ||
            (PHP_TYPE_ISNOTSTR(module_zval_p)) ||
            (PHP_TYPE_ISNOTSTR(function_zval_p)) ||
            (PHP_TYPE_ISNOTSTR(namespace_zval_p)) ||
            (PHP_TYPE_ISNOTSTR(set_zval_p)) ||
            (PHP_TYPE_ISNOTARR(predicate_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Input parameters (type) for aggregate function are not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for aggregate function are not proper");
        goto exit;
    }

    if (args_p && PHP_TYPE_ISNULL(args_p)) {
        args_p = NULL;
    }

    if (options_p && PHP_TYPE_ISNULL(options_p)) {
        options_p = NULL;
    }

    module_p = Z_STRVAL_P(module_zval_p);
    function_name_p = Z_STRVAL_P(function_zval_p);
    namespace_p = Z_STRVAL_P(namespace_zval_p);
    set_p = Z_STRVAL_P(set_zval_p);

    module_len = Z_STRLEN_P(module_zval_p);
    function_len = Z_STRLEN_P(function_zval_p);
    namespace_len = Z_STRLEN_P(namespace_zval_p);
    set_len = Z_STRLEN_P(set_zval_p);

    if (module_len == 0 || function_len == 0 || namespace_len == 0
            || set_len == 0) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Expects parameter 1,2,4 and 5 to be non-empty strings");
        goto exit;
    }

    bins_ht_p = (bins_p ? Z_ARRVAL_P(bins_p) : NULL);

    zval_dtor(returned_p);
    array_init(returned_p);

    if (AEROSPIKE_OK !=
            (status = aerospike_query_aggregate(aerospike_obj_p->as_ref_p->as_p,
                                                &error, module_p, function_name_p,
                                                &args_p, namespace_p, set_p,
                                                bins_ht_p, Z_ARRVAL_P(predicate_p),
                                                returned_p, options_p TSRMLS_CC))) {
        DEBUG_PHP_EXT_ERROR("aggregate returned an error");
        goto exit;
    }
exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    RETURN_LONG(status);
}
/*
 *******************************************************************************************************
 * PHP Method:  Aerospike::scan()
 *******************************************************************************************************
 * Scans a set in the Aerospike database.
 * Method prototype for PHP userland:
 * public int Aerospike::scan ( string $ns, string $set, callback $record_cb [,
 *      array $select [, array $options ]] )
 *
 * @param ns                The namespace
 * @param set               The set to be scanned
 * @param record_cb         A callback function invoked for each record streaming back from
 *                          the server.
 * @param bins              An array of bin names to be returned.
 * @param percent           The percentage of data to scan.
 * @param scan_priority     The priority of the scan.
 * @param concurrent        Whether to scan all nodes in parallel.
 * @param no_bins           Whether to return only metabins and exclude bins.
 * @param options           Options including 
 *                          Aerospike::OPT_READ_TIMEOUT
 *                          Aerospike::OPT_SCAN_PRIORITY
 *                          Aerospike::OPT_SCAN_PERCENTAGE of the records in the set to return
 *                          Aerospike::OPT_SCAN_CONCURRENTLY whether to run the scan in parallel
 *                          Aerospike::OPT_SCAN_NOBINS whether to not retrieve bins for
 *                          the records
 * @return                  Returns an integer status code.  Compare to the Aerospike class status
 *                          constants.  When non-zero the Aerospike::error() and
 *                          Aerospike::errorno() methods can be used.
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
    zval                   *bins_p = NULL;
    zval                   *options_p = NULL;
    HashTable*             bins_ht_p = NULL;

    /*
     * initialized to 'no error' (status AEROSPIKE_OK, empty message)
     */
    as_error_init(&error);

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        DEBUG_PHP_EXT_ERROR("Aerospike::scan() has no valid aerospike object");
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Aerospike::scan() has no valid aerospike object");
        goto exit;
    }

    if (PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        DEBUG_PHP_EXT_ERROR("Aerospike::scan() has no valid aerospike object");
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "Aerospike::scan() has no connection to the database");
        goto exit;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssf|aza",
        &ns_p, &ns_p_length, &set_p, &set_p_length,
        &fci, &fcc, &bins_p, &options_p) == FAILURE) {
        status = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("Aerospike::scan() has no valid aerospike object");
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Aerospike::scan() unable to parse parameters");
        goto exit;
    }

    if (ns_p_length == 0 || set_p_length == 0) {
        status = AEROSPIKE_ERR_PARAM;
        DEBUG_PHP_EXT_ERROR("Aerospike::scan() has no valid aerospike object");
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM, "Aerospike::scan() expects parameter 1 & 2 to be a non-empty strings.");
        goto exit;
    }

    userland_callback user_func;
    user_func.fci_p =  &fci;
    user_func.fcc_p = &fcc;
    user_func.obj = aerospike_obj_p;

    bins_ht_p = (bins_p ? Z_ARRVAL_P(bins_p) : NULL);

    if (AEROSPIKE_OK !=
            (status = aerospike_scan_run(aerospike_obj_p->as_ref_p->as_p,
                                     &error, ns_p, set_p, &user_func,
                                     bins_ht_p, options_p TSRMLS_CC))) {
        DEBUG_PHP_EXT_ERROR("scan returned an error");
        goto exit;
    }

exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 * PHP Method:  Aerospike::scanApply()
 *******************************************************************************************************
 * Initiates a background read/write scan by applying a record UDF to each record being scanned.
 * Method prototype for PHP userland:
 * public int Aerospike::scanApply ( string $ns, string $set, string $module,
 *          string $function, array $args, int &$scan_id [, array $options ] )
 *
 * @param ns                The namespace
 * @param set               The set to be scanned
 * @param module            The name of the UDF module registered against the Aerospike DB.
 * @param function          The name of the function to be applied to the records.
 * @param args              An array of arguments for the UDF.
 * @param scan_id           Scan_id filled by an integer handle for the initiated background scan
 * @param options           Options including
 *                          Aerospike::OPT_WRITE_TIMEOUT 
 *                          Aerospike::OPT_SCAN_PRIORITY The priority of the scan.
 *                          Aerospike::OPT_SCAN_PERCENTAGE of the records in the set to return
 *                          Aerospike::OPT_SCAN_CONCURRENTLY whether to run the
 *                          scan in parallel
 *                          Aerospike::OPT_SCAN_NOBINS whether to not retrieve
 *                          bins for the records
 * @return                  Returns an integer status code.  Compare to the Aerospike class status
 *                          constants.  When non-zero the **Aerospike::error()** and
 *                          Aerospike::errorno() methods can be used.
 *
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, scanApply)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    char*                  module_p = NULL;
    char*                  function_name_p = NULL;
    char*                  namespace_p = NULL;
    char*                  set_p = NULL;
    zval*                  module_zval_p = NULL;
    zval*                  function_zval_p = NULL;
    zval*                  namespace_zval_p = NULL;
    zval*                  set_zval_p = NULL;
    zval*                  scan_id_p = NULL;
    long                   module_len = 0;
    long                   function_len = 0;
    long                   namespace_len = 0;
    long                   set_len = 0;
    zval*                  args_p = NULL;
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
                "scanApply: Connection not established");
        DEBUG_PHP_EXT_ERROR("scanApply : Connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                "zzzzzz|z", &namespace_zval_p, &set_zval_p,
                &module_zval_p, &function_zval_p, &args_p, &scan_id_p,
                &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to parse parameters for scanApply()");
        DEBUG_PHP_EXT_ERROR("Unable to parse the parameters for scanApply()");
        goto exit;
    }

    if (((args_p) && (PHP_TYPE_ISNOTARR(args_p)) &&
                (PHP_TYPE_ISNOTNULL(args_p))) || ((options_p) &&
                (PHP_TYPE_ISNOTARR(options_p)) &&
                (PHP_TYPE_ISNOTNULL(options_p))) ||
            (PHP_TYPE_ISNOTSTR(module_zval_p)) ||
            (PHP_TYPE_ISNOTSTR(function_zval_p)) ||
            (PHP_TYPE_ISNOTSTR(namespace_zval_p)) ||
            (PHP_TYPE_ISNOTSTR(set_zval_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Input parameters (type) for scanApply function are not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for scanApply function are not proper");
        goto exit;
    }

    if (args_p && PHP_TYPE_ISNULL(args_p)) {
        args_p = NULL;
    }

    if (options_p && PHP_TYPE_ISNULL(options_p)) {
        options_p = NULL;
    }
    module_p = Z_STRVAL_P(module_zval_p);
    function_name_p = Z_STRVAL_P(function_zval_p);
    namespace_p = Z_STRVAL_P(namespace_zval_p);
    set_p = Z_STRVAL_P(set_zval_p);

    module_len = Z_STRLEN_P(module_zval_p);
    function_len = Z_STRLEN_P(function_zval_p);
    namespace_len = Z_STRLEN_P(namespace_zval_p);
    set_len = Z_STRLEN_P(set_zval_p);

    if (module_len == 0 || function_len == 0 || namespace_len == 0 || set_len == 0) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Expects parameter 1,2,4 and 5 to be non-empty strings");
        goto exit;
    }

    if ((options_p) && (PHP_TYPE_ISNOTARR(options_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Input parameters (type) for scanApply function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for scanApply function not proper");
    }

    zval_dtor(scan_id_p);
    ZVAL_LONG(scan_id_p, 0);
    if (AEROSPIKE_OK !=
            (status = aerospike_scan_run_background(aerospike_obj_p->as_ref_p->as_p,
                                                &error, module_p, function_name_p,
                                                &args_p, namespace_p, set_p,
                                                scan_id_p, options_p TSRMLS_CC))) {
        DEBUG_PHP_EXT_ERROR("scanApply returned an error");
        goto exit;
    }
exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 * PHP Method:  Aerospike::scanInfo()
 *******************************************************************************************************
 * Check the progress of background scan running on the database.
 * Method prototype for PHP userland:
 * public int Aerospike::scanInfo ( integer $scan_id, array &$info [, array
 *                                  $options ] )
 *
 * @param scan_id           The scan id
 * @param info              The status of the background scan returned as an associative array
 *                          conforming to the following:
 *                          Associative Array:
 *                              progress_pct => percentage progress
 *                              records_scanned => no. of records scanned
 *                              status => one of Aerospike::SCAN_STATUS_UNDEF,
 *                                        Aerospike::SCAN_STATUS_INPROGRESS,
 *                                        Aerospike::SCAN_STATUS_ABORTED,
 *                                        Aerospike::SCAN_STATUS_COMPLETED
 *
 * @param options           Options including Aerospike::OPT_WRITE_TIMEOUT and
 *                          Aerospike::OPT_READ_TIMEOUT.
 * @return                  Returns an integer status code.  Compare to the Aerospike class
 *                          status constants.  When non-zero the Aerospike::error() and
 *                          Aerospike::errorno() methods can be used.
 ******************************************************************************************************
 */
PHP_METHOD(Aerospike, scanInfo)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    long                   scan_id = -1;
    zval*                  scan_info_p = NULL;
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
                "scanInfo: Connection not established");
        DEBUG_PHP_EXT_ERROR("scanInfo: Connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz|z",
                &scan_id, &scan_info_p, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to parse parameters for scanInfo()");
        DEBUG_PHP_EXT_ERROR("Unable to parse the parameters for scanInfo()");
        goto exit;
    }

    if (((options_p) && (PHP_TYPE_ISNOTARR(options_p)) &&
                (PHP_TYPE_ISNOTNULL(options_p)))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Input parameters (type) for scanInfo function are not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for scanInfo function are not proper");
        goto exit;
    }

    if (options_p && PHP_TYPE_ISNULL(options_p)) {
        options_p = NULL;
    }

    zval_dtor(scan_info_p);
    array_init(scan_info_p);

    if (AEROSPIKE_OK !=
            (status = aerospike_scan_get_info(aerospike_obj_p->as_ref_p->as_p,
                                              &error, scan_id, scan_info_p,
                                              options_p TSRMLS_CC))) {
        DEBUG_PHP_EXT_ERROR("scanInfo returned an error");
        goto exit;
    }
exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
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
 *                                  $language = Aerospike::UDF_TYPE_LUA [, array
 *                                  $options]] )
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
    zval*                  module_zval_p = NULL;
    zval*                  path_zval_p = NULL;
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

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|la",
                &path_zval_p, &module_zval_p, &language, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to parse parameters for register function");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for register function");
        goto exit;
    }

    if (((options_p) && (PHP_TYPE_ISNOTARR(options_p))) ||
            (PHP_TYPE_ISNOTSTR(module_zval_p)) ||
            (PHP_TYPE_ISNOTSTR(path_zval_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Input parameters (type) for register function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for reegister function not proper");
    }

    module_p = Z_STRVAL_P(module_zval_p);
    path_p = Z_STRVAL_P(path_zval_p);

    module_len = Z_STRLEN_P(module_zval_p);
    path_len = Z_STRLEN_P(path_zval_p);

    if (path_len == 0 || module_len == 0) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Expects parameter 1 & 2 to be non-empty strings");
        goto exit;
    }

    if (AEROSPIKE_OK !=
            (status = aerospike_udf_register(aerospike_obj_p, &error, path_p,
                                             module_p, language, options_p))) {
        DEBUG_PHP_EXT_ERROR("register function returned an error");
        goto exit;
    }
exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 * PHP Method : Aerospike::deregister()
 *******************************************************************************************************
 * Removes a UDF module from the Aerospike DB.
 * Method prototype for PHP userland:
 * public int Aerospike::deregister ( string $module [, array $options ])
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, deregister)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    char*                  module_p = NULL;
    zval*                  module_zval_p = NULL;
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

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|a",
                &module_zval_p, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to parse parameters for deregister function");
        DEBUG_PHP_EXT_ERROR("Unable to parse parameters for deregister function");
        goto exit;
    }

    if (((options_p) && (PHP_TYPE_ISNOTARR(options_p))) ||
            (PHP_TYPE_ISNOTSTR(module_zval_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Input parameters (type) for deregister function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for deregister function not proper");
    }

    module_p = Z_STRVAL_P(module_zval_p);

    if (Z_STRLEN_P(module_zval_p) == 0) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Expects parameter 1 to be non-empty string");
        goto exit;
    }

    if (AEROSPIKE_OK !=
            (status = aerospike_udf_deregister(aerospike_obj_p, &error,
                                               module_p, options_p))) {
        DEBUG_PHP_EXT_ERROR("deregister function returned an error");
        goto exit;
    }
exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 * PHP Method : Aerospike::apply()
 *******************************************************************************************************
 * Applies UDF on record in the Aerospike DB.
 * Method prototype for PHP userland:
 * public int Aerospike::apply ( array $key, string $module, string $function [,
 *                               array $args [, mixed &$returned  [, array $options ]]] )
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
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER,
                "apply: Connection not established");
        DEBUG_PHP_EXT_ERROR("apply: Connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz|zzz",
                &key_record_p, &module_zval_p, &function_zval_p, &args_p,
                &return_value_of_udf_p, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to parse parameters for apply()");
        DEBUG_PHP_EXT_ERROR("Unable to parse the parameters for apply()");
        goto exit;
    }

    if (PHP_TYPE_ISNOTARR(key_record_p) ||
            ((args_p) &&
             (PHP_TYPE_ISNOTARR(args_p)) &&
             (PHP_TYPE_ISNOTNULL(args_p))) ||
            ((options_p) &&
             (PHP_TYPE_ISNOTARR(options_p)) &&
             (PHP_TYPE_ISNOTNULL(options_p))) ||
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

    if (options_p && PHP_TYPE_ISNULL(options_p)) {
        options_p = NULL;
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

    if (return_value_of_udf_p) {
        zval_dtor(return_value_of_udf_p);
    } else {
        MAKE_STD_ZVAL(return_value_of_udf_p);
    }
    array_init(return_value_of_udf_p);

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
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
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
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER,
                "listRegistered: Connection not established");
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
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 * PHP Method : Aerospike::getRegistered()
 *******************************************************************************************************
 * Get the code for a UDF module registered with the server.
 * Method prototype for PHP userland:
 * public int Aerospike::getRegistered ( string $module, string &$code [, int
 *                                       $language = Aerospike::UDF_TYPE_LUA
 *                                       [, array $options ]] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, getRegistered)
{
    as_status              status = AEROSPIKE_OK;
    as_error               error;
    char*                  module_p = NULL;
    long                   language = UDF_TYPE_LUA;
    zval*                  module_zval_p = NULL;
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
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER,
                "getRegistered: Connection not established");
        DEBUG_PHP_EXT_ERROR("getRegistered: Connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|lz",
                &module_zval_p, &udf_code_p, &language, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to parse parameters for getRegistered()");
        DEBUG_PHP_EXT_ERROR("Unable to parse the parameters for getRegistered()");
        goto exit;
    }

    if (((options_p) && (PHP_TYPE_ISNOTARR(options_p))) ||
            (PHP_TYPE_ISNOTSTR(module_zval_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Input parameters (type) for getRegistered function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for getRegistered function not proper");
    }

    module_p = Z_STRVAL_P(module_zval_p);

    if (Z_STRLEN_P(module_zval_p) == 0) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Expects parameter 1 to be non-empty string");
        goto exit;
    }

    zval_dtor(udf_code_p);

    if (AEROSPIKE_OK !=
            (status = aerospike_get_registered_udf_module_code(aerospike_obj_p,
                                                               &error, module_p,
                                                               udf_code_p,
                                                               language, 
                                                               options_p))) {
        ZVAL_EMPTY_STRING(udf_code_p);
        DEBUG_PHP_EXT_ERROR("getRegistered function returned an error");
        goto exit;
    }
exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 *  Secondary Index APIs:
 *******************************************************************************************************
 */

/*
 *******************************************************************************************************
 * PHP Method:  Aerospike::createIndex()
 *******************************************************************************************************
 * Creates a secondary index on a bin in the Aerospike database.
 * Method prototype for PHP userland:
 * public int Aerospike::createIndex ( string $ns, string $set, string $bin, int $type, string $name
 * [, array $options ] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, createIndex)
{
    as_status               status = AEROSPIKE_OK;
    as_error                error;
    char                    *ns_p = NULL;
    int                     ns_p_length = 0;
    char                    *set_p = NULL;
    int                     set_p_length = 0;
    char                    *bin_p = NULL;
    int                     bin_p_length = 0;
    long                    type = -1;
    char                    *name_p = NULL;
    int                     name_p_length = 0;
    zval*                   options_p = NULL;
    Aerospike_object*       aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if (PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER,
                "createIndex: Connection not established");
        DEBUG_PHP_EXT_ERROR("createIndex: Connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssls|z",
                &ns_p, &ns_p_length, &set_p, &set_p_length, &bin_p,
                &bin_p_length, &type, &name_p, &name_p_length, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to parse parameters for createIndex()");
        DEBUG_PHP_EXT_ERROR("Unable to parse the parameters for createIndex()");
        goto exit;
    }

    if (ns_p_length == 0 || set_p_length == 0 || bin_p_length == 0 || name_p_length == 0) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Aerospike::createIndex() expects parameters 1-3 and 5 to be non-empty strings");
        DEBUG_PHP_EXT_ERROR("Aerospike::createIndex() expects parameters 1-3 and 5 to be non-empty strings");
        goto exit;
    }

    if ((options_p) && (PHP_TYPE_ISNOTARR(options_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Input parameters (type) for createIndex function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for createIndex function not proper");
    }

    if (AEROSPIKE_OK !=
            (status = aerospike_index_create_php(aerospike_obj_p->as_ref_p->as_p,
                                             &error, ns_p, set_p, bin_p, type,
                                             name_p, options_p TSRMLS_CC))) {
        DEBUG_PHP_EXT_ERROR("createIndex() function returned an error");
        goto exit;
    }

exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    RETURN_LONG(status);
}

/*
 *******************************************************************************************************
 * PHP Method:  Aerospike::dropIndex()
 *******************************************************************************************************
 * Drops a secondary index on a bin in the Aerospike database.
 * Method prototype for PHP userland:
 * public int Aerospike::dropIndex ( string $ns, string $name [, array $options ] )
 *******************************************************************************************************
 */
PHP_METHOD(Aerospike, dropIndex)
{
    as_status               status = AEROSPIKE_OK;
    as_error                error;
    char                    *ns_p = NULL;
    int                     ns_p_length = 0;
    char                    *name_p = NULL;
    int                     name_p_length = 0;
    zval*                   options_p = NULL;
    Aerospike_object*       aerospike_obj_p = PHP_AEROSPIKE_GET_OBJECT;

    if (!aerospike_obj_p) {
        status = AEROSPIKE_ERR;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if (PHP_IS_CONN_NOT_ESTABLISHED(aerospike_obj_p->is_conn_16)) {
        status = AEROSPIKE_ERR_CLUSTER;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER,
                "dropIndex: Connection not established");
        DEBUG_PHP_EXT_ERROR("dropIndex: Connection not established");
        goto exit;
    }

    if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|z",
                &ns_p, &ns_p_length, &name_p, &name_p_length, &options_p)) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Unable to parse parameters for dropIndex()");
        DEBUG_PHP_EXT_ERROR("Unable to parse the parameters for dropIndex()");
        goto exit;
    }

    if (ns_p_length == 0 || name_p_length == 0) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Aerospike::dropIndex() expects parameters 1-2 to be non-empty strings");
        DEBUG_PHP_EXT_ERROR("Aerospike::dropIndex() expects parameters 1-2 to be non-empty strings");
        goto exit;
    }

    if ((options_p) && (PHP_TYPE_ISNOTARR(options_p))) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_PARAM,
                "Input parameters (type) for dropIndex function not proper");
        DEBUG_PHP_EXT_ERROR("Input parameters (type) for dropIndex function not proper");
    }

    if (AEROSPIKE_OK !=
            (status = aerospike_index_remove_php(aerospike_obj_p->as_ref_p->as_p,
                                                 &error, ns_p, name_p,
                                                 options_p TSRMLS_CC))) {
        DEBUG_PHP_EXT_ERROR("dropIndex() function returned an error");
        goto exit;
    }

exit:
    PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
    RETURN_LONG(status);
}

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

    as_log_set_level(log_level);

exit:
    if (status != AEROSPIKE_OK) {
        PHP_EXT_SET_AS_ERR_IN_CLASS(&error);
    } else {
        PHP_EXT_RESET_AS_ERR_IN_CLASS();
    }
    aerospike_helper_set_error(Aerospike_ce, getThis() TSRMLS_CC);
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

    as_log_set_callback((as_log_callback)&aerospike_helper_log_callback);
    is_callback_registered = 1;
    Z_ADDREF_P(func_call_info.function_name);
    PHP_EXT_RESET_AS_ERR_IN_CLASS();
    RETURN_TRUE;
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
 *******************************************************************************************************
 * Function to declare policy constants in Aerospike class.
 *
 * @param Aerospike_ce          The zend class entry for Aerospike class.
 *
 * @return AEROSPIKE_OK if the success. Otherwise AEROSPIKE_x.
 *******************************************************************************************************
 */
/*
static as_status declare_policy_constants_php(zend_class_entry *Aerospike_ce TSRMLS_DC)
{
    int32_t i;
    as_status   status = AEROSPIKE_OK;

    if (!Aerospike_ce) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    for (i = 0; i <= AEROSPIKE_CONSTANTS_ARR_SIZE; i++) {
        zend_declare_class_constant_long(
                Aerospike_ce, aerospike_constants[i].constant_str,
                strlen(aerospike_constants[i].constant_str),
                aerospike_constants[i].constantno TSRMLS_CC);
    }

exit:
    return status;
}
*/
//zend_class_entry *Aerospike_ce;
//static zend_class_entry ce;
/*
 ********************************************************************
 * Aerospike module init.
 ********************************************************************
 */
PHP_MINIT_FUNCTION(aerospike)
{
    DEBUG_PHP_EXT_DEBUG("Inside minit");
    zend_class_entry ce={0};
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
#ifdef ZTS
    ts_allocate_id(&aerospike_globals_id, sizeof(zend_aerospike_globals), (ts_allocate_ctor) aerospike_globals_ctor, (ts_allocate_dtor) aerospike_globals_dtor);
#else
    ZEND_INIT_MODULE_GLOBALS(aerospike, aerospike_globals_ctor, aerospike_globals_dtor);
#endif
    REGISTER_INI_ENTRIES();
    //	Aerospike_ce->get_iterator = Aerospike_get_iterator;
    /* Refer aerospike_policy.h
     * This will expose the policy values for PHP
     * as well as CSDK to PHP client.
     */
    declare_policy_constants_php(Aerospike_ce TSRMLS_CC);

    //declare_policy_constants_php(Aerospike_ce);
    /* Refer aerospike_status.h
     * This will expose the status code from CSDK
     * to PHP client.
     */
    zend_declare_property_long(Aerospike_ce, "errorno", strlen("errorno"), DEFAULT_ERRORNO, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_string(Aerospike_ce, "error", strlen("error"), DEFAULT_ERROR, ZEND_ACC_PRIVATE TSRMLS_CC);

    EXPOSE_LOGGER_CONSTANTS_STR_ZEND(Aerospike_ce);
    EXPOSE_STATUS_CODE_ZEND(Aerospike_ce);
    EXPOSE_GENERAL_CONSTANTS_LONG_ZEND(Aerospike_ce);
    EXPOSE_GENERAL_CONSTANTS_STRING_ZEND(Aerospike_ce);

    return SUCCESS;
}

/*
 ********************************************************************
 * Aerospike module shutdown.
 ********************************************************************
 */
PHP_MSHUTDOWN_FUNCTION(aerospike)
{
    DEBUG_PHP_EXT_DEBUG("Inside mshutdown");
    UNREGISTER_INI_ENTRIES();
#ifndef ZTS
    aerospike_globals_dtor(&aerospike_globals TSRMLS_CC);
#else
    ts_free_id(aerospike_globals_id);
#endif
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

    DEBUG_PHP_EXT_DEBUG("Inside rinit of this build");
    return SUCCESS;
}

/*
 ********************************************************************
 * Aerospike request shutdown.
 ********************************************************************
 */
PHP_RSHUTDOWN_FUNCTION(aerospike)
{
    if (user_serializer_call_info.function_name) {
        if (1 == Z_REFCOUNT_P(user_serializer_call_info.function_name)) {
            zval_ptr_dtor(&user_serializer_call_info.function_name);
            user_serializer_call_info.function_name = NULL;
        } else {
            DEBUG_PHP_EXT_ERROR("leak for user serializer function");
        }
    }
    if (user_deserializer_call_info.function_name) {
        if (1 == Z_REFCOUNT_P(user_deserializer_call_info.function_name)) {
            zval_ptr_dtor(&user_deserializer_call_info.function_name);
            user_deserializer_call_info.function_name = NULL;
        } else {
            DEBUG_PHP_EXT_ERROR("leak for user deserializer function");
        }
    }

    DEBUG_PHP_EXT_DEBUG("Inside rshutdown of this build");
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

