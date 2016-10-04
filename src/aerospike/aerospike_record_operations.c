/*
 *
 * Copyright (C) 2014-2016 Aerospike, Inc.
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

#include "php.h"
#include "ext/standard/php_var.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_error.h"
#include "aerospike/as_record.h"
#include "aerospike/as_boolean.h"
#include "string.h"
#include "aerospike_common.h"
#include "aerospike_policy.h"
#include "aerospike_general_constants.h"
#include "aerospike_transform.h"

extern bool operater_ordered_callback(const char *key, const as_val *value, void *array TSRMLS_DC)
{
	static int iterator = 0;
	as_error err;
	DECLARE_ZVAL(record_local_p);

#if PHP_VERSION_ID < 70000
	MAKE_STD_ZVAL(record_local_p);
#else
	zval *temp;
#endif

	array_init(AEROSPIKE_ZVAL_ARG(record_local_p));
	if (key) {
		if (0 != AEROSPIKE_ADD_NEXT_INDEX_STRINGL(AEROSPIKE_ZVAL_ARG(record_local_p), key, strlen(key), 1)) {
			DEBUG_PHP_EXT_DEBUG("Unable to get the record key.");
			return false;
		}
	}

	if (value) {
		switch(value->type) {
			case AS_STRING:
				if (0 != AEROSPIKE_ADD_NEXT_INDEX_STRINGL(AEROSPIKE_ZVAL_ARG(record_local_p), as_string_get((as_string *) value),
					strlen(as_string_get((as_string *) value)), 1)) {
					DEBUG_PHP_EXT_DEBUG("Unable to get the record.");
					return false;
				}
				break;

			case AS_INTEGER:
				if (0 != add_next_index_long(AEROSPIKE_ZVAL_ARG(record_local_p), (long) as_integer_get((as_integer *) value))) {
					DEBUG_PHP_EXT_DEBUG("Unable to get the record.");
					return false;
				}
				break;

			case AS_BOOLEAN:
				if (0 != add_next_index_bool(AEROSPIKE_ZVAL_ARG(record_local_p), (int) as_boolean_get((as_boolean *)value))) {
					DEBUG_PHP_EXT_DEBUG("Unable to get the record.");
					return false;
				}

			case AS_BYTES:
				ADD_LIST_APPEND_BYTES(NULL, NULL, (void *)value, &record_local_p, &err TSRMLS_CC);
				if (err.code != AEROSPIKE_OK) {
					DEBUG_PHP_EXT_DEBUG("Unable to get the record.");
					return false;
				}
				break;

			case AS_UNDEF:
			case AS_NIL:
				ADD_LIST_APPEND_NULL(NULL, NULL, (void *)value, &record_local_p, &err TSRMLS_CC);
				if (err.code != AEROSPIKE_OK) {
					DEBUG_PHP_EXT_DEBUG("Unable to get the record.");
					return false;
				}
				break;

			case AS_DOUBLE:
				ADD_LIST_APPEND_DOUBLE(NULL, NULL, (void *)value, /*&record_local_p*/ (void *) array, &err TSRMLS_CC);
				if (err.code != AEROSPIKE_OK) {
					DEBUG_PHP_EXT_DEBUG("Unable to get the record.");
					return false;
				}
				break;

			case AS_LIST:
#if PHP_VERSION_ID < 70000
				ADD_LIST_APPEND_LIST(NULL, (void *)key, (void *)value, &record_local_p, &err TSRMLS_CC);
#else
				temp = &record_local_p;
				ADD_LIST_APPEND_LIST(NULL, (void *)key, (void *)value, &temp, &err TSRMLS_CC);
#endif
				if (err.code != AEROSPIKE_OK) {
					DEBUG_PHP_EXT_DEBUG("Unable to get the record.");
					return false;
				}
				break;

			case AS_MAP:
#if PHP_VERSION_ID < 70000
				ADD_LIST_APPEND_MAP(NULL, NULL, (void *)value, &record_local_p, &err TSRMLS_CC);
#else
				temp = &record_local_p;
				ADD_LIST_APPEND_MAP(NULL, NULL, (void *)value, &temp, &err TSRMLS_CC);
#endif
				if (err.code != AEROSPIKE_OK) {
					DEBUG_PHP_EXT_DEBUG("Unable to get the record.");
					return false;
				}
				break;

			case AS_REC:
#if PHP_VERSION_ID < 70000
				ADD_LIST_APPEND_REC(NULL, NULL, (void *)value, &record_local_p, &err TSRMLS_CC);
#else
				temp = &record_local_p;
				ADD_LIST_APPEND_REC(NULL, NULL, (void *)value, &temp, &err TSRMLS_CC);
#endif
				if (err.code != AEROSPIKE_OK) {
					DEBUG_PHP_EXT_DEBUG("Unable to get the record.");
					return false;
				}
				break;

			case AS_PAIR:
#if PHP_VERSION_ID < 70000
				ADD_LIST_APPEND_PAIR(NULL, NULL, (void *)value, &record_local_p, &err TSRMLS_CC);
#else
				temp = &record_local_p;
				ADD_LIST_APPEND_PAIR(NULL, NULL, (void *)value, &temp, &err TSRMLS_CC);
#endif
				if (err.code != AEROSPIKE_OK) {
					DEBUG_PHP_EXT_DEBUG("Unable to get the record.");
					return false;
				}
				break;

			case AS_GEOJSON:
				// TODO Handle this
				break;

			case AS_VAL_T_MAX:
				// TODO Handle this
				break;
		}
	} else {
		if (0 != add_next_index_null(AEROSPIKE_ZVAL_ARG(record_local_p))) {
			DEBUG_PHP_EXT_DEBUG("Unable to get the record.");
			return false;
		}
	}
	if(0 != add_index_zval(((foreach_callback_udata *) array)->udata_p, iterator, AEROSPIKE_ZVAL_ARG(record_local_p))) {
		DEBUG_PHP_EXT_DEBUG("Unable to get the record.");
		return false;
	}
	iterator++;

	return true;
}


/*
 *******************************************************************************************************
 * Wrapper function to perform an aerospike_key_oeprate within the C client.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param as_key_p              The C client's as_key that identifies the record.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param bin_name_p            The bin name to perform operation upon.
 * @param str                   The string to be appended in case of operation: append.
 * @param offset                The offset to be incremented by in case of operation: increment,
 *                              or to be initialized if record/bin does not already exist.
 * @param time_to_live          The ttl for the record in case of operation: touch.
 * @param operation             The operation type.
 *
 *******************************************************************************************************
 */
