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




/**
 *  Callback for each list element
 *
 *  @param value     Value of current list element
 *  @param list      List to which current value is to be appended
 *
 *  @return true if success. Otherwise false.
 */
#if 0
    bool
callback_for_each_list_element(as_val *value, zval **list)
{
    as_error err;
    //TODO: Yet to complete handling all cases
    zval *tmp;
    switch (as_val_type(value)) {
        case AS_UNDEF:
        case AS_NIL:
            add_next_index_null(*list);
            break;
        case AS_BOOLEAN:
            add_next_index_bool(*list, (int) as_boolean_get((as_boolean *) value));
            break;
        case AS_INTEGER:
            add_next_index_long(*list, (long) as_integer_get((as_integer *) value));
            break;
        case AS_STRING:
            add_next_index_stringl(*list, as_string_get((as_string *) value), strlen(as_string_get((as_string *) value)), 1);
            break;
        case AS_LIST:
            MAKE_STD_ZVAL(tmp);
            array_init(tmp);
            as_list_foreach((as_list *) value, (as_list_foreach_callback) callback_for_each_list_element, &tmp);
            add_next_index_zval(*list, tmp);
            break;
        case AS_MAP:
            MAKE_STD_ZVAL(tmp);
            array_init(tmp);
            as_map_foreach((as_map *) value, (as_map_foreach_callback) callback_for_each_map_element, &tmp);
            add_next_index_zval(*list, tmp);
        case AS_REC:
            //TODO: Handle as_rec
        case AS_PAIR:
            //TODO: Handle as_pair
        case AS_BYTES:
            //TODO: Handle as_bytes
        default:
            /*
             * For now, using AEROSPIKE_ERR. Need to have specific
             * error code here.
             */
            err.code = AEROSPIKE_ERR_PARAM;
            goto failure;
    }
    return true;
failure:
    fprintf(stderr, "error(%d) %s at [%s:%d]\n", err.code, err.message, err.file, err.line);
    return false;
}

/**
 *  Callback for each map element
 *
 *  @param key       Key of the current map element
 *  @param value     Value of current map element
 *  @param arr       Array to which current key-value is to be appended
 *
 *  @return true if success. Otherwise false.
 */
    bool
callback_for_each_map_element(as_val *key, as_val *value, zval **arr)
{
    //TODO: Yet to complete handling all cases
    //TODO: Handle Non string Key

    zval *tmp;
    as_error err;
    switch (as_val_type(value)) {
        case AS_UNDEF:
        case AS_NIL:
            add_assoc_null(*arr, as_string_get((as_string *) key));
            break;
        case AS_BOOLEAN:
            add_assoc_bool(*arr, as_string_get((as_string *) key), (int) as_boolean_get((as_boolean *) value));
            break;
        case AS_INTEGER:
            add_assoc_long(*arr, as_string_get((as_string *) key), (long) as_integer_get((as_integer *) value));
            break;
        case AS_STRING:
            add_assoc_stringl(*arr, as_string_get((as_string *) key), as_string_get((as_string *) value), strlen(as_string_get((as_string *) value)), 1);
            break;
        case AS_LIST:
            MAKE_STD_ZVAL(tmp);
            array_init(tmp);
            as_list_foreach((as_list *) value, (as_list_foreach_callback) callback_for_each_list_element, &tmp);
            add_assoc_zval(*arr, as_string_get((as_string *) key), tmp);
            break;
        case AS_MAP:
            MAKE_STD_ZVAL(tmp);
            array_init(tmp);
            as_map_foreach((as_map *) value, (as_map_foreach_callback) callback_for_each_map_element, &tmp);
            add_assoc_zval(*arr, as_string_get((as_string *) key), tmp);
            break;
        case AS_REC:
            //TODO: Handle rec
            break;
        case AS_PAIR:
            //TODO: Handle pair
            break;
        case AS_BYTES:
            //TODO: Handle bytes
            break;
        default:
            /*
             * For now, using AEROSPIKE_ERR. Need to have specific
             * error code here.
             */
            err.code = AEROSPIKE_ERR_PARAM;
            goto failure;
    }
    return true;
failure:
    fprintf(stderr, "error(%d) %s at [%s:%d]\n", err.code, err.message, err.file, err.line);
    return false;
}

