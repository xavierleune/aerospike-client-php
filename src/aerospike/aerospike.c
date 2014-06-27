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
#include "aerospike/as_status.h"
#include "aerospike/as_val.h"
#include "aerospike/as_boolean.h"
#include "aerospike/as_arraylist.h"
#include "aerospike/as_hashmap.h"
#include "aerospike/as_stringmap.h"

#include "dbg.h"
#include <stdbool.h>

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


static zend_class_entry *Aerospike_ce, *AerospikeException_ce;

static zend_object_handlers Aerospike_handlers;

/*
as_val *
get_actual_type(const as_val *value)
{
	switch (as_val_type(value)) {
		case AS_UNDEF | AS_UNKNOWN:
                        return NULL;
		case AS_NIL:
			return NULL;
		case AS_BOOLEAN:
			return as_boolean_get((as_boolean *) value);
		case AS_INTEGER:
			return as_integer_get((as_integer *) value);
		case AS_STRING:
			return as_string_get((as_string *) value);
		case AS_LIST:
                        return (as_list *) value;
		case AS_MAP:
                        return (as_map *) value;
		case AS_REC:
			return (as_record *) value;
		case AS_PAIR:
			return (as_pair *) value;
		case AS_BYTES:
			return as_bytes_get((as_bytes *) value);
		default:
			return NULL;
	}
}*/

bool
callback_for_each_list_element(as_val *value, zval **list)
{
	//TODO: Yet to complete handling all cases
	zval *tmp;
	switch (as_val_type(value)) {
                case AS_UNDEF | AS_UNKNOWN:
			add_next_index_null(*list);
			break;
                case AS_NIL:
			add_next_index_null(*list);
			break;
                case AS_BOOLEAN:
			add_next_index_bool(*list, (int) as_boolean_get((as_boolean *) value));
			break;
                case AS_INTEGER:
			printf("\nGot int: %d\n", (long) as_integer_get((as_integer *) value));
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
			//TODO: Handle as_map
                case AS_REC:
			//TODO: Handle as_rec
                case AS_PAIR:
			//TODO: Handle as_pair
                case AS_BYTES:
			//TODO: Handle as_bytes
                default:
                    return false;
        }
	return true;
}

bool
callback_for_each_map_element(as_val *key, as_val *value, zval **arr)
{
	printf("\nIn map callback\n");
//	int key_type = as_val_type(key);
//	printf("\nGOT KEY TYPE: %s\n", key_type);

	switch (as_val_type(value)) {
		case AS_UNDEF | AS_UNKNOWN:
			//TODO: Handle undef/unknown
			break;
		case AS_NIL:
			//TODO: Handle nil
			break;
		case AS_BOOLEAN:
			//TODO: Handle boolean
			break;
		case AS_INTEGER:
			add_assoc_long(*arr, as_string_get((as_string *) key), (long) as_integer_get((as_integer *) value));
			break;
		case AS_STRING:
			add_assoc_stringl(*arr, as_string_get((as_string *) key), as_string_get((as_string *) value), strlen(as_string_get((as_string *) value)), 1);
			break;
		case AS_LIST:
			//TODO: Handle list
			break;
		case AS_MAP:
			//TODO: Handle map
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
			return false;
	}
	return true;
}

/**
 *  Appends a bin and value to input parameter bins_array
 *
 *  @param name      Bin name
 *  @param value     Bin value
 *  @param bins_array      Bin array to be appended to.
 *
 *  @return true if success. Otherwise false.
 */
bool 
update_bins_array(const char *name, const as_val *value, zval *bins_array)
{
	//TODO: Yet to complete all cases
	zval *tmp;
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
			printf("LIST ELEM 0: %d", as_list_get_int64((as_list *) value, 0));
			printf("LIST ELEM 1: %d", as_list_get_int64((as_list *) value, 1));
			printf("LIST ELEM 2: %s", as_list_get_str((as_list *) value, 2));
			as_list_foreach((as_list *) value, (as_list_foreach_callback) callback_for_each_list_element, &tmp);
			add_assoc_zval(bins_array, name, tmp);
			break;
		case AS_MAP:
			printf("\nGot Map!\n");
			MAKE_STD_ZVAL(tmp);
			printf("\na: %d\n", as_stringmap_get_int64((as_map *) value, "a"));
                        printf("\nb: %d\n", as_stringmap_get_int64((as_map *) value, "b"));
                        printf("\nc: %d\n", as_stringmap_get_int64((as_map *) value, "c"));
			array_init(tmp);
			as_map_foreach((as_map *) value, (as_map_foreach_callback) callback_for_each_map_element, &tmp);
			add_assoc_zval(bins_array, name, tmp);
			break;
		case AS_REC:
			//TODO: Handle as_rec
			//return (as_record *) value;
		case AS_PAIR:
			//TODO: Handle as_pair
			//return (as_pair *) value;
		case AS_BYTES:
			//TODO: Handle as_bytes
			//return as_bytes_get((as_bytes *) value);
		default:
			zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
			zend_throw_exception(AerospikeException_ce, ZEND_STRL("Aerospike::INVALID_BIN_VALUE_TYPE"), Z_LVAL(class_constant)  TSRMLS_CC);
			return false;
	}
	return true;
}