static as_status
aerospike_record_operations_ops(Aerospike_object *aerospike_obj_p,
								aerospike* as_object_p,
								as_key* as_key_p,
								zval* options_p,
								as_error* error_p,
								char* bin_name_p,
								char* str,
								char* geoStr,
								u_int64_t offset,
								double double_offset,
								uint64_t time_to_live,
								int64_t index,
								u_int64_t operation,
								as_operations* ops,
								#if PHP_VERSION_ID < 70000
									zval** each_operation
								#else
									zval*  each_operation
								#endif
								, as_policy_operate* operate_policy,
								int8_t serializer_policy,
								as_record** get_rec TSRMLS_DC)
{
	as_val*                value_p = NULL;
	const char             *select[] = {bin_name_p, NULL};

	DECLARE_ZVAL(temp_record_p);
	DECLARE_ZVAL(append_val_copy);

	as_record              record;
	as_static_pool         static_pool = {0};
	as_val                 *val = NULL;
	as_arraylist           args_list;
	as_arraylist*          args_list_p = NULL;
	as_static_pool         items_pool = {0};

	as_error_init(error_p);
	as_record_inita(&record, 1);

	switch (operation) {
		case AS_OPERATOR_APPEND:
			if (!as_operations_add_append_str(ops, bin_name_p, str)) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to append");
				DEBUG_PHP_EXT_DEBUG("Unable to append");
				goto exit;
			}
			break;
		case AS_OPERATOR_PREPEND:
			if (!as_operations_add_prepend_str(ops, bin_name_p, str)) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to prepend");
				DEBUG_PHP_EXT_DEBUG("Unable to prepend");
				goto exit;
			}
			break;
		case AS_OPERATOR_INCR:
			if (offset) {
				if (!as_operations_add_incr(ops, bin_name_p, offset)) {
					PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to increment");
					DEBUG_PHP_EXT_DEBUG("Unable to increment");
					goto exit;
				}
			} else {
				if (!as_operations_add_incr_double(ops, bin_name_p, double_offset)) {
					PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to increment");
					DEBUG_PHP_EXT_DEBUG("Unable to increment");
					goto exit;
				}
			}
			break;
		case AS_OPERATOR_TOUCH:
			ops->ttl = time_to_live;
			if (!as_operations_add_touch(ops)) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to touch");
				DEBUG_PHP_EXT_DEBUG("Unable to touch");
				goto exit;
			}
			break;
		case AS_OPERATOR_READ:
			if (!as_operations_add_read(ops, bin_name_p)) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to read");
				DEBUG_PHP_EXT_DEBUG("Unable to read");
				goto exit;
			}
			break;

		case AS_OPERATOR_WRITE:
			if (str) {
				if (!as_operations_add_write_str(ops, bin_name_p, str)) {
					PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to write");
					DEBUG_PHP_EXT_DEBUG("Unable to write");
					goto exit;
				}
			} else if (geoStr) {
				if (!as_operations_add_write_geojson_str(ops, bin_name_p, geoStr)) {
					PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to write");
					DEBUG_PHP_EXT_DEBUG("Unable to write");
					goto exit;
				}
			} else if (offset) {
				if (!as_operations_add_write_int64(ops, bin_name_p, offset)) {
					PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to write");
					DEBUG_PHP_EXT_DEBUG("Unable to write");
					goto exit;
				}
			} else {
				if (!as_operations_add_write_double(ops, bin_name_p, double_offset)) {
					PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to write");
					DEBUG_PHP_EXT_DEBUG("Unable to write");
					goto exit;
				}
			}
			break;

		case OP_LIST_APPEND:
		#if PHP_VERSION_ID < 70000
			MAKE_STD_ZVAL(temp_record_p);
			array_init(temp_record_p);
			ALLOC_ZVAL(append_val_copy);
			MAKE_COPY_ZVAL(each_operation, append_val_copy);
			add_assoc_zval(temp_record_p, bin_name_p, append_val_copy);
		#else
			array_init(&temp_record_p);
			add_assoc_zval(&temp_record_p, bin_name_p, each_operation);
		 #endif

			aerospike_transform_iterate_records(aerospike_obj_p, &temp_record_p, &record, &static_pool, serializer_policy, aerospike_has_double(as_object_p), error_p TSRMLS_CC);
			if (AEROSPIKE_OK != error_p->code) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Unable to parse the value parameter");
				DEBUG_PHP_EXT_ERROR("Unable to parse the value parameter");
				goto exit;
			}
			val = (as_val*) as_record_get(&record, bin_name_p);
			if (val) {
				if (!as_operations_add_list_append(ops, bin_name_p, val)) {
					PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to append");
					DEBUG_PHP_EXT_DEBUG("Unable to append");
					goto exit;
				}
			}
			break;

		case OP_LIST_INSERT:
		#if PHP_VERSION_ID < 70000
			MAKE_STD_ZVAL(temp_record_p);
			array_init(temp_record_p);
			ALLOC_ZVAL(append_val_copy);
			MAKE_COPY_ZVAL(each_operation, append_val_copy);
			add_assoc_zval(temp_record_p, bin_name_p, append_val_copy);
		#else
			array_init(&temp_record_p);
			add_assoc_zval(&temp_record_p, bin_name_p, each_operation);
		#endif

			aerospike_transform_iterate_records(aerospike_obj_p, &temp_record_p, &record, &static_pool, serializer_policy, aerospike_has_double(as_object_p), error_p TSRMLS_CC);
			if (AEROSPIKE_OK != error_p->code) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Unable to parse the value parameter");
				DEBUG_PHP_EXT_ERROR("Unable to parse the value parameter");
				goto exit;
			}
			val = (as_val*) as_record_get(&record, bin_name_p);
			if (val) {
				if (!as_operations_add_list_insert(ops, bin_name_p, index, val)) {
					PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to insert");
					DEBUG_PHP_EXT_DEBUG("Unable to insert");
					goto exit;
				}
			}
			break;

		case OP_LIST_INSERT_ITEMS:
			if (AEROSPIKE_Z_TYPE_P(each_operation) != IS_ARRAY) {
				DEBUG_PHP_EXT_DEBUG("Value passed if not array type.");
				goto exit;
			}
		 	as_arraylist_inita(&args_list, zend_hash_num_elements(AEROSPIKE_Z_ARRVAL_P(each_operation)));
			args_list_p = &args_list;

			AS_LIST_PUT(aerospike_obj_p, NULL, each_operation, args_list_p, &items_pool, serializer_policy,
					error_p TSRMLS_CC);

			if (error_p->code == AEROSPIKE_OK) {
				if (!as_operations_add_list_append_items(ops, bin_name_p, (as_list*)args_list_p)) {
					PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to insert items.");
					DEBUG_PHP_EXT_DEBUG("Unable to insert items.");
					goto exit;
				}
			}
			break;

		case OP_LIST_POP:
			if (!as_operations_add_list_pop(ops, bin_name_p, index)) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to pop.");
				DEBUG_PHP_EXT_DEBUG("Unable to pop.");
				goto exit;
			}
			break;

		case OP_LIST_POP_RANGE:
			if (!as_operations_add_list_pop_range(ops, bin_name_p, index, offset)) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to pop range.");
				DEBUG_PHP_EXT_DEBUG("Unable to pop range.");
				goto exit;
			}
			break;

		case OP_LIST_REMOVE:
			if (!as_operations_add_list_remove(ops, bin_name_p, index)) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to remove.");
				DEBUG_PHP_EXT_DEBUG("Unable to remove.");
				goto exit;
			}
			break;

		case OP_LIST_REMOVE_RANGE:
			if (!as_operations_add_list_remove_range(ops, bin_name_p, index, offset)) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to remove range.");
				DEBUG_PHP_EXT_DEBUG("Unable to remove range.");
				goto exit;
			}
			break;

		case OP_LIST_CLEAR:
			if (!as_operations_add_list_clear(ops, bin_name_p)) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to clear.");
				DEBUG_PHP_EXT_DEBUG("Unable to clear.");
				goto exit;
			}
			break;

		case OP_LIST_SET:
		 #if PHP_VERSION_ID < 70000
			 MAKE_STD_ZVAL(temp_record_p);
			 array_init(temp_record_p);
			 ALLOC_ZVAL(append_val_copy);
			 MAKE_COPY_ZVAL(each_operation, append_val_copy);
			 add_assoc_zval(temp_record_p, bin_name_p, append_val_copy);
		 #else
			 array_init(&temp_record_p);
			 add_assoc_zval(&temp_record_p, bin_name_p, each_operation);
		 #endif

			aerospike_transform_iterate_records(aerospike_obj_p, &temp_record_p, &record, &static_pool, serializer_policy, aerospike_has_double(as_object_p), error_p TSRMLS_CC);
			if (AEROSPIKE_OK != error_p->code) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Unable to parse the value parameter");
				DEBUG_PHP_EXT_ERROR("Unable to parse the value parameter");
				goto exit;
			}
			val = (as_val*) as_record_get(&record, bin_name_p);
			if (val) {
				if (!as_operations_add_list_set(ops, bin_name_p, index, val)){
					PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to set.");
					DEBUG_PHP_EXT_DEBUG("Unable to set.");
					goto exit;
				}
			}
			break;

		case OP_LIST_GET:
			if (!as_operations_add_list_get(ops, bin_name_p, index)) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to get.");
				DEBUG_PHP_EXT_DEBUG("Unable to get.");
				goto exit;
			}
			break;

		case OP_LIST_GET_RANGE:
			if (!as_operations_add_list_get_range(ops, bin_name_p, index, offset)) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to get range.");
				DEBUG_PHP_EXT_DEBUG("Unable to  get range.");
				goto exit;
			}
			break;

		case OP_LIST_TRIM:
			if (!as_operations_add_list_trim(ops, bin_name_p, index, offset)) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to trim.");
				DEBUG_PHP_EXT_DEBUG("Unable to trim.");
				goto exit;
			}
			break;

		case OP_LIST_SIZE:
			if (!as_operations_add_list_size(ops, bin_name_p)) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to get size.");
				DEBUG_PHP_EXT_DEBUG("Unable to get size.");
				goto exit;
			}
			break;

		default:
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Invalid operation");
			DEBUG_PHP_EXT_DEBUG("Invalid operation");
			goto exit;
	}