/**
 *  Appends a bin and value to input parameter bins_array
 *
 *  @param name            Bin name
 *  @param value           Bin value
 *  @param bins_array      Bin array to be appended to.
 *
 *  @return true if success. Otherwise false.
 */
    bool 
update_bins_array(const char *name, const as_val *value, zval *bins_array)
{
    //TODO: Yet to complete all cases
    zval *tmp;
    as_error err;
    zval class_constant;
    switch (as_val_type(value)) {
        case AS_UNDEF | AS_UNKNOWN:
            add_assoc_null(bins_array, name);
            break;
        case AS_NIL:
            add_assoc_null(bins_array, name);
            break;
        case AS_BOOLEAN:
            add_assoc_bool(bins_array, name, (int) as_boolean_get((as_boolean *) value));
            break;
        case AS_INTEGER:
            add_assoc_long(bins_array, name, as_integer_get((as_integer *) value));
            break;
        case AS_STRING:
            add_assoc_stringl(bins_array, name, as_string_get((as_string *) value), strlen(as_string_get((as_string *) value)), 1);
            break;
        case AS_LIST:
            MAKE_STD_ZVAL(tmp);
            array_init(tmp);
            as_list_foreach((as_list *) value, (as_list_foreach_callback) callback_for_each_list_element, &tmp);
            add_assoc_zval(bins_array, name, tmp);
            break;
        case AS_MAP:
            MAKE_STD_ZVAL(tmp);
            array_init(tmp);
            as_map_foreach((as_map *) value, (as_map_foreach_callback) callback_for_each_map_element, &tmp);
            add_assoc_zval(bins_array, name, tmp);
            break;
        case AS_REC:
            //TODO: Handle as_rec
        case AS_PAIR:
            //TODO: Handle as_pair
        case AS_BYTES:
            //TODO: Handle as_bytes
        default:
            /*
             * For now, using AEROSPIKE_ERR. Need to have specific
             * error code here.
             */
            err.code = AEROSPIKE_ERR_PARAM;
            goto failure;
    }
    return true;
failure:
    fprintf(stderr, "error(%d) %s at [%s:%d]\n", err.code, err.message, err.file, err.line);
    return false;
}
#endif


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
    if (AEROSPIKE_OK != (set_policy(&config.policies.read, &config.policies.write, options_p))) {
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

    DEBUG_PHP_EXT_INFO("success in creating php-aerospike object")
exit:
    /*RETURN_LONG(Z_LVAL(class_constant));*/
    RETURN_LONG(status);
}

/* PHP Method:  bool Aerospike::get()
   Read record header(s) and bin(s) for specified key(s) in one batch call. */