aerospike as;

typedef struct Aerospike_object {
	zend_object std;
	int value;
} Aerospike_object;

static void Aerospike_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	Aerospike_object *intern = (Aerospike_object *) object;
	zend_object_std_dtor(&(intern->std) TSRMLS_CC);
	efree(object);
	aerospike_destroy(&as);
	log_info("**DTOR**");
}

static void Aerospike_object_free_storage(void *object TSRMLS_DC)
{
	Aerospike_object *intern = (Aerospike_object *) object;

	if (!intern) {
		return;
	}

	zend_object_std_dtor(&intern->std TSRMLS_CC);
	efree(intern);
	printf("\nObject freed\n");
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


/* PHP Method:  aerospike::__construct()
   Constructs a new "aerospike" object. */
PHP_METHOD(Aerospike, __construct)
{
//	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);

	// DEBUG
	log_info("**In Aerospike::__construct() method**");

	// XXX -- Temporary implementation based on globals.
	zval *object = getThis();
	zval *host;
	char *arrkey;

	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &host) == FAILURE) {
		zval class_constant;
		zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
		zend_throw_exception(AerospikeException_ce, ZEND_STRL("Aerospike::INVALID_API_PARAM"), Z_LVAL(class_constant)  TSRMLS_CC);
		return;
	}
	// Errors populate this object.
	as_error err;

	// Configuration for the client.
	as_config config;
	as_config_init(&config);

	HashTable *keyindex = Z_ARRVAL_P(host);
	
	HashPosition pointer;
	zval **data , **host_array;
	uint arrkey_len;
	ulong index;
	zend_hash_internal_pointer_reset_ex(keyindex, &pointer);
	zend_hash_get_current_data_ex(keyindex, (void **) &data, &pointer);
	uint arrkey_type = zend_hash_get_current_key_ex(keyindex, &arrkey, &arrkey_len, &index, 0, &pointer);

	if (strcmp(arrkey, "hosts") == 0) {
		if (Z_TYPE_P(*data) != IS_ARRAY) {
			zval class_constant;
			zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
			zend_throw_exception(AerospikeException_ce, ZEND_STRL("Aerospike::INVALID_API_PARAM"), Z_LVAL(class_constant)  TSRMLS_CC);
			return;
		}
		host_array = data;
	} else {
		zval class_constant;
		zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
		zend_throw_exception(AerospikeException_ce, ZEND_STRL("Aerospike::INVALID_API_PARAM"), Z_LVAL(class_constant)  TSRMLS_CC);
		return;
	}		

	HashTable *hostindex = Z_ARRVAL_P(*host_array);
	HashPosition hostpointer;
	zval **hostdata;
	int i = 0;
	for (zend_hash_internal_pointer_reset_ex(hostindex, &hostpointer); zend_hash_get_current_data_ex(hostindex, (void **) &hostdata, &hostpointer) == SUCCESS; zend_hash_move_forward_ex(hostindex, &hostpointer)) {

		uint arrkey_len, arrkey_type;
		ulong index;
		
		arrkey_type = zend_hash_get_current_key_ex(hostindex, &arrkey, &arrkey_len, &index, 0, &hostpointer);

		if (Z_TYPE_P(*hostdata) != IS_ARRAY) {
			zval class_constant;
			zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
			zend_throw_exception(AerospikeException_ce, ZEND_STRL("Aerospike::INVALID_API_PARAM"), Z_LVAL(class_constant)  TSRMLS_CC);
			return;
		}

		HashTable *hostdataindex = Z_ARRVAL_P(*hostdata);
		HashPosition hostdatapointer;
		zval **hostdatavalue;
		for (zend_hash_internal_pointer_reset_ex(hostdataindex, &hostdatapointer); zend_hash_get_current_data_ex(hostdataindex, (void **) &hostdatavalue, &hostdatapointer) == SUCCESS; zend_hash_move_forward_ex(hostdataindex, &hostdatapointer)) {
			uint arrkey_len, arrkey_type;
			ulong index;
			
			arrkey_type = zend_hash_get_current_key_ex(hostdataindex, &arrkey, &arrkey_len, &index, 0, &hostdatapointer);

			if (strcmp(arrkey, "name") == 0) {
				zval class_constant;
				switch (Z_TYPE_P(*hostdatavalue)) {
					case IS_STRING:
						config.hosts[i].addr = Z_STRVAL_PP(hostdatavalue);
						break;
					default:
						zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
						zend_throw_exception(AerospikeException_ce, "Aerospike::INVALID_HOST_ADDRESS_TYPE", Z_LVAL(class_constant)  TSRMLS_CC);
						return;
				}
			} else if (strcmp(arrkey, "port") == 0) {
				switch (Z_TYPE_P(*hostdatavalue)) {
					zval class_constant;
					case IS_LONG:
						config.hosts[i].port = Z_LVAL_PP(hostdatavalue);
						break;
					default:
						zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
						zend_throw_exception(AerospikeException_ce, "Aerospike::INVALID_HOST_PORT_TYPE", Z_LVAL(class_constant) TSRMLS_CC);
						return;
				}
			} else {
				zval class_constant;
				zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
				zend_throw_exception(AerospikeException_ce, ZEND_STRL("Aerospike::INVALID_API_PARAM"), Z_LVAL(class_constant)  TSRMLS_CC);
				return;
			}
		}
		if (config.hosts[i].addr == NULL) {
			zval class_constant;
			zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);                                    
			zend_throw_exception(AerospikeException_ce, "Aerospike::NO_HOST_ADDRESS_FOUND_IN_HOST_ARRAY", Z_LVAL(class_constant) TSRMLS_CC);
			return;
		}
		if (! config.hosts[i].port) {
			zval class_constant;
			zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);                        
			zend_throw_exception(AerospikeException_ce, "Aerospike::NO_HOST_PORT_FOUND_IN_HOST_ARRAY", Z_LVAL(class_constant) TSRMLS_CC);
			return;
		}
		i++;
	}
	// XXX -- Should process constructor arguments here.

	// XXX -- For now, if none are supplied, default to localhost.

	// Add a seed host for cluster discovery.
	