exit:

	if (args_list_p) {
		as_arraylist_destroy(args_list_p);
	}

	return error_p->code;
}

/*
 *******************************************************************************************************
 * Wrapper function to perform an aerospike_key_exists within the C client.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param as_key_p              The C client's as_key that identifies the record.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param metadata_p            The return metadata for the exists/getMetadata API to be
 *                              populated by this function.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status aerospike_record_operations_exists(aerospike* as_object_p,
		as_key* as_key_p,
		as_error *error_p,
		zval* metadata_p,
		zval* options_p TSRMLS_DC)
{
	as_policy_read              read_policy;
	as_record*                  record_p = NULL;

	if ((!as_key_p) || (!error_p) || (!as_object_p) || (!metadata_p)) {
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Cannot perform exists");
		DEBUG_PHP_EXT_DEBUG("Cannot perform exists");
		goto exit;
	}

	set_policy(&as_object_p->config, &read_policy, NULL, NULL, NULL, NULL,
			NULL, NULL, NULL, options_p, error_p TSRMLS_CC);
	if (AEROSPIKE_OK != error_p->code) {
		DEBUG_PHP_EXT_DEBUG("Unable to set policy");
		goto exit;
	}

	if (AEROSPIKE_OK != aerospike_key_exists(as_object_p, error_p,
				&read_policy, as_key_p, &record_p)) {
		goto exit;
	}

	add_assoc_long(metadata_p, "generation", record_p->gen);
	add_assoc_long(metadata_p, "ttl", record_p->ttl);

exit:
	if (record_p) {
		as_record_destroy(record_p);
	}

	return error_p->code;
}


/*
 *******************************************************************************************************
 * Wrapper function to perform an aerospike_key_remove within the C client.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param as_key_p              The C client's as_key that identifies the record.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_record_operations_remove(Aerospike_object* aerospike_obj_p,
		as_key* as_key_p,
		as_error *error_p,
		zval* options_p)
{
	as_policy_remove            remove_policy;
	aerospike*                  as_object_p = aerospike_obj_p->as_ref_p->as_p;
	TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);

	if ( (!as_key_p) || (!error_p) || (!as_object_p)) {
		DEBUG_PHP_EXT_DEBUG("Unable to remove key");
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to remove key");
		goto exit;
	}

	set_policy(&as_object_p->config, NULL, NULL, NULL, &remove_policy,
			NULL, NULL, NULL, NULL, options_p, error_p TSRMLS_CC);
	if (AEROSPIKE_OK != error_p->code) {
		DEBUG_PHP_EXT_DEBUG("Unable to set policy");
		goto exit;
	}

	get_generation_value(options_p, &remove_policy.generation, error_p TSRMLS_CC);
	if (error_p->code == AEROSPIKE_OK) {
		aerospike_key_remove(as_object_p, error_p, &remove_policy, as_key_p);
	}

exit:
	return error_p->code;
}

static as_status
aerospike_record_initialization(aerospike* as_object_p,
		as_key* as_key_p,
		zval* options_p,
		as_error* error_p,
		as_policy_operate* operate_policy,
		int8_t* serializer_policy TSRMLS_DC)
{
	as_policy_operate_init(operate_policy);

	if ((!as_object_p) || (!error_p) || (!as_key_p)) {
		DEBUG_PHP_EXT_DEBUG("Unable to perform operate");
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to perform operate");
		goto exit;
	}

	set_policy(&as_object_p->config, NULL, NULL, operate_policy, NULL, NULL,
			NULL, NULL, serializer_policy, options_p, error_p TSRMLS_CC);
	if (AEROSPIKE_OK != error_p->code) {
		DEBUG_PHP_EXT_DEBUG("Unable to set policy");
		goto exit;
	}
exit:
	return error_p->code;
}

extern as_status
aerospike_record_operations_general(Aerospike_object* aerospike_obj_p,
		as_key* as_key_p,
		zval* options_p,
		as_error* error_p,
		char* bin_name_p,
		char* str,
		u_int64_t offset,
		double double_offset,
		u_int64_t time_to_live,
		u_int64_t operation)
{
	as_operations       ops;
	as_record*          get_rec = NULL;
	aerospike*          as_object_p = aerospike_obj_p->as_ref_p->as_p;
	as_policy_operate   operate_policy;
	/*
	 * TODO: serializer_policy is not used right now.
	 * Need to pass on serializer_policy to aerospike_record_operations_ops
	 * in case of write operation, to write bytes to the database.
	 */
	int8_t              serializer_policy;

	TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);
	as_operations_inita(&ops, 1);
	get_generation_value(options_p, &ops.gen, error_p TSRMLS_CC);
	if (error_p->code != AEROSPIKE_OK) {
		goto exit;
	}
	if (AEROSPIKE_OK != aerospike_record_initialization(as_object_p, as_key_p,
				options_p, error_p,
				&operate_policy,
				&serializer_policy TSRMLS_CC)) {
		DEBUG_PHP_EXT_ERROR("Initialization returned error");
		goto exit;
	}

	if (AEROSPIKE_OK != aerospike_record_operations_ops(aerospike_obj_p, as_object_p, as_key_p,
				options_p, error_p,
				bin_name_p, str, NULL,
				offset, double_offset, time_to_live, 0, operation,
				&ops, NULL, NULL, 0, &get_rec TSRMLS_CC)) {

		DEBUG_PHP_EXT_ERROR("Prepend function returned an error");
		goto exit;
	}


	if (AEROSPIKE_OK == get_options_ttl_value(options_p, &ops.ttl, error_p TSRMLS_CC)) {
		aerospike_key_operate(as_object_p, error_p, &operate_policy,
				as_key_p, &ops, NULL);
	}