PHP_METHOD(Aerospike, get)
{
    zval *record_identifier, **record_key = NULL, *record, *bins = NULL, *options = NULL;
    char *arrkey, *namespace = NULL, *set = NULL;
    as_record *rec = NULL;
    // Errors populate this object.
    as_error err;

    // DEBUG
    log_info("**In Aerospike::get() method**\n");

    zval *object = getThis();
    Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "za|aa", &record_identifier, &record, &bins, &options) == FAILURE) {
        /*
         * For now, using AEROSPIKE_ERR. Need to have specific
         * error code here.
         */
        err.code = AEROSPIKE_ERR_PARAM;
        goto failure;
    }

    as_key key;

    HashTable *keyindex = Z_ARRVAL_P(record_identifier);
    HashPosition pointer;
    zval **data;

    foreach_hashtable (keyindex, pointer, data) {
        uint arrkey_len, arrkey_type;
        ulong index;

        arrkey_type = zend_hash_get_current_key_ex(keyindex, &arrkey, &arrkey_len, &index, 0, &pointer);

        if (strcmp(arrkey, "ns") == 0) {
            namespace = Z_STRVAL_PP(data);
        } else if (strcmp(arrkey, "set") == 0) {
            set = Z_STRVAL_PP(data);
        } else if (strcmp(arrkey, "key") == 0) {
            record_key = data;
        } else {
            /*
             * For now, using AEROSPIKE_ERR. Need to have specific
             * error code here.
             */
            err.code = AEROSPIKE_ERR_PARAM;
            goto failure;
        }
    }

    if (namespace == NULL) {
        /*
         * For now, using AEROSPIKE_ERR. Need to have specific
         * error code here.
         */
        err.code = AEROSPIKE_ERR_PARAM;
        goto failure;
    } 
    if (set == NULL) {
        /*
         * For now, using AEROSPIKE_ERR. Need to have specific
         * error code here.
         */
        err.code = AEROSPIKE_ERR_PARAM;
        goto failure;
    } 
    if (record_key == NULL) {
        /*
         * For now, using AEROSPIKE_ERR. Need to have specific
         * error code here.
         */
        err.code = AEROSPIKE_ERR_PARAM;
        goto failure;
    }

    switch (Z_TYPE_PP(record_key)) {
        case IS_LONG:
            as_key_init_int64(&key, namespace, set, (int64_t) Z_LVAL_PP(record_key));
            break;
        case IS_STRING:
            as_key_init_str(&key, namespace, set, (char *) Z_STRVAL_PP(record_key));
            break;
        default:
            /*
             * For now, using AEROSPIKE_ERR. Need to have specific
             * error code here.
             */
            err.code = AEROSPIKE_ERR_PARAM;
            goto failure;
    } 

    as_policy_read read_policy;
    if (AEROSPIKE_OK != (err.code = set_policy(&read_policy, NULL, options))) {
        goto failure;
    }

    if (bins == NULL) {
        if (aerospike_key_get(intern->as_p, &err, &read_policy, &key, &rec) != AEROSPIKE_OK) {
            /*
             * For now, using AEROSPIKE_ERR. Need to have specific
             * error code here.
             */
            err.code = AEROSPIKE_ERR_PARAM;
            goto failure;
        } else {
            // rec contains record, hence process it and add bins of the record to input array record
            if (as_record_foreach(rec, (as_rec_foreach_callback) AS_DEFAULT_GET, record)) {
                zval class_constant;
                zend_get_constant_ex(ZEND_STRL("Aerospike::OK"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
                RETURN_LONG(Z_LVAL(class_constant));
            } else {
                /*
                 * For now, using AEROSPIKE_ERR. Need to have specific
                 * error code here.
                 */
                err.code = AEROSPIKE_ERR_SERVER;
                goto failure;
            }
        }
    } else if (bins != NULL) {
        HashTable *bins_array =  Z_ARRVAL_P(bins);
        int bins_count = zend_hash_num_elements(bins_array);
        const char *select[bins_count];
        zval **bin_names;
        uint i=0;

        foreach_hashtable (bins_array, pointer, bin_names) {
            switch (Z_TYPE_PP(bin_names)) {
                case IS_STRING:
                    select[i++] = Z_STRVAL_PP(bin_names);
                    break;
                default:
                    /*
                     * For now, using AEROSPIKE_ERR. Need to have specific
                     * error code here.
                     */
                    err.code = AEROSPIKE_ERR_PARAM;
                    goto failure;
            }
        }

        select[bins_count] = NULL;
        if (aerospike_key_select(intern->as_p, &err, &read_policy, &key, select, &rec) != AEROSPIKE_OK) {
            /*
             * For now, using AEROSPIKE_ERR. Need to have specific
             * error code here.
             */
            err.code = AEROSPIKE_ERR_PARAM;
            goto failure;
        } else {
            if (as_record_foreach(rec, (as_rec_foreach_callback) AS_DEFAULT_GET, record)) {
                //zval class_constant;
                //zend_get_constant_ex(ZEND_STRL("Aerospike::OK"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
                //RETURN_LONG(Z_LVAL(class_constant));
                RETURN_LONG(0);
            } else {
                /*
                 * For now, using AEROSPIKE_ERR. Need to have specific
                 * error code here.
                 */
                err.code = AEROSPIKE_ERR_SERVER;
                goto failure;
            }
        }
    }
failure:
    // XXX do we need to destroy read policy
    if (rec) {
        as_record_destroy(rec);
    }
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

#if 0
/* PHP Method:  bool Aerospike::put()
   Write record bin(s). */
PHP_METHOD(Aerospike, put)
{
    // DEBUG
    log_info("**In Aerospike::put() method**");

    zval *object = getThis();
    zval *record_identifier, *record, **record_key = NULL, **bin_value, *options = NULL;
    char *bin_name , *arrkey, *namespace = NULL, *set = NULL;
    long ttl = 0;
    as_record rec;
    as_error err;

    Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "aa|la", &record_identifier, &record, &ttl, &options) == FAILURE) {
        /*
         * For now, using AEROSPIKE_ERR. Need to have specific
         * error code here.
         */
        err.code = AEROSPIKE_ERR_PARAM;
        goto failure;
    }

    HashTable *keyindex = Z_ARRVAL_P(record_identifier);
    HashPosition pointer;
    zval **data;

    foreach_hashtable (keyindex, pointer, data) {
        uint arrkey_len, arrkey_type;
        ulong index;

        arrkey_type = zend_hash_get_current_key_ex(keyindex, &arrkey, &arrkey_len, &index, 0, &pointer);

        if (strcmp(arrkey, "ns") == 0) {
            namespace = Z_STRVAL_PP(data);
        } else if (strcmp(arrkey, "set") == 0) {
            set = Z_STRVAL_PP(data);
        } else if (strcmp(arrkey, "key") == 0) {
            record_key = data;
        } else {
            /*
             * For now, using AEROSPIKE_ERR. Need to have specific
             * error code here.
             */
            err.code = AEROSPIKE_ERR_PARAM;
            goto failure;
        }
    }

    if (namespace == NULL) {
        /*
         * For now, using AEROSPIKE_ERR. Need to have specific
         * error code here.
         */
        err.code = AEROSPIKE_ERR_PARAM;
        goto failure;
    }
    if (set == NULL) {
        /*
         * For now, using AEROSPIKE_ERR. Need to have specific
         * error code here.
         */
        err.code = AEROSPIKE_ERR_PARAM;
        goto failure;
    }
    if (record_key == NULL) {
        /*
         * For now, using AEROSPIKE_ERR. Need to have specific
         * error code here.
         */
        err.code = AEROSPIKE_ERR_PARAM;
        goto failure;
    }

    HashTable *record_index = Z_ARRVAL_P(record);
    zval **dataval;
    int array_count;

    array_count = zend_hash_num_elements(record_index);

    as_record_inita(&rec, array_count);

    foreach_hashtable (record_index, pointer, dataval) {
        uint arrkey_len, arrkey_type;
        ulong index;

        arrkey_type = zend_hash_get_current_key_ex(record_index, &arrkey, &arrkey_len, &index, 0, &pointer);

        bin_name = arrkey;
        bin_value = dataval;
        zval class_constant, **data;
        HashTable *arr_hash;
        int array_len;
        HashPosition pointer;
        as_arraylist *list;
        as_hashmap *map;
        char *key;

        switch (Z_TYPE_PP(bin_value)) {
            case IS_NULL:
                as_record_set_nil(&rec, bin_name);
                break;
            case IS_LONG:
                as_record_set_int64(&rec, bin_name, (int64_t) Z_LVAL_PP(bin_value));
                break;
            case IS_STRING:
                as_record_set_str(&rec, bin_name, (char *) Z_STRVAL_PP(bin_value));
                break;
            case IS_ARRAY:
                arr_hash = Z_ARRVAL_PP(bin_value);				
                zend_hash_internal_pointer_reset_ex(arr_hash, &pointer);
                zend_hash_get_current_data_ex(arr_hash, (void **) &data, &pointer);

                if (zend_hash_get_current_key_ex(arr_hash, &key, &arrkey_len, &index, 0, &pointer) == HASH_KEY_IS_STRING) {
                    map = as_hashmap_new(32);
                    as_map *m = (as_map *) map;
                    handle_put_map(bin_value, m);
                    as_record_set_map(&rec, bin_name, m);
                    as_hashmap_destroy(map);
                } else {
                    array_len = zend_hash_num_elements(arr_hash);
                    list = as_arraylist_new(array_len, 0);
                    handle_put_list(bin_value, list);
                    as_record_set_list(&rec, bin_name, (as_list *) as_val_reserve(list));
                    as_arraylist_destroy(list);
                }
                break;
            default:
                /*
                 * For now, using AEROSPIKE_ERR. Need to have specific
                 * error code here.
                 */
                err.code = AEROSPIKE_ERR_PARAM;
                goto failure;
        }
    }

    as_key key;

    switch (Z_TYPE_P(*record_key)) {
        case IS_LONG:
            as_key_init_int64(&key, namespace, set, (int64_t) Z_LVAL_PP(record_key));
            break;
        case IS_STRING:
            as_key_init_str(&key, namespace, set, (char *) Z_STRVAL_PP(record_key));
            break;
        default:
            /*
             * For now, using AEROSPIKE_ERR. Need to have specific
             * error code here.
             */
            err.code = AEROSPIKE_ERR_PARAM;
            goto failure;
    }	

    // set optional TTL
    if (ttl != 0) {
        rec.ttl = ttl;
    }

    // set optional policies
    as_policy_write write_policy;
    if (AEROSPIKE_OK != (err.code = set_policy(NULL, &write_policy, options))) {
        goto failure;
    }
    if (aerospike_key_put(&as, &err, &write_policy, &key, &rec) != AEROSPIKE_OK) {
        // An error occurred, so we log it.
        goto failure;
        //log_err(err.message, );
    }
    // XXX destroy policy ???
    zval class_constant;
    zend_get_constant_ex(ZEND_STRL("Aerospike::OK"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
    RETURN_LONG(Z_LVAL(class_constant));
failure:
    /* TODO Any memory cleanup */
    fprintf(stderr, "error(%d) %s at [%s:%d]\n", err.code, err.message, err.file, err.line);
    RETURN_LONG(err.code);
}
#endif

#if 0
/**
 *  Copies list elements from a PHP array into an as_arraylist
 *  
 *  @param array      a PHP array to be copied into an as_arraylist
 *  @param list       an as_arrayist into which the PHP array is copied
 *
 *  @return true if success. Otherwise false.
 */
int
handle_put_list(zval **array, as_arraylist *list)
{
    HashTable *arr_hash, *inner_arr_hash;
    HashPosition pointer, inner_pointer;
    zval **data, **inner_data, class_constant;
    char *key, *inner_key;
    int inner_array_len, i;
    uint arrkey_len, inner_arrkey_len;
    ulong index, inner_index;
    as_arraylist *inner_list;
    as_hashmap *inner_map;
    as_error err;

    arr_hash = Z_ARRVAL_PP(array);
    foreach_hashtable (arr_hash, pointer, data) {
        switch (Z_TYPE_PP(data)) {
            case IS_LONG:
                as_arraylist_append_int64(list, Z_LVAL_PP(data));
                break;
            case IS_STRING:
                as_arraylist_append_str(list, Z_STRVAL_PP(data));
                break;
            case IS_ARRAY:
                inner_arr_hash = Z_ARRVAL_PP(data);
                zend_hash_internal_pointer_reset_ex(inner_arr_hash, &inner_pointer);

                if (zend_hash_get_current_key_ex(inner_arr_hash, &inner_key, &inner_arrkey_len, &inner_index, 0, &inner_pointer) == HASH_KEY_IS_STRING) {
                    inner_map = as_hashmap_new(32);
                    as_map *inner_m = (as_map *) inner_map;
                    handle_put_map(data, inner_m);
                    as_arraylist_append_map(list, inner_m);
                } else {
                    inner_array_len = zend_hash_num_elements(inner_arr_hash);
                    inner_list = as_arraylist_new(inner_array_len, 0);
                    handle_put_list(data, inner_list);
                    as_arraylist_append_list(list, (as_list *) inner_list);
                }
                break;			
            default:
                /*
                 * For now, using AEROSPIKE_ERR. Need to have specific
                 * error code here.
                 */
                err.code = AEROSPIKE_ERR_PARAM;
                goto failure;
        }
    }
failure:
    /* TODO Any memory cleanup */
    fprintf(stderr, "error(%d) %s at [%s:%d]\n", err.code, err.message, err.file, err.line);
    return err.code;
}

/**
 *  Copies map elements from a PHP array into an as_map
 *    
 *  @param array      a PHP array to be copied into an as_map
 *  @param map        an as_map into which the PHP array is copied
 * 
 *  @return true if success. Otherwise false.
 */
    int
handle_put_map(zval **array, as_map *map)
{
    HashTable *arr_hash, *inner_arr_hash;
    HashPosition pointer, inner_pointer;
    zval **data, **inner_data, class_constant;
    char *key, *inner_key, map_key_type[10];
    uint arrkey_len, arrkey_type, inner_arrkey_len;
    ulong index, inner_index;
    as_arraylist* inner_list;
    as_hashmap *inner_map;
    int inner_array_len, i;
    as_error err;

    arr_hash = Z_ARRVAL_PP(array);
    foreach_hashtable (arr_hash, pointer, data) {
        if (zend_hash_get_current_key_ex(arr_hash, &key, &arrkey_len, &index, 0, &pointer) == HASH_KEY_IS_STRING) {
            strcpy(map_key_type, "STRING");
        } else if (zend_hash_get_current_key_ex(arr_hash, &key, &arrkey_len, &index, 0, &pointer) == HASH_KEY_IS_LONG) {
            strcpy(map_key_type, "INT");
        } else {
            return FAILURE;
        }		

        switch (Z_TYPE_PP(data)) {
            case IS_LONG:
                if (!strcmp(map_key_type, "STRING")) {
                    as_stringmap_set_int64(map, key, Z_LVAL_PP(data));
                } else {
                    //TODO: Handle INT Key of map
                }
                break;

            case IS_STRING:
                if (!strcmp(map_key_type, "STRING")) {
                    as_stringmap_set_str(map, key, Z_STRVAL_PP(data));
                } else {
                    //TODO: Handle INT Key of map
                }
                break;

            case IS_ARRAY:
                inner_arr_hash = Z_ARRVAL_PP(data);
                zend_hash_internal_pointer_reset_ex(inner_arr_hash, &inner_pointer);

                if (zend_hash_get_current_key_ex(inner_arr_hash, &inner_key, &inner_arrkey_len, &inner_index, 0, &inner_pointer) == HASH_KEY_IS_STRING) {
                    inner_map = as_hashmap_new(32);
                    as_map *inner_m = (as_map *) inner_map;
                    handle_put_map(data, inner_m);
                    as_stringmap_set_map(map, key, inner_m);
                } else {
                    inner_array_len = zend_hash_num_elements(inner_arr_hash);
                    inner_list = as_arraylist_new(inner_array_len, 0);
                    handle_put_list(data, inner_list);
                    as_stringmap_set_list(map, key, (as_list *) inner_list);
                }
                break;
            default:
                /*
                 * For now, using AEROSPIKE_ERR. Need to have specific
                 * error code here.
                 */
                err.code = AEROSPIKE_ERR_PARAM;
                goto failure;
        }
    }
failure:
    /* TODO Any memory cleanup */
    fprintf(stderr, "error(%d) %s at [%s:%d]\n", err.code, err.message, err.file, err.line);
    return err.code;
}

#endif

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

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f*",
                             &func_call_info, &func_call_info_cache,
                             &func_call_info.params, &func_call_info.param_count) == FAILURE) {
        DEBUG_PHP_EXT_ERROR("invalid aerospike object");
        RETURN_FALSE;
    }

    if (as_log_set_callback(&aerospike_obj_p->as_p->log, &aerospike_helper_log_callback)) {
        Z_ADDREF_P(func_call_info.function_name);
        RETURN_TRUE;
    } else {
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