#if 0
	// XXX -- This doesn't work with the current build options.
	config.hosts[0] = { .addr = "127.0.0.1", .port = 3000 };
#else
	//config.hosts[0].addr = "127.0.0.1";

	// TODO: get host address and port as input parameter
	//config.hosts[0].addr = "10.71.72.49";
	//config.hosts[0].port = 3000;
#endif

	// The Aerospike client instance, initialized with the configuration.
	if(i == 0) {
		zval class_constant;
		zend_get_constant_ex(ZEND_STRL("Aerospike::NO_HOSTS"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
		zend_throw_exception(AerospikeException_ce, ZEND_STRL("Aerospike::NO_HOSTS"), Z_LVAL(class_constant)  TSRMLS_CC);
		return;
	}
	aerospike_init(&as, &config);

	// Connect to the cluster.
	if (aerospike_connect(&as, &err) != AEROSPIKE_OK) {
		// An error occurred, so we log it.
		fprintf(stderr, "error(%d) %s at [%s:%d]\n", err.code, err.message, err.file, err.line);
		// XXX -- Release any allocated resources and throw and exception.
		RETURN_LONG(err.code);
	} else {
		fprintf(stderr, "It worked!\n");
		zval class_constant;
		zend_get_constant_ex(ZEND_STRL("Aerospike::OK"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
		RETURN_LONG(Z_LVAL(class_constant));
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
	log_info("**In Aerospike::__destruct() method**");

	// XXX -- Temporary implementation based on globals.

	// Cleanup the resources used by the client
	aerospike_destroy(&as);

	/*** TO BE IMPLEMENTED ***/

	zval class_constant;
	zend_get_constant_ex(ZEND_STRL("Aerospike::OK"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
	RETURN_LONG(Z_LVAL(class_constant));
}

/* PHP Method:  bool Aerospike::get()
   Read record header(s) and bin(s) for specified key(s) in one batch call. */
PHP_METHOD(Aerospike, get)
{
	zval *record_identifier, **record_key = NULL, *record;
	char *arrkey, *namespace = NULL, *set = NULL;
	zval *bins;
	as_record *rec = NULL;

	// DEBUG
	log_info("**In Aerospike::get() method**\n");

	zval *object = getThis();
	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "za|a", &record_identifier, &record, &bins) == FAILURE) {
		zval class_constant;
		zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
		zend_throw_exception(AerospikeException_ce, ZEND_STRL("Aerospike::INVALID_API_PARAM"), Z_LVAL(class_constant)  TSRMLS_CC);
		return;
	}

	// Errors populate this object.
	as_error err;
	as_key key;

	HashTable *keyindex = Z_ARRVAL_P(record_identifier);
	HashPosition pointer;
	zval **data;

	for (zend_hash_internal_pointer_reset_ex(keyindex, &pointer); zend_hash_get_current_data_ex(keyindex, (void **) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(keyindex, &pointer)) {
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
			zval class_constant;
			zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
			zend_throw_exception(AerospikeException_ce, ZEND_STRL("Aerospike::INVALID_API_PARAM"), Z_LVAL(class_constant) TSRMLS_CC);
			return;
		}
	}

	if (namespace == NULL) {
		zval class_constant;
		zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
		zend_throw_exception(AerospikeException_ce, "Aerospike::NO_NAMESPACE", Z_LVAL(class_constant) TSRMLS_CC);
		return;
	} 
	if (set == NULL) {
		zval class_constant;
		zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
		zend_throw_exception(AerospikeException_ce, "Aerospike::NO_SET", Z_LVAL(class_constant) TSRMLS_CC);
                return;
	} 
	if (record_key == NULL) {
		zval class_constant;
		zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
		zend_throw_exception(AerospikeException_ce, "Aerospike::NO_KEY", Z_LVAL(class_constant) TSRMLS_CC);
                return;
        }
	
	switch (Z_TYPE_PP(record_key)) {
		zval class_constant;
		case IS_LONG:
			as_key_init_int64(&key, namespace, set, (int64_t) Z_LVAL_PP(record_key));
			break;
		case IS_STRING:
			as_key_init_str(&key, namespace, set, (char *) Z_STRVAL_PP(record_key));
			break;
		default:
			zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
			zend_throw_exception(AerospikeException_ce, "Aerospike::INVALID_KEY_TYPE", Z_LVAL(class_constant) TSRMLS_CC);
			return;
	}
 
	if (ZEND_NUM_ARGS() == 2) {
		if (aerospike_key_get(&as, &err, NULL, &key, &rec) != AEROSPIKE_OK) {
			// An error occurred, so we log it.
			fprintf(stderr, "error(%d) %s at [%s:%d]\n", err.code, err.message, err.file, err.line);
			//log_err(err.message, );
			// XXX -- Release any allocated resources and throw and exception.
			as_record_destroy(rec);
			//zval class_constant;
			//zend_get_constant_ex(ZEND_STRL("Aerospike::KEY_NOT_FOUND_ERROR"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
			RETURN_LONG(err.code);
		} else {
			// rec contains record, hence process it and add bins of the record to input array record
			if (as_record_foreach(rec, (as_rec_foreach_callback) update_bins_array, record)) {
				zval class_constant;
				zend_get_constant_ex(ZEND_STRL("Aerospike::OK"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
				RETURN_LONG(Z_LVAL(class_constant));
			} else {
				zval class_constant;
                                zend_get_constant_ex(ZEND_STRL("Aerospike::SERVER_ERROR"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
                                RETURN_LONG(Z_LVAL(class_constant));
			}
		}
	} else if (ZEND_NUM_ARGS() == 3) {
		HashTable *bins_array =  Z_ARRVAL_P(bins);
		int bins_count = zend_hash_num_elements(bins_array);
		const char *select[bins_count];
		zval **bin_names;
		uint i=0;

		for (zend_hash_internal_pointer_reset_ex(bins_array, &pointer); zend_hash_get_current_data_ex(bins_array, (void **) &bin_names, &pointer) == SUCCESS; zend_hash_move_forward_ex(bins_array, &pointer)) {
			switch (Z_TYPE_PP(bin_names)) {
				zval class_constant;
				case IS_STRING:
					select[i++] = Z_STRVAL_PP(bin_names);
					break;
				default:
					zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);	
					zend_throw_exception(AerospikeException_ce, "Aerospike::INVALID_BIN_NAME_TYPE", Z_LVAL(class_constant) TSRMLS_CC);
					return;
				}
		}

		select[bins_count] = NULL;
		if (aerospike_key_select(&as, &err, NULL, &key, select, &rec) != AEROSPIKE_OK) {
			// An error occurred, so we log it.
			fprintf(stderr, "error(%d) %s at [%s:%d]\n", err.code, err.message, err.file, err.line);
			//log_err(err.message, );
			// XXX -- Release any allocated resources and throw and exception.
			as_record_destroy(rec);
			RETURN_LONG(err.code);
		} else {
			if (as_record_foreach(rec, (as_rec_foreach_callback) update_bins_array, record)) {
				zval class_constant;
				zend_get_constant_ex(ZEND_STRL("Aerospike::OK"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
				RETURN_LONG(Z_LVAL(class_constant));
			} else {
				zval class_constant;
				zend_get_constant_ex(ZEND_STRL("Aerospike::SERVER_ERROR"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
				RETURN_LONG(Z_LVAL(class_constant));
			}
		}
	}
	as_record_destroy(rec);
}


/* PHP Method:  bool Aerospike::put()
   Write record bin(s). */
PHP_METHOD(Aerospike, put)
{
	// DEBUG
	log_info("**In Aerospike::put() method**");

	zval *object = getThis();
	zval *record_identifier, *record, **record_key = NULL, **bin_value;
	char *bin_name , *arrkey, *namespace = NULL, *set = NULL;
	as_record rec;

	Aerospike_object *intern = (Aerospike_object *) zend_object_store_get_object(object TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "aa", &record_identifier, &record) == FAILURE) {
		zval class_constant;
		zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
		zend_throw_exception(AerospikeException_ce, "Aerospike::INVALID_API_PARAM", Z_LVAL(class_constant)  TSRMLS_CC);
                return;
	}

	HashTable *keyindex = Z_ARRVAL_P(record_identifier);
	HashPosition pointer;
	zval **data;

	for (zend_hash_internal_pointer_reset_ex(keyindex, &pointer); zend_hash_get_current_data_ex(keyindex, (void **) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(keyindex, &pointer)) {
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
			zval class_constant;
			zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
			zend_throw_exception(AerospikeException_ce, "Aerospike::INVALID_API_PARAM", Z_LVAL(class_constant)  TSRMLS_CC);
			return;
		}
	}

	if (namespace == NULL) {
		zval class_constant;
		zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
		zend_throw_exception(AerospikeException_ce, "Aerospike::NO_NAMESPACE", Z_LVAL(class_constant)  TSRMLS_CC);
		return;
	}
	if (set == NULL) {
		zval class_constant;
		zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
		zend_throw_exception(AerospikeException_ce, "Aerospike::NO_SET", Z_LVAL(class_constant)  TSRMLS_CC);
		return;
	}
	if (record_key == NULL) {
		zval class_constant;
		zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
		zend_throw_exception(AerospikeException_ce, "Aerospike::NO_KEY", Z_LVAL(class_constant)  TSRMLS_CC);
		return;
	}

	HashTable *record_index = Z_ARRVAL_P(record);
	zval **dataval;
	int array_count;
	
	array_count = zend_hash_num_elements(record_index);

	as_record_inita(&rec, array_count);

	for (zend_hash_internal_pointer_reset_ex(record_index, &pointer); zend_hash_get_current_data_ex(record_index, (void **) &dataval, &pointer) == SUCCESS; zend_hash_move_forward_ex(record_index, &pointer)) {
		uint arrkey_len, arrkey_type;
		ulong index;

		arrkey_type = zend_hash_get_current_key_ex(record_index, &arrkey, &arrkey_len, &index, 0, &pointer);

		bin_name = arrkey;
		bin_value = dataval;
		int status = 0;

		/*
		if (status = as_put_value(record_key, bin_name, bin_value, &rec, namespace, set) != SUCCESS) {
			RETURN_LONG(status);
		}*/

		switch (Z_TYPE_P(*bin_value)) {
			zval class_constant;
			HashTable *arr_hash;
			int array_len;
			HashPosition pointer;
			zval **data;
			as_arraylist *list;
			as_hashmap *map;
						
			//uint arrkey_len, arrkey_type;
			char *key;
			//ulong index;

			printf("BIN TYPE: %d\n", Z_TYPE_PP(bin_value));
			case IS_LONG:
				as_record_set_int64(&rec, bin_name, (int64_t) Z_LVAL_P(*bin_value));
				break;
			case IS_STRING:
				as_record_set_str(&rec, bin_name, (char *) Z_STRVAL_P(*bin_value));
				break;
			case IS_ARRAY:
				arr_hash = Z_ARRVAL_P(*bin_value);
				array_len = zend_hash_num_elements(arr_hash);
				printf("Got array of len: %d\n", array_len);
				//as_arraylist_init(&list, array_len, 0);
				//as_map map;
				//as_stringmap_init(&map);
				
				zend_hash_internal_pointer_reset_ex(arr_hash, &pointer);
				zend_hash_get_current_data_ex(arr_hash, (void **) &data, &pointer);
				
				if (zend_hash_get_current_key_ex(arr_hash, &key, &arrkey_len, &index, 0, &pointer) == HASH_KEY_IS_STRING) {
					map = as_hashmap_new(32);
					as_map *m = (as_map *) map;
					/*as_stringmap_set_int64(m, "a", 1);
					as_stringmap_set_int64(m, "b", 2);
					as_stringmap_set_int64(m, "c", 3);*/

					handle_put_map(bin_value, m);
					as_record_set_map(&rec, bin_name, m);
					as_map *m1 = as_record_get_map(&rec, "bin1");
					printf("\nname: %s\n", as_stringmap_get_str(m1, "name"));
					printf("\nage: %d\n", as_stringmap_get_int64(m1, "age"));
					//printf("\nc: %d\n", as_stringmap_get_int64(m1, "c"));					
				} else {
					/*
					for (zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); zend_hash_get_current_data_ex(arr_hash, (void **) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(arr_hash, &pointer)) {				
						printf("ELEM TYPE: %d\n", Z_TYPE_PP(data));
						switch (Z_TYPE_PP(data)) {
							case IS_LONG:
								printf("GOT LONG: %d\n", Z_LVAL_PP(data));
								as_arraylist_append_int64(&list, Z_LVAL_PP(data));
								break;
							case IS_STRING:
								printf("GOT STRING: %s\n", Z_LVAL_PP(data));
								as_arraylist_append_str(&list, Z_STRVAL_PP(data));
								break;
							default:
								zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);	
								zend_throw_exception(AerospikeException_ce, "Aerospike::INVALID_BIN_NAME_TYPE", Z_LVAL(class_constant) TSRMLS_CC);
								return;
							}
					}
	
					as_record_set_list(&rec, bin_name, (as_list *) &list);
					as_val *r = (as_val *) as_record_get(&rec, "bin3");
					printf("BIN type: %d", as_val_type(r));*/
		
					array_len = zend_hash_num_elements(arr_hash);
					list = as_arraylist_new(array_len, 0);
					handle_put_list(bin_value, list);
					as_record_set_list(&rec, bin_name, (as_list *) as_val_reserve(list));
					as_list *l1 = as_record_get_list(&rec, "bin3");
					int i=0;
					for (i=0; i<2; i++) {
						printf("%d\n", as_list_get_int64(l1, i));
					}
					printf("%s\n", as_list_get_str(l1, 2));
					as_list *inner = as_list_get_list(l1, 3);
					printf("\nInner\n");
					for (i=0; i<3; i++) {
                                                printf("%d\n", as_list_get_int64((as_list *) inner, i));
                                        }
					as_arraylist_destroy(list);
				}
				break;
			default:
				zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
				zend_throw_exception(AerospikeException_ce, "Aerospike::INVALID_BIN_VALUE_TYPE", Z_LVAL(class_constant) TSRMLS_CC);
				return;
			}
	}

	//as_val *r = (as_val *) as_record_get(rec, "bin2");
	//printf("BIN type: %d", as_val_type(r));
	
	as_list *l1 = as_record_get_list(&rec, "bin3");
	int i=0;
	for (i=0; i<3; i++) {
		printf("%d\n", as_list_get_int64(l1, i));
	}

	as_error err;
	as_key key;
	
	switch (Z_TYPE_P(*record_key)) {
		zval class_constant;
		case IS_LONG:
			as_key_init_int64(&key, namespace, set, (int64_t) Z_LVAL_P(*record_key));
			break;
		case IS_STRING:
			as_key_init_str(&key, namespace, set, (char *) Z_STRVAL_P(*record_key));
			break;
		default:
			zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
			zend_throw_exception(AerospikeException_ce, "Aerospike::INVALID_KEY_TYPE", Z_LVAL(class_constant) TSRMLS_CC);
        	return;
	}	

	if (aerospike_key_put(&as, &err, NULL, &key, &rec) != AEROSPIKE_OK) {
		// An error occurred, so we log it.
		fprintf(stderr, "error(%d) %s at [%s:%d]\n", err.code, err.message, err.file, err.line);
		//log_err(err.message, );
		// XXX -- Release any allocated resources and throw and exception.
		RETURN_LONG(err.code);
	} else {
		zval class_constant;
		zend_get_constant_ex(ZEND_STRL("Aerospike::OK"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
		RETURN_LONG(Z_LVAL(class_constant));
	}
}

int
handle_put_list(zval **bin_value, as_arraylist *list)
{
	HashTable *arr_hash;
	HashPosition pointer;
	zval **data;
	char *key;
	uint arrkey_len;
	zval class_constant;
	ulong index;

	arr_hash = Z_ARRVAL_P(*bin_value);
	printf("Got array\n");
	for (zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); zend_hash_get_current_data_ex(arr_hash, (void **) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(arr_hash, &pointer)) {
		printf("ELEM TYPE: %d\n", Z_TYPE_PP(data));
		switch (Z_TYPE_PP(data)) {
			as_arraylist *inner_list;
			HashTable *inner_arr_hash;
			int inner_array_len, i;
			case IS_LONG:
				printf("GOT LONG: %d\n", Z_LVAL_PP(data));
				as_arraylist_append_int64(list, Z_LVAL_PP(data));
				break;
			case IS_STRING:
				printf("GOT STRING: %s\n", Z_LVAL_PP(data));
				as_arraylist_append_str(list, Z_STRVAL_PP(data));
				break;
			case IS_ARRAY:
				printf("\nGOT ARRAY\n");
				inner_arr_hash = Z_ARRVAL_PP(data);
				inner_array_len = zend_hash_num_elements(inner_arr_hash);
				
				//as_arraylist_init(&inner_list, inner_array_len, 0);
				inner_list = as_arraylist_new(inner_array_len, 0);
				handle_put_list(data, inner_list);
				
				printf("\nHandled Inner and its contents are:\n");
                                for (i=0; i<3; i++) {
					printf("%d\n", as_list_get_int64((as_list *) inner_list, i));
				}
					
				as_arraylist_append_list(list, (as_list *) inner_list);
				as_list *l = as_list_get_list((as_list *) list, 3);
				for (i=0; i<3; i++) {
					printf("%d\n", as_list_get_int64((as_list *) l, i));
				}
				//XXX LEAK:
				//as_arraylist_destroy(inner_list);
				break;
			default:
				zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
				zend_throw_exception(AerospikeException_ce, "Aerospike::INVALID_BIN_NAME_TYPE", Z_LVAL(class_constant) TSRMLS_CC);
				return;
		}
	}
}

int
handle_put_map(zval **bin_value, as_map *map)
{
        HashTable *arr_hash;
        HashPosition pointer;
        zval **data;
        char *key, map_key_type[10];
        uint arrkey_len, arrkey_type;
        zval class_constant;
        ulong index;

        arr_hash = Z_ARRVAL_P(*bin_value);
        printf("Got array\n");
	for (zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); zend_hash_get_current_data_ex(arr_hash, (void **) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(arr_hash, &pointer)) {
		if (zend_hash_get_current_key_ex(arr_hash, &key, &arrkey_len, &index, 0, &pointer) == HASH_KEY_IS_STRING) {
			printf("String Key");
			strcpy(map_key_type, "STRING");
		} else if (zend_hash_get_current_key_ex(arr_hash, &key, &arrkey_len, &index, 0, &pointer) == HASH_KEY_IS_LONG) {
			printf("Int Key");
			strcpy(map_key_type, "INT");
		} else {
			printf("Invalid Key Type");
			return FAILURE;
		}		

		printf("KEY_TYPE: %s", map_key_type);

		printf("VALUE TYPE: %d\n", Z_TYPE_PP(data));
		switch (Z_TYPE_PP(data)) {
			as_arraylist inner_list;
			HashTable *inner_arr_hash;
			int inner_array_len, i;

			case IS_LONG:
				printf("GOT LONG: %d\n", Z_LVAL_PP(data));
				if (!strcmp(map_key_type, "STRING")) {
					as_stringmap_set_int64(map, key, Z_LVAL_PP(data));
				} else {
					//TODO: Handle INT Key of map
				}
				break;
			
			case IS_STRING:
				printf("GOT STRING: %s\n", Z_LVAL_PP(data));
				if (!strcmp(map_key_type, "STRING")) {
					as_stringmap_set_str(map, key, Z_STRVAL_PP(data));
				} else {
					//TODO: Handle INT Key of map                                                                        
				}                                     
				break;
			/*
			case IS_ARRAY:
				printf("\nGOT ARRAY\n");
				inner_arr_hash = Z_ARRVAL_PP(data);
				inner_array_len = zend_hash_num_elements(inner_arr_hash);
				as_arraylist_init(&inner_list, inner_array_len, 0);
				handle_put_array(data, &inner_list);

				printf("\nHandled Inner and its contents are:\n");
				for (i=0; i<3; i++) {
					printf("%d\n", as_list_get_int64((as_list *) &inner_list, i));
				}
				as_arraylist_append_list(list, (as_list *) &inner_list);
				as_list *l = as_list_get_list((as_list *) list, 3);
				for (i=0; i<3; i++) {
					printf("%d\n", as_list_get_int64((as_list *) l, i));
				}
				break;*/
			default:
				zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
				zend_throw_exception(AerospikeException_ce, "Aerospike::INVALID_BIN_NAME_TYPE", Z_LVAL(class_constant) TSRMLS_CC);
				return;
		}
	}
}

/**
 *  return int
 *
 *  @param record_key		record key.
 *  @param bin_name 		name of bin.
 *  @param bin_value 		value of bin.
 *  @param as_record rec 	struct.
 *  @param namespace 		namespace.
 *  @param set 			set.
 *
 *  @return SUCCESS for SUCCESS. Otherwise FAILURE.
 */
int
as_put_value(zval **record_key, char *bin_name, zval **bin_value, as_record *rec, char *namespace, char *set)
{
	switch (Z_TYPE_P(*bin_value)) {
		zval class_constant;
		HashTable *arr_hash;
		int array_len;
		HashPosition pointer;
		zval **data;
		as_arraylist list; 
		uint arrkey_len, arrkey_type;
		char *key;
		ulong index;

		case IS_LONG:
			as_record_set_int64(rec, bin_name, (int64_t) Z_LVAL_P(*bin_value));
			break;
		case IS_STRING:
			as_record_set_str(rec, bin_name, (char *) Z_STRVAL_P(*bin_value));
			break;
		case IS_ARRAY:
			arr_hash = Z_ARRVAL_P(*bin_value);
			array_len = zend_hash_num_elements(arr_hash);
			printf("Got array\n");
			as_arraylist_init(&list, array_len, 0);
			//as_map map;
			//as_stringmap_init(&map);
			zend_hash_internal_pointer_reset_ex(arr_hash, &pointer);
			zend_hash_get_current_data_ex(arr_hash, (void **) &data, &pointer);
			
			if (zend_hash_get_current_key_ex(arr_hash, &key, &arrkey_len, &index, 0, &pointer) == HASH_KEY_IS_STRING) {
				//TODO: Handle map				
			} else {
				for (zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); zend_hash_get_current_data_ex(arr_hash, (void **) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(arr_hash, &pointer)) {				
					printf("ELEM TYPE: %d\n", Z_TYPE_PP(data));
					switch (Z_TYPE_PP(data)) {
						case IS_LONG:
							printf("GOT LONG: %d\n", Z_LVAL_PP(data));
							as_arraylist_append_int64(&list, Z_LVAL_PP(data));
						case IS_STRING:
							//select[i++] = Z_STRVAL_PP(bin_names);
							break;
						default:
							zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);	
							zend_throw_exception(AerospikeException_ce, "Aerospike::INVALID_BIN_NAME_TYPE", Z_LVAL(class_constant) TSRMLS_CC);
							return;
						}
				}	
				as_record_set_list(rec, bin_name, (as_list *) &list);
				as_val *r = (as_val *) as_record_get(rec, "bin3");
				printf("BIN type: %d", as_val_type(r));
				as_list *l1 = as_record_get_list(rec, "bin3");
				int i=0;
				for (i=0; i<3; i++) {
					printf("%d\n", as_list_get_int64(l1, i));
				}
			}			
			break;
		default:
			zend_get_constant_ex(ZEND_STRL("Aerospike::INVALID_API_PARAM"), &class_constant, Aerospike_ce, 0 TSRMLS_DC);
			zend_throw_exception(AerospikeException_ce, "Aerospike::INVALID_BIN_VALUE_TYPE", Z_LVAL(class_constant) TSRMLS_CC);
            return;
	}	
	return SUCCESS;
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
	log_info("**In Aerospike::isConnected() method**\n");

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
	log_info("**In Aerospike::close() method**\n");

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
	log_info("**In aerospike minit**\n");

	REGISTER_INI_ENTRIES();

	ZEND_INIT_MODULE_GLOBALS(aerospike, aerospike_globals_ctor, aerospike_globals_dtor);

	zend_class_entry ce;
	zend_class_entry *exception_ce = (zend_class_entry *) zend_exception_get_default();

	INIT_CLASS_ENTRY(ce, "AerospikeException", NULL);
	if (!(AerospikeException_ce = zend_register_internal_class_ex(&ce, exception_ce, NULL TSRMLS_CC))) {
		return FAILURE;
	}

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

	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("COMMAND_REJECTED"), -8 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("QUERY_TERMINATED"), -7 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("SCAN_TERMINATED"), -6 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("NO_HOSTS"), -5 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("INVALID_API_PARAM"), -4 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("FAIL_ASYNCQ_FULL"), -3 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("FAIL_TIMEOUT"), -2 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("FAIL_CLIENT"), -1 TSRMLS_CC);

	// Server status codes:

	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("OK"), 0 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("SERVER_ERROR"), 1 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("KEY_NOT_FOUND_ERROR"), 2 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("GENERATION_ERROR"), 3 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("PARAMETER_ERROR"), 4 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("KEY_FOUND_ERROR"), 5 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("BIN_FOUND_ERROR"), 6 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("CLUSTER_KEY_MISMATCH"), 7 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("PARTITION_OUT_OF_SPACE"), 8 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("SERVERSIDE_TIMEOUT"), 9 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("NO_XDR"), 10 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("SERVER_UNAVAILABLE"), 11 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("INCOMPATIBLE_TYPE"), 12 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("RECORD_TOO_BIG"), 13 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("KEY_BUSY"), 14 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("SCAN_ABORT"), 15 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("UNSUPPORTED_FEATURE"), 16 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("BIN_NOT_FOUND"), 17 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("DEVICE_OVERLOAD"), 18 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("KEY_MISMATCH"), 19 TSRMLS_CC);

	// UDF status codes:

	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("UDF_BAD_RESPONSE"), 100 TSRMLS_CC);

	// Secondary Index status codes:

	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("INDEX_FOUND"), 200 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("INDEX_NOTFOUND"), 201 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("INDEX_OOM"), 202 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("INDEX_NOTREADABLE"), 203 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("INDEX_GENERIC"), 204 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("NAME_MAXLEN"), 205 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("MAXCOUNT"), 206 TSRMLS_CC);

	// Query statue codes:

	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("ABORTED"), 210 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("QUERY_QUEUEFULL"), 211 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("QUERY_TIMEOUT"), 212 TSRMLS_CC);
	zend_declare_class_constant_long(Aerospike_ce, ZEND_STRL("QUERY_GENERIC"), 213 TSRMLS_CC);

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