exit:
	if (get_rec) {
		as_record_destroy(get_rec);
	}
	as_operations_destroy(&ops);
	return error_p->code;
}

extern as_status
aerospike_record_operations_operate(Aerospike_object* aerospike_obj_p,
		as_key* as_key_p,
		zval* options_p,
		as_error* error_p,
		zval* returned_p,
		HashTable* operations_array_p)
{
	as_operations               ops;
	as_record*                  get_rec = NULL;
	aerospike*                  as_object_p = aerospike_obj_p->as_ref_p->as_p;
	as_status                   status = AEROSPIKE_OK;
	as_policy_operate           operate_policy;
	HashPosition                pointer;
	HashPosition                each_pointer;
	char*                       bin_name_p;
	char*                       str;
	char*                       geoStr;
	int                         offset = 0;
	double                      double_offset = 0.0;
#if PHP_VERSION_ID < 70000
	long                        l_offset = 0;
#else
	zend_long                   l_offset = 0;
#endif
	int                         op;
	HashTable*                  each_operation_array_p = NULL;
	DECLARE_ZVAL_P(each_operation);
	DECLARE_ZVAL_P(operation);
	DECLARE_ZVAL_P(each_operation_back);
	int8_t                      serializer_policy;
	uint32_t                    ttl;
	int64_t                     index;
	foreach_callback_udata      foreach_record_callback_udata;

	TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);
	as_operations_inita(&ops, zend_hash_num_elements(operations_array_p));

	get_generation_value(options_p, &ops.gen, error_p TSRMLS_CC);
	if (error_p->code != AEROSPIKE_OK) {
		goto exit;
	}

	if (AEROSPIKE_OK !=
			(status = aerospike_record_initialization(as_object_p, as_key_p,
													  options_p, error_p,
													  &operate_policy,
													  &serializer_policy TSRMLS_CC))) {
		DEBUG_PHP_EXT_ERROR("Initialization returned error");
		goto exit;
	}

#if PHP_VERSION_ID < 70000
	AEROSPIKE_FOREACH_HASHTABLE(operations_array_p, pointer, operation) {
#else
	ZEND_HASH_FOREACH_VAL(operations_array_p, operation) {
#endif
		as_record *temp_rec = NULL;

		if (IS_ARRAY == AEROSPIKE_Z_TYPE_P(operation)) {
				each_operation_array_p = AEROSPIKE_Z_ARRVAL_P(operation);
			str = NULL;
			geoStr = NULL;
			op = 0;
			ttl = 0;
			bin_name_p = NULL;
#if PHP_VERSION_ID < 70000
			AEROSPIKE_FOREACH_HASHTABLE(each_operation_array_p, each_pointer, each_operation) {
#else
			ZEND_HASH_FOREACH_VAL(each_operation_array_p, each_operation) {
#endif
				uint options_key_len;
				char* options_key;
#if PHP_VERSION_ID < 70000
				ulong options_index;
#else
				zend_ulong options_index;
#endif

#if PHP_VERSION_ID < 70000
				if (zend_hash_get_current_key_ex(each_operation_array_p, (char **) &options_key,
					&options_key_len, &options_index, 0, &each_pointer) != HASH_KEY_IS_STRING)
#else
				zend_string* z_str;// = zend_string_init(options_key, strlen(options_key), 0);
				ZEND_HASH_FOREACH_KEY_VAL(each_operation_array_p, each_pointer, z_str, each_operation) {
					options_key = z_str->val;
					if (!z_str)
#endif
					{
						DEBUG_PHP_EXT_DEBUG("Unable to set policy: Invalid Policy Constant Key");
						PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
							"Unable to set policy: Invalid Policy Constant Key");
						goto exit;
					} else {
						if (!strcmp(options_key, "op") && (IS_LONG ==
							AEROSPIKE_Z_TYPE_P(each_operation))) {
							op = (uint32_t) AEROSPIKE_Z_LVAL_P(each_operation);
						} else if (!strcmp(options_key, "bin") && (IS_STRING ==
							AEROSPIKE_Z_TYPE_P(each_operation))) {
							bin_name_p = (char *) AEROSPIKE_Z_STRVAL_P(each_operation);
						} else if (!strcmp(options_key, "val")) {
							if (IS_STRING == AEROSPIKE_Z_TYPE_P(each_operation)) {
								str = AEROSPIKE_Z_STRVAL_P(each_operation);
								each_operation_back = each_operation;
							} else if (IS_LONG == AEROSPIKE_Z_TYPE_P(each_operation)) {
								offset = (uint32_t)
								AEROSPIKE_Z_LVAL_P(each_operation);
							} else if (IS_DOUBLE == AEROSPIKE_Z_TYPE_P(each_operation) &&
										aerospike_has_double((as_object_p ))) {
								double_offset = (double)
								AEROSPIKE_Z_DVAL_P(each_operation);
							} else if (IS_OBJECT == AEROSPIKE_Z_TYPE_P(each_operation)) {
#if PHP_VERSION_ID < 70000
								const char* name;
								zend_uint name_len;
#else
								char* name;
								char *str;
								size_t name_len;
#endif
								int dup;
#if PHP_VERSION_ID < 70000
								dup = zend_get_object_classname(*((zval**)each_operation),
									&name, &name_len TSRMLS_CC);
								if((!strcmp(name, GEOJSONCLASS))
#else
								zend_class_entry *ce;
								ce = Z_OBJCE_P((zval*)each_operation);
								str = ce->name->val;
								if((!strcmp(str, GEOJSONCLASS))
#endif
									&& (aerospike_obj_p->hasGeoJSON)
									&& op == AS_OPERATOR_WRITE) {
									int result;

									DECLARE_ZVAL(retval);
									DECLARE_ZVAL(fname);
#if PHP_VERSION_ID < 70000
									ALLOC_INIT_ZVAL(fname);
									ZVAL_STRINGL(fname, "__tostring", sizeof("__tostring") -1, 1);
									result = call_user_function_ex(NULL, each_operation, fname, &retval,
										0, NULL, 0, NULL TSRMLS_CC);
									geoStr = (char *) malloc (strlen(Z_STRVAL_P(retval)) + 1);
									if (geoStr == NULL) {
										DEBUG_PHP_EXT_DEBUG("Failed to allocate memory\n");
										PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
											"Failed to allocate memory\n");
										goto exit;
									}
									memset(geoStr, '\0', strlen(geoStr));
									strcpy(geoStr, Z_STRVAL_P(retval));
									if (name) {
										efree((void *) name);
										name = NULL;
							 		}
									if (fname) {
										zval_ptr_dtor(&fname);
									}
									if (retval) {
										zval_ptr_dtor(&retval);
									}
#else
									AEROSPIKE_ZVAL_STRINGL(&fname, "__tostring", sizeof("__tostring") -1, 1);
									result = call_user_function_ex(NULL, each_operation, &fname, &retval,
										0, NULL, 0, NULL TSRMLS_CC);
									geoStr = (char *) malloc (strlen(Z_STRVAL_P(&retval)) + 1);
									if (geoStr == NULL) {
										DEBUG_PHP_EXT_DEBUG("Failed to allocate memory\n");
										PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
											"Failed to allocate memory\n");
										goto exit;
									}
									memset(geoStr, '\0', strlen(geoStr));
									strcpy(geoStr, Z_STRVAL_P(&retval));

									/*if (name) {
										efree((void *) name);
										name = NULL;
									}*/
									if (&fname) {
										zval_ptr_dtor(&fname);
									}
									if (&retval) {
										zval_ptr_dtor(&retval);
									}
#endif
								} else {
									status = AEROSPIKE_ERR_CLIENT;
									DEBUG_PHP_EXT_DEBUG("Invalid operation on GeoJSON datatype OR Old version of server, "
										"GeoJSON not supported on this server");
									PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Invalid operation on GeoJSON "
										"datatype OR Old version of server, GeoJSON not supported on this server");
									goto exit;
								}
							} else if (IS_ARRAY == AEROSPIKE_Z_TYPE_P(each_operation)) {
						} else {
							status = AEROSPIKE_ERR_CLIENT;
							goto exit;
						}
					} else if (!strcmp(options_key, "ttl") && (IS_LONG == AEROSPIKE_Z_TYPE_P(each_operation))) {
						  ttl = (uint32_t)AEROSPIKE_Z_LVAL_P(each_operation);
					} else if (!strcmp(options_key, "index") && (IS_LONG == AEROSPIKE_Z_TYPE_P(each_operation))) {
						  index = AEROSPIKE_Z_LVAL_P(each_operation);
					} else {
						status = AEROSPIKE_ERR_CLIENT;
						DEBUG_PHP_EXT_DEBUG("Unable to set Operate: Invalid Options Key");
						PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to set Operate: Invalid Options Key");
						goto exit;
					}
				}
#if PHP_VERSION_ID < 70000
			}
#else
			} ZEND_HASH_FOREACH_END();
			} ZEND_HASH_FOREACH_END();
#endif
			if (op == AS_OPERATOR_INCR) {
				if (str) {
					l_offset = (long) offset;
					if (!(is_numeric_string(AEROSPIKE_Z_STRVAL_P(each_operation_back),
							AEROSPIKE_Z_STRLEN_P(each_operation_back), &l_offset, NULL, 0))) {
						status = AEROSPIKE_ERR_PARAM;
						PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "invalid value for increment operation");
						DEBUG_PHP_EXT_DEBUG("Invalid value for increment operation");
						goto exit;
					}
				}
			}

			if (AEROSPIKE_OK != (status = aerospike_record_operations_ops(aerospike_obj_p, as_object_p,
							as_key_p, options_p, error_p, bin_name_p, str, geoStr,
							offset, double_offset, ttl, index, op, &ops, each_operation, &operate_policy,
							serializer_policy, &temp_rec TSRMLS_CC))) {
				DEBUG_PHP_EXT_ERROR("Operate function returned an error");
				goto exit;
			}
			if (temp_rec) {
				as_record_destroy(temp_rec);
			}
		} else {
			status = AEROSPIKE_ERR_CLIENT;
			goto exit;
		}
	}
#if PHP_VERSION_ID >= 70000
	ZEND_HASH_FOREACH_END();
#endif

	if (AEROSPIKE_OK != get_options_ttl_value(options_p, &ops.ttl, error_p TSRMLS_CC)) {
		goto exit;
	}

	if (AEROSPIKE_OK != (status = aerospike_key_operate(as_object_p, error_p,
					&operate_policy, as_key_p, &ops, &get_rec))) {
		DEBUG_PHP_EXT_DEBUG("%s", error_p->message);
		goto exit;
	} else {
		if (get_rec) {
			foreach_record_callback_udata.udata_p = returned_p;
			foreach_record_callback_udata.error_p = error_p;
			foreach_record_callback_udata.obj = aerospike_obj_p;
			if (!as_record_foreach(get_rec, (as_rec_foreach_callback) AS_DEFAULT_GET,
						&foreach_record_callback_udata)) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
						"Unable to get bins of a record");
				DEBUG_PHP_EXT_DEBUG("Unable to get bins of a record");
			}
		}
	}

exit:
	if (get_rec) {
		foreach_record_callback_udata.udata_p = NULL;
		as_record_destroy(get_rec);
	}
	as_operations_destroy(&ops);
	return status;
}

extern as_status
aerospike_record_operations_operate_ordered(Aerospike_object* aerospike_obj_p,
		as_key* as_key_p,
		zval* options_p,
		as_error* error_p,
		zval* returned_p,
		HashTable* operations_array_p)
{
	as_operations               ops;
	as_record*                  get_rec = NULL;
	as_record*                  get_rec_temp = NULL;
	aerospike*                  as_object_p = aerospike_obj_p->as_ref_p->as_p;
	as_status                   status = AEROSPIKE_OK;
	as_policy_operate           operate_policy;
	HashPosition                pointer;
	HashPosition                each_pointer;
	HashTable*                  each_operation_array_p = NULL;
	char*                       bin_name_p;
	char*                       str;
	char*                       geoStr;
	int                         offset = 0;
	double                      double_offset = 0.0;
#if PHP_VERSION_ID < 70000
	long                        l_offset = 0;
#else
	zend_long                   l_offset = 0;
#endif
	int                         op;
	int8_t                      serializer_policy;
	uint32_t                    ttl;
	int64_t                     index;
	foreach_callback_udata      foreach_record_callback_udata;
	DECLARE_ZVAL_P(each_operation);
	DECLARE_ZVAL_P(operation);
	DECLARE_ZVAL_P(each_operation_back);
	DECLARE_ZVAL(record_p_local);
	DECLARE_ZVAL(metadata_container_p);
	DECLARE_ZVAL(key_container_p);

#if PHP_VERSION_ID < 70000
	MAKE_STD_ZVAL(record_p_local)
	MAKE_STD_ZVAL(metadata_container_p);
	MAKE_STD_ZVAL(key_container_p);
#endif
	array_init(AEROSPIKE_ZVAL_ARG(record_p_local));
	array_init(AEROSPIKE_ZVAL_ARG(metadata_container_p));
	array_init(AEROSPIKE_ZVAL_ARG(key_container_p));

	TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);

	if (AEROSPIKE_OK !=
			(status = aerospike_record_initialization(as_object_p, as_key_p,
													  options_p, error_p,
													  &operate_policy,
													  &serializer_policy TSRMLS_CC))) {
		DEBUG_PHP_EXT_ERROR("Initialization returned error");
		goto exit;
	}

#if PHP_VERSION_ID < 70000
	AEROSPIKE_FOREACH_HASHTABLE(operations_array_p, pointer, operation) {
#else
	ZEND_HASH_FOREACH_VAL(operations_array_p, operation) {
		zend_string* z_str;
#endif
		as_operations_inita(&ops, zend_hash_num_elements(operations_array_p));

		get_generation_value(options_p, &ops.gen, error_p TSRMLS_CC);
		if (error_p->code != AEROSPIKE_OK) {
			goto exit;
		}
		as_record *temp_rec = NULL;

		if (IS_ARRAY == AEROSPIKE_Z_TYPE_P(operation)) {
			each_operation_array_p = AEROSPIKE_Z_ARRVAL_P(operation);
			str = NULL;
			geoStr = NULL;
			op = 0;
			ttl = 0;
			index = 0;
			bin_name_p = NULL;
#if PHP_VERSION_ID < 70000
			AEROSPIKE_FOREACH_HASHTABLE(each_operation_array_p, each_pointer, each_operation) {
#else
			ZEND_HASH_FOREACH_KEY_VAL(each_operation_array_p, each_pointer, z_str, each_operation) {
#endif
				uint options_key_len;
#if PHP_VERSION_ID < 70000
				ulong options_index;
#else
				zend_ulong options_index;
#endif
				char* options_key;

#if PHP_VERSION_ID < 70000
				if (zend_hash_get_current_key_ex(each_operation_array_p, (char **) &options_key,
					&options_key_len, &options_index, 0, &each_pointer) != HASH_KEY_IS_STRING)
#else
					options_key = z_str->val;
					if (!z_str)
#endif
				{
					DEBUG_PHP_EXT_DEBUG("Unable to set policy: Invalid Policy Constant Key");
					PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
							"Unable to set policy: Invalid Policy Constant Key");
					goto exit;
				} else {
					if (!strcmp(options_key, "op") && (IS_LONG == AEROSPIKE_Z_TYPE_P(each_operation))) {
						op = (uint32_t) AEROSPIKE_Z_LVAL_P(each_operation);
					} else if (!strcmp(options_key, "bin") && (IS_STRING == AEROSPIKE_Z_TYPE_P(each_operation))) {
						bin_name_p = (char *) AEROSPIKE_Z_STRVAL_P(each_operation);
					} else if (!strcmp(options_key, "val")) {
						if (IS_STRING == AEROSPIKE_Z_TYPE_P(each_operation)) {
							str = (char *) AEROSPIKE_Z_STRVAL_P(each_operation);
							each_operation_back = each_operation;
						} else if (IS_LONG == AEROSPIKE_Z_TYPE_P(each_operation)) {
							offset = (uint32_t) AEROSPIKE_Z_LVAL_P(each_operation);
						} else if (IS_OBJECT == AEROSPIKE_Z_TYPE_P(each_operation)) {
#if PHP_VERSION_ID < 70000
							const char* name;
							zend_uint name_len;
#else
							char* name;
							char *str;
							size_t name_len;
#endif
							int dup;
#if PHP_VERSION_ID < 70000
							dup = zend_get_object_classname(*((zval**)each_operation),
								&name, &name_len TSRMLS_CC);
							if((!strcmp(name, GEOJSONCLASS))
#else
							zend_class_entry *ce;
							ce = Z_OBJCE_P((zval*)each_operation);
							str = ce->name->val;
							if((!strcmp(str, GEOJSONCLASS))
#endif
									&& (aerospike_obj_p->hasGeoJSON)
									&& op == AS_OPERATOR_WRITE) {
								int result;

								DECLARE_ZVAL(retval);
								DECLARE_ZVAL(fname);
#if PHP_VERSION_ID < 70000
								ALLOC_INIT_ZVAL(fname);
								ZVAL_STRINGL(fname, "__tostring", sizeof("__tostring") -1, 1);
								result = call_user_function_ex(NULL, each_operation, fname, &retval,
									0, NULL, 0, NULL TSRMLS_CC);
								geoStr = (char *) malloc (strlen(Z_STRVAL_P(retval)) + 1);
								if (geoStr == NULL) {
									DEBUG_PHP_EXT_DEBUG("Failed to allocate memory\n");
									PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
										"Failed to allocate memory\n");
									goto exit;
								}
								memset(geoStr, '\0', strlen(geoStr));
								strcpy(geoStr, Z_STRVAL_P(retval));
								if (name) {
									efree((void *) name);
									name = NULL;
						 		}
								if (fname) {
									zval_ptr_dtor(&fname);
								}
								if (retval) {
									zval_ptr_dtor(&retval);
								}
#else
								AEROSPIKE_ZVAL_STRINGL(&fname, "__tostring", sizeof("__tostring") -1, 1);
								result = call_user_function_ex(NULL, each_operation, &fname, &retval,
									0, NULL, 0, NULL TSRMLS_CC);
								geoStr = (char *) malloc (strlen(Z_STRVAL_P(&retval)) + 1);
								if (geoStr == NULL) {
									DEBUG_PHP_EXT_DEBUG("Failed to allocate memory\n");
									PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
										"Failed to allocate memory\n");
									goto exit;
								}
								memset(geoStr, '\0', strlen(geoStr));
								strcpy(geoStr, Z_STRVAL_P(&retval));

								/*if (name) {
									efree((void *) name);
									name = NULL;
								}*/
								if (&fname) {
									zval_ptr_dtor(&fname);
								}
								if (&retval) {
									zval_ptr_dtor(&retval);
								}
#endif
							}
							else {
								status = AEROSPIKE_ERR_CLIENT;
								DEBUG_PHP_EXT_DEBUG("Invalid operation on GeoJSON datatype OR Old version of server, "
										"GeoJSON not supported on this server");
								PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Invalid operation on GeoJSON "
										"datatype OR Old version of server, GeoJSON not supported on this server");
								goto exit;
							}
						} else if (IS_ARRAY == AEROSPIKE_Z_TYPE_P(each_operation)) {
						} else {
							status = AEROSPIKE_ERR_CLIENT;
							goto exit;
						}
					} else if (!strcmp(options_key, "ttl") && (IS_LONG == AEROSPIKE_Z_TYPE_P(each_operation))) {
						ttl = (uint32_t) AEROSPIKE_Z_LVAL_P(each_operation);
					} else if (!strcmp(options_key, "index") && (IS_LONG == AEROSPIKE_Z_TYPE_P(each_operation))) {
						index = AEROSPIKE_Z_LVAL_P(each_operation);
					} else {
						status = AEROSPIKE_ERR_CLIENT;
						DEBUG_PHP_EXT_DEBUG("Unable to set Operate: Invalid Optiopns Key");
						PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to set Operate: Invalid Options Key");
						goto exit;
					}
				}
#if PHP_VERSION_ID < 70000
			}
#else
			} ZEND_HASH_FOREACH_END();
#endif
			if (op == AS_OPERATOR_INCR) {
				if (str) {
					l_offset = (long) offset;
					if  (!(is_numeric_string(AEROSPIKE_Z_STRVAL_P(each_operation_back), AEROSPIKE_Z_STRLEN_P(each_operation_back), &l_offset, NULL, 0))) {
						status = AEROSPIKE_ERR_PARAM;
						PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "invalid value for increment operation");
						DEBUG_PHP_EXT_DEBUG("Invalid value for increment operation");
						goto exit;
					}
				}
			}
			if (AEROSPIKE_OK != (status = aerospike_record_operations_ops(aerospike_obj_p, as_object_p,
							as_key_p, options_p, error_p, bin_name_p, str, geoStr,
							offset, double_offset, ttl, index, op, &ops, each_operation, &operate_policy,
							serializer_policy, &temp_rec TSRMLS_CC))) {
				DEBUG_PHP_EXT_ERROR("Operate function returned an error");
				goto exit;
			}
			if (temp_rec) {
				as_record_destroy(temp_rec);
			}
		} else {
			status = AEROSPIKE_ERR_CLIENT;
			goto exit;
		}

		if (AEROSPIKE_OK != get_options_ttl_value(options_p, &ops.ttl, error_p TSRMLS_CC)) {
			goto exit;
		}
		foreach_record_callback_udata.udata_p = AEROSPIKE_ZVAL_ARG(record_p_local);
		foreach_record_callback_udata.error_p = error_p;
		foreach_record_callback_udata.obj = aerospike_obj_p;

		if (AEROSPIKE_OK != (status = aerospike_key_operate(as_object_p, error_p,
						&operate_policy, as_key_p, &ops, &get_rec))) {
			DEBUG_PHP_EXT_DEBUG("%s", error_p->message);
			operater_ordered_callback(bin_name_p, NULL, &foreach_record_callback_udata TSRMLS_CC);
			goto exit;
		} else {
			if (get_rec) {
				if (!((op == AS_OPERATOR_READ ) ||
							(op == OP_LIST_SIZE) ||
							(op == OP_LIST_GET)   ||
							(op == OP_LIST_GET_RANGE) ||
							(op == OP_LIST_POP)   ||
							(op == OP_LIST_POP_RANGE))) {
					if (!operater_ordered_callback(bin_name_p, NULL, &foreach_record_callback_udata TSRMLS_CC)) {
						PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
								"Unable to get bins of a record");
						DEBUG_PHP_EXT_DEBUG("Unable to get bins of a record");
					}
				} else if (get_rec->bins.size == 0){
					if (!operater_ordered_callback(bin_name_p, NULL, &foreach_record_callback_udata TSRMLS_CC)) {
						PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
								"Unable to get bins of a record");
						DEBUG_PHP_EXT_DEBUG("Unable to get bins of a record");
					}
				}else {
					if (!as_record_foreach(get_rec, (as_rec_foreach_callback) operater_ordered_callback,
								&foreach_record_callback_udata)) {
						PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
								"Unable to get bins of a record");
						DEBUG_PHP_EXT_DEBUG("Unable to get bins of a record");
					}
				}
			}
		}
		as_operations_destroy(&ops);
		if (get_rec) {
			if (get_rec_temp) {
				as_record_destroy(get_rec_temp);
				get_rec_temp = NULL;
			}
			get_rec_temp = get_rec;
			get_rec = NULL;
		}
	}
#if PHP_VERSION_ID >= 70000
	ZEND_HASH_FOREACH_END();
#endif

exit:
	status = aerospike_get_record_metadata(get_rec_temp, AEROSPIKE_ZVAL_ARG(metadata_container_p) TSRMLS_CC);
	if (status != AEROSPIKE_OK) {
		DEBUG_PHP_EXT_DEBUG("Unable to get metadata of record.");
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Unable to get metadata of record.");
		goto exit_1;
	}


	status = aerospike_get_record_key_digest(&(aerospike_obj_p->as_ref_p->as_p->config), get_rec_temp, as_key_p,
	AEROSPIKE_ZVAL_ARG(key_container_p), options_p, true TSRMLS_CC);
	if (status != AEROSPIKE_OK) {
		DEBUG_PHP_EXT_DEBUG("Unable to get key and digest for record.");
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Unable to get key and digest for record.");
		goto exit_1;
	}

	if (0 != add_assoc_zval(returned_p, PHP_AS_KEY_DEFINE_FOR_KEY, AEROSPIKE_ZVAL_ARG(key_container_p))) {
		DEBUG_PHP_EXT_DEBUG("Unable to get key of a record.");
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Unable to get key of a record.");
		goto exit_1;
	}

	if (0 != add_assoc_zval(returned_p, PHP_AS_RECORD_DEFINE_FOR_METADATA, AEROSPIKE_ZVAL_ARG(metadata_container_p))) {
		DEBUG_PHP_EXT_DEBUG("Unable to get metadata of a record.");
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Unable to get metadata of a record.");
		goto exit_1;
	}

	if (0 != add_assoc_zval(returned_p, PHP_AS_RECORD_DEFINE_FOR_RESULTS, AEROSPIKE_ZVAL_ARG(record_p_local))) {
		DEBUG_PHP_EXT_DEBUG("Unable to get the record.");
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Unable to get the record.");
		goto exit_1;
	}

exit_1:
	if (get_rec_temp) {
		foreach_record_callback_udata.udata_p = NULL;
		as_record_destroy(get_rec_temp);
		get_rec_temp = NULL;
	}
	return status;
}


/*
 *******************************************************************************************************
 * Wrapper function to remove bin(s) from a record.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param as_key_p              The C client's as_key that identifies the record.
 * @param bins_p                The PHP array of bins to be removed from the record.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_record_operations_remove_bin(Aerospike_object* aerospike_obj_p,
		as_key* as_key_p,
		zval* bins_p,
		as_error* error_p,
		zval* options_p)
{
	as_status           status = AEROSPIKE_OK;
	as_record           rec;
	HashTable           *bins_array_p = Z_ARRVAL_P(bins_p);
	HashPosition        pointer;
	DECLARE_ZVAL_P(bin_names);
	as_policy_write     write_policy;
	aerospike*          as_object_p = aerospike_obj_p->as_ref_p->as_p;
	TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);

	as_record_inita(&rec, zend_hash_num_elements(bins_array_p));

	if ((!as_object_p) || (!error_p) || (!as_key_p) || (!bins_array_p)) {
		status = AEROSPIKE_ERR_CLIENT;
		goto exit;
	}

	set_policy(&as_object_p->config, NULL, &write_policy, NULL, NULL, NULL,
			NULL, NULL, NULL, options_p, error_p TSRMLS_CC);
	if (AEROSPIKE_OK != (status = (error_p->code))) {
		DEBUG_PHP_EXT_DEBUG("Unable to set policy");
		goto exit;
	}


	AEROSPIKE_FOREACH_HASHTABLE (bins_array_p, pointer, bin_names) {
		if (IS_STRING == AEROSPIKE_Z_TYPE_P(bin_names)) {
			if (!(as_record_set_nil(&rec, AEROSPIKE_Z_STRVAL_P(bin_names)))) {
				status = AEROSPIKE_ERR_CLIENT;
				goto exit;
			}
		} else {
			status = AEROSPIKE_ERR_CLIENT;
			goto exit;
		}
	}
	AEROSPIKE_FOREACH_HASHTABLE_END;

	get_generation_value(options_p, &rec.gen, error_p TSRMLS_CC);

	if (error_p->code == AEROSPIKE_OK) {
		if (AEROSPIKE_OK == get_options_ttl_value(options_p, &rec.ttl,
					error_p TSRMLS_CC)) {
			if (AEROSPIKE_OK != (status = aerospike_key_put(as_object_p, error_p,
							NULL, as_key_p, &rec))) {
				goto exit;
			}
		}

	}

exit:
	as_record_destroy(&rec);
	return(status);
}

/*
 *******************************************************************************************************
 * Wrapper function to perform an aerospike_key_exists within the C client.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param as_key_p              The C client's as_key that identifies the record.
 * @param metadata_p            The return metadata for the exists/getMetadata API to be
 *                              populated by this function.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_php_exists_metadata(Aerospike_object* aerospike_obj_p,
		zval* key_record_p,
		zval* metadata_p,
		zval* options_p,
		as_error* error_p)
{
	as_status              status = AEROSPIKE_OK;
	as_key                 as_key_for_put_record;
	int16_t                initializeKey = 0;
	aerospike*             as_object_p = aerospike_obj_p->as_ref_p->as_p;

	TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);
	if ((!as_object_p) || (!key_record_p)) {
		DEBUG_PHP_EXT_DEBUG("Unable to perform exists");
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to perform exists");
		status = AEROSPIKE_ERR_CLIENT;
		goto exit;
	}

	if (PHP_TYPE_ISNOTARR(key_record_p) ||
			((options_p) && (PHP_TYPE_ISNOTARR(options_p)))) {
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
				"input parameters (type) for exist/getMetdata function not proper.");
		DEBUG_PHP_EXT_ERROR("input parameters (type) for exist/getMetdata function not proper.");
		status = AEROSPIKE_ERR_PARAM;
		goto exit;
	}

	if (PHP_TYPE_ISNOTARR(metadata_p)) {
		DECLARE_ZVAL(metadata_arr_p);
#if PHP_VERSION_ID < 70000
		MAKE_STD_ZVAL(metadata_arr_p);
		array_init(metadata_arr_p);
		ZVAL_ZVAL(metadata_p, metadata_arr_p, 1, 1);
#else
		array_init(&metadata_arr_p);
		ZVAL_ZVAL(metadata_p, &metadata_arr_p, 1, 1);
#endif
	}

	if (AEROSPIKE_OK != (status =
				aerospike_transform_iterate_for_rec_key_params(Z_ARRVAL_P(key_record_p),
					&as_key_for_put_record, &initializeKey))) {
		PHP_EXT_SET_AS_ERR(error_p, status,
				"unable to iterate through exists/getMetadata key params");
		DEBUG_PHP_EXT_ERROR("unable to iterate through exists/getMetadata key params");
		goto exit;
	}

	if (AEROSPIKE_OK != (status =
				aerospike_record_operations_exists(as_object_p, &as_key_for_put_record,
					error_p, metadata_p, options_p TSRMLS_CC))) {
		DEBUG_PHP_EXT_ERROR("exists/getMetadata: unable to fetch the record");
		goto exit;
	}

exit:
	if (initializeKey) {
		as_key_destroy(&as_key_for_put_record);
	}

	return status;
}
