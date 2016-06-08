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
#include "aerospike/as_log.h"
#include "aerospike/as_key.h"
#include "aerospike/as_config.h"
#include "aerospike/as_error.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike.h"
#include "aerospike_common.h"
#include "aerospike/as_udf.h"
#include "aerospike/as_query.h"
#include "aerospike/aerospike_query.h"
#include "aerospike_policy.h"

#define PROGRESS_PCT "progress_pct"
#define RECORDS_READ "records_read"
#define STATUS "status"
/*
 ******************************************************************************************************
 Initializes and defines an as_query object.
 *
 * @param as_query_p                The C client's as_query object to be
 *                                  initialized.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param namespace_p               The namespace to scan.
 * @param set_p                     The set to scan.
 * @param predicate_ht_p            The HashTable for Query Predicate array.
 * @param module_p                  The name of UDF module containing the function
 *                                  to execute.
 * @param function_p                The name of the function to be applied to
 *                                  the record.
 * @param args_list_p               An as_arraylist initialized with arguments for the UDF.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
static as_status
aerospike_query_define(as_query* query_p, as_error* error_p, char* namespace_p,
		char* set_p, HashTable *predicate_ht_p, const char* module_p,
		const char* function_p, as_arraylist* args_list_p TSRMLS_DC)
{
	#if PHP_VERSION_ID < 70000
				zval**              val_pp = NULL;
				zval**              op_pp  = NULL;
				zval**              bin_pp = NULL;
				zval**              index_type_pp = NULL;
	#else
				zval*               val_pp = NULL;
				zval*               op_pp  = NULL;
				zval*               bin_pp = NULL;
				zval*               index_type_pp = NULL;
	#endif

	if (predicate_ht_p && (zend_hash_num_elements(predicate_ht_p) != 0)) {
		#if PHP_VERSION_ID >= 70000
		  zend_string* z_bin = zend_string_init(BIN, strlen(BIN), 0);
	 	  zend_string* z_op  = zend_string_init(OP, strlen(OP), 0);
		  zend_string* z_val = zend_string_init(VAL, strlen(VAL), 0);
		#endif
		if (
#if PHP_VERSION_ID < 70000
				(!zend_hash_exists(predicate_ht_p, BIN, sizeof(BIN))) ||
				(!zend_hash_exists(predicate_ht_p, OP, sizeof(OP)))  ||
				(!zend_hash_exists(predicate_ht_p, VAL, sizeof(VAL)))
#else
				(!zend_hash_exists(predicate_ht_p, z_bin)) ||
				(!zend_hash_exists(predicate_ht_p, z_op))  ||
				(!zend_hash_exists(predicate_ht_p, z_val))
#endif
				) {
			DEBUG_PHP_EXT_DEBUG("Predicate is expected to include the keys 'bin','op', and 'val'.");
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
					"Predicate is expected to include the keys 'bin','op', and 'val'.");
			goto exit;
		}

		if (
#if PHP_VERSION_ID < 70000
				(FAILURE == AEROSPIKE_ZEND_HASH_FIND(predicate_ht_p, OP, sizeof(OP),
						(void **) &op_pp)) ||
				(FAILURE == AEROSPIKE_ZEND_HASH_FIND(predicate_ht_p, BIN, sizeof(BIN),
										   (void **) &bin_pp)) ||
				(FAILURE == AEROSPIKE_ZEND_HASH_FIND(predicate_ht_p, VAL, sizeof(VAL),
										   (void **) &val_pp))
#else
				((op_pp = AEROSPIKE_ZEND_HASH_FIND(predicate_ht_p, OP, sizeof(OP),
				               (void **) &op_pp)) == NULL) ||
				((bin_pp = AEROSPIKE_ZEND_HASH_FIND(predicate_ht_p, BIN, sizeof(BIN),
								 			 (void **) &bin_pp)) == NULL) ||
				((val_pp = AEROSPIKE_ZEND_HASH_FIND(predicate_ht_p, VAL, sizeof(VAL),
								 			 (void **) &val_pp)) == NULL)
#endif
				) {
			DEBUG_PHP_EXT_DEBUG("Predicate is expected to include the keys 'bin','op', 'val'.");
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
					"Predicate is expected to include the keys 'bin','op', 'val'.");
			goto exit;
		}

#if PHP_VERSION_ID < 70000
		convert_to_string_ex(op_pp);
		convert_to_string_ex(bin_pp);
#else
		convert_to_string_ex(op_pp);
		convert_to_string_ex(bin_pp);
#endif
		//convert_to_string_ex(index_type_pp);
#if PHP_VERSION_ID < 70000
		if (strncmp(Z_STRVAL_PP(op_pp), "=", 1) == 0) {
#else
		if (strncmp(Z_STRVAL_P(op_pp), "=", 1) == 0) {
#endif
			switch(
			  #if PHP_VERSION_ID < 70000
				  Z_TYPE_PP(val_pp
			  #else
					Z_TYPE_P(val_pp
				#endif
				)) {
				case IS_STRING:
#if PHP_VERSION_ID < 70000
					convert_to_string_ex(val_pp);
					if (!as_query_where(query_p, Z_STRVAL_PP(bin_pp),
								as_equals(STRING, Z_STRVAL_PP(val_pp)))) {
#else
					convert_to_string_ex(val_pp);
					if (!as_query_where(query_p, Z_STRVAL_P(bin_pp),
								as_equals(STRING, Z_STRVAL_P(val_pp)))) {
#endif
						DEBUG_PHP_EXT_DEBUG("Unable to set query predicate");
						PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
								"Unable to set query predicate");
					}
					break;
				case IS_LONG:
#if PHP_VERSION_ID < 70000
					convert_to_long_ex(val_pp);
					if (!as_query_where(query_p, Z_STRVAL_PP(bin_pp),
								as_equals(NUMERIC, Z_LVAL_PP(val_pp)))) {
#else
					convert_to_long_ex(val_pp);
					if (!as_query_where(query_p, Z_STRVAL_P(bin_pp),
								as_equals(NUMERIC, Z_LVAL_P(val_pp)))) {
#endif
						DEBUG_PHP_EXT_DEBUG("Unable to set query predicate");
						PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
								"Unable to set query predicate");
					}
					break;
				default:
					DEBUG_PHP_EXT_DEBUG("Predicate 'val' must be either string or integer.");
					PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
							"Predicate 'val' must be either string or integer.");
					goto exit;
			}
		}
#if PHP_VERSION_ID < 70000
					else if (strncmp(Z_STRVAL_PP(op_pp), "BETWEEN", 7) == 0)
#else
					else if (strncmp(Z_STRVAL_P(op_pp), "BETWEEN", 7) == 0)
#endif
					{
			bool between_unpacked = false;
			if (
			  #if PHP_VERSION_ID < 70000
				  Z_TYPE_PP(val_pp
				#else
				  Z_TYPE_P(val_pp
				#endif
				) == IS_ARRAY) {
        convert_to_array_ex(val_pp);
				#if PHP_VERSION_ID < 70000
				  zval** min_pp;
				  zval** max_pp;
				#else
				  zval* min_pp;
				  zval* max_pp;
				#endif
				if (
#if PHP_VERSION_ID < 70000
						(AEROSPIKE_ZEND_HASH_INDEX_FIND(Z_ARRVAL_PP(val_pp), 0, (void **) &min_pp) == SUCCESS) &&
						(AEROSPIKE_ZEND_HASH_INDEX_FIND(Z_ARRVAL_PP(val_pp), 1, (void **) &max_pp) == SUCCESS)
#else
						((min_pp = AEROSPIKE_ZEND_HASH_INDEX_FIND(Z_ARRVAL_P(val_pp), 0, (void **) &min_pp)) != NULL) &&
						((max_pp = AEROSPIKE_ZEND_HASH_INDEX_FIND(Z_ARRVAL_P(val_pp), 1, (void **) &max_pp)) != NULL)
#endif
						) {
					convert_to_long_ex(min_pp);
					convert_to_long_ex(max_pp);
					if (

							#if PHP_VERSION_ID < 70000
							  Z_TYPE_PP(min_pp
							#else
								Z_TYPE_P(min_pp
							#endif
						) == IS_LONG &&
							#if PHP_VERSION_ID < 70000
								Z_TYPE_PP(max_pp
							#else
								Z_TYPE_P(max_pp
							#endif
						) == IS_LONG) {
						between_unpacked = true;
						if (
#if PHP_VERSION_ID < 70000
								!as_query_where(query_p, Z_STRVAL_PP(bin_pp),
									as_range(DEFAULT, NUMERIC, Z_LVAL_PP(min_pp), Z_LVAL_PP(max_pp)))
#else
								!as_query_where(query_p, Z_STRVAL_P(bin_pp),
									as_range(DEFAULT, NUMERIC, Z_LVAL_P(min_pp), Z_LVAL_P(max_pp)))
#endif
								) {
							DEBUG_PHP_EXT_DEBUG("Unable to set query predicate");
							PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
									"Unable to set query predicate");
						}
					}
				}
			}
			if (!between_unpacked) {
				DEBUG_PHP_EXT_DEBUG("Predicate BETWEEN 'op' requires an array of (min,max) integers.");
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
						"Predicate BETWEEN 'op' requires an array of (min,max) integers.");
				goto exit;
			}
		} else if (
#if PHP_VERSION_ID < 70000
				strncmp(Z_STRVAL_PP(op_pp), "CONTAINS", 8) == 0
#else
				strncmp(Z_STRVAL_P(op_pp), "CONTAINS", 8) == 0
#endif
				) {
			if (
#if PHP_VERSION_ID < 70000
					(FAILURE == AEROSPIKE_ZEND_HASH_FIND(predicate_ht_p, INDEX_TYPE, sizeof(INDEX_TYPE),
										   (void **) &index_type_pp))
#else
					((index_type_pp = AEROSPIKE_ZEND_HASH_FIND(predicate_ht_p, INDEX_TYPE, sizeof(INDEX_TYPE),
										   (void **) &index_type_pp)) == NULL)
#endif
					) {
				DEBUG_PHP_EXT_DEBUG("Predicate is expected to include 'index_type'.");
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
					"Predicate is expected to include 'index_type'.");
				goto exit;
			}
#if PHP_VERSION_ID < 70000
			convert_to_long_ex(index_type_pp);
#else
			convert_to_long_ex(index_type_pp);
#endif
			switch(

					#if PHP_VERSION_ID < 70000
						Z_TYPE_PP(val_pp
					#else
						Z_TYPE_P(val_pp
					#endif
				)) {
				case IS_STRING:
					convert_to_string_ex(val_pp);
					if (
						#if PHP_VERSION_ID < 70000
					    Z_LVAL_PP(index_type_pp
						#else
						  Z_LVAL_P(index_type_pp
						#endif
						) == AS_INDEX_TYPE_MAPVALUES) {
						if (
#if PHP_VERSION_ID < 70000
								!as_query_where(query_p, Z_STRVAL_PP(bin_pp),
									as_contains(MAPVALUES , STRING, Z_STRVAL_PP(val_pp)))
#else
								!as_query_where(query_p, Z_STRVAL_P(bin_pp),
									as_contains(MAPVALUES , STRING, Z_STRVAL_P(val_pp)))
#endif
								) {
							DEBUG_PHP_EXT_DEBUG("Unable to set query predicate");
							PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
									"Unable to set query predicate");
						}
					} else if (
					    #if PHP_VERSION_ID < 70000
						    Z_LVAL_PP(index_type_pp
							#else
							  Z_LVAL_P(index_type_pp
							#endif
						) == AS_INDEX_TYPE_MAPKEYS) {
						if (
#if PHP_VERSION_ID < 70000
								!as_query_where(query_p, Z_STRVAL_PP(bin_pp),
									as_contains(MAPKEYS , STRING, Z_STRVAL_PP(val_pp)))
#else
								!as_query_where(query_p, Z_STRVAL_P(bin_pp),
									as_contains(MAPKEYS , STRING, Z_STRVAL_P(val_pp)))
#endif
								) {
							DEBUG_PHP_EXT_DEBUG("Unable to set query predicate");
							PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
									"Unable to set query predicate");
						}
					} else if (
						#if PHP_VERSION_ID < 70000
						  Z_LVAL_PP(index_type_pp
						#else
							Z_LVAL_P(index_type_pp
						#endif
						) == AS_INDEX_TYPE_LIST) {
						if (
#if PHP_VERSION_ID < 70000
								!as_query_where(query_p, Z_STRVAL_PP(bin_pp),
									as_contains(LIST , STRING, Z_STRVAL_PP(val_pp)))
#else
								!as_query_where(query_p, Z_STRVAL_P(bin_pp),
									as_contains(LIST , STRING, Z_STRVAL_P(val_pp)))
#endif
								) {
							DEBUG_PHP_EXT_DEBUG("Unable to set query predicate");
							PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
									"Unable to set query predicate");
						}
					} else {
						DEBUG_PHP_EXT_DEBUG("Index type is invalid.");
						PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
								"Index type is invalid.");
						goto exit;
					}
					break;
				case IS_LONG:
					convert_to_long_ex(val_pp);
					if (
							#if PHP_VERSION_ID < 70000
							  Z_LVAL_PP(index_type_pp
							#else
							  Z_LVAL_P(index_type_pp
							#endif
						) == AS_INDEX_TYPE_MAPVALUES) {
						if (
#if PHP_VERSION_ID < 70000
								!as_query_where(query_p, Z_STRVAL_PP(bin_pp),
									as_contains(MAPVALUES , NUMERIC, Z_LVAL_PP(val_pp)))
#else
								!as_query_where(query_p, Z_STRVAL_P(bin_pp),
									as_contains(MAPVALUES , NUMERIC, Z_LVAL_P(val_pp)))
#endif
								) {
							DEBUG_PHP_EXT_DEBUG("Unable to set query predicate");
							PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
									"Unable to set query predicate");
						}
					} else if (

							#if PHP_VERSION_ID < 70000
								Z_LVAL_PP(index_type_pp
							#else
								Z_LVAL_P(index_type_pp
							#endif
						) == AS_INDEX_TYPE_MAPKEYS) {
						if (
#if PHP_VERSION_ID < 70000
								!as_query_where(query_p, Z_STRVAL_PP(bin_pp),
									as_contains(MAPKEYS , NUMERIC, Z_LVAL_PP(val_pp)))
#else
								!as_query_where(query_p, Z_STRVAL_P(bin_pp),
									as_contains(MAPKEYS , NUMERIC, Z_LVAL_P(val_pp)))
#endif
								) {
							DEBUG_PHP_EXT_DEBUG("Unable to set query predicate");
							PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
									"Unable to set query predicate");
						}
					} else if (

							#if PHP_VERSION_ID < 70000
								Z_LVAL_PP(index_type_pp
							#else
							  Z_LVAL_P(index_type_pp
							#endif
						) == AS_INDEX_TYPE_LIST) {
						if (
#if PHP_VERSION_ID < 70000
								!as_query_where(query_p, Z_STRVAL_PP(bin_pp),
									as_contains(LIST , NUMERIC, Z_LVAL_PP(val_pp)))
#else
								!as_query_where(query_p, Z_STRVAL_P(bin_pp),
									as_contains(LIST , NUMERIC, Z_LVAL_P(val_pp)))
#endif
								) {
							DEBUG_PHP_EXT_DEBUG("Unable to set query predicate");
							PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
									"Unable to set query predicate");
						}
					} else {
						DEBUG_PHP_EXT_DEBUG("Index type is invalid.");
						PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
								"Index type is invalid.");
						goto exit;
					}
					break;
				default:
					DEBUG_PHP_EXT_DEBUG("Predicate 'val' must be either string or integer.");
					PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
							"Predicate 'val' must be either string or integer.");
					goto exit;
			}
		} else if (
#if PHP_VERSION_ID < 70000
				strncmp(Z_STRVAL_PP(op_pp), "RANGE", 5) == 0
#else
				strncmp(Z_STRVAL_P(op_pp), "RANGE", 5) == 0
#endif
				) {
			if (
#if PHP_VERSION_ID < 70000
					(FAILURE == AEROSPIKE_ZEND_HASH_FIND(predicate_ht_p, INDEX_TYPE, sizeof(INDEX_TYPE),
										   (void **) &index_type_pp))
#else
					((index_type_pp = AEROSPIKE_ZEND_HASH_FIND(predicate_ht_p, INDEX_TYPE, sizeof(INDEX_TYPE),
						 					 (void **) &index_type_pp)) == NULL)
#endif
					) {
				DEBUG_PHP_EXT_DEBUG("Predicate is expected to include 'index_type'.");
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
					"Predicate is expected to include 'index_type'.");
				goto exit;
			}
			bool between_unpacked = false;
			if (
					#if PHP_VERSION_ID < 70000
						Z_TYPE_PP(val_pp
					#else
						Z_TYPE_P(val_pp
					#endif
				) == IS_ARRAY) {
			  convert_to_array_ex(val_pp);
			  convert_to_long_ex(index_type_pp);
#if PHP_VERSION_ID < 70000
				zval **min_pp;
				zval **max_pp;
#else
				zval *min_pp;
				zval *max_pp;
#endif
				if (
#if PHP_VERSION_ID < 70000
						(AEROSPIKE_ZEND_HASH_INDEX_FIND(Z_ARRVAL_PP(val_pp), 0, (void **) &min_pp) == SUCCESS) &&
						(AEROSPIKE_ZEND_HASH_INDEX_FIND(Z_ARRVAL_PP(val_pp), 1, (void **) &max_pp) == SUCCESS)
#else
						((min_pp = AEROSPIKE_ZEND_HASH_INDEX_FIND(Z_ARRVAL_P(val_pp), 0, (void **) &min_pp)) != NULL) &&
						((max_pp = AEROSPIKE_ZEND_HASH_INDEX_FIND(Z_ARRVAL_P(val_pp), 1, (void **) &max_pp)) != NULL)
#endif
						) {
          convert_to_long_ex(min_pp);
					convert_to_long_ex(max_pp);
					if (
							#if PHP_VERSION_ID < 70000
								Z_TYPE_PP(min_pp
							#else
								Z_TYPE_P(min_pp
							#endif
						) == IS_LONG &&
							#if PHP_VERSION_ID < 70000
									Z_TYPE_PP(max_pp
							#else
									Z_TYPE_P(max_pp
							#endif
						) == IS_LONG) {
						between_unpacked = true;
						if (

								#if PHP_VERSION_ID < 70000
										Z_LVAL_PP(index_type_pp
								#else
										Z_LVAL_P(index_type_pp
								#endif
							) == AS_INDEX_TYPE_MAPVALUES) {
							if (
#if PHP_VERSION_ID < 70000
									!as_query_where(query_p, Z_STRVAL_PP(bin_pp),
										as_range(MAPVALUES, NUMERIC, Z_LVAL_PP(min_pp), Z_LVAL_PP(max_pp)),
										AS_INDEX_TYPE_MAPVALUES)
#else
									!as_query_where(query_p, Z_STRVAL_P(bin_pp),
										as_range(MAPVALUES, NUMERIC, Z_LVAL_P(min_pp), Z_LVAL_P(max_pp)),
										AS_INDEX_TYPE_MAPVALUES)
#endif

									) {
								DEBUG_PHP_EXT_DEBUG("Unable to set query predicate");
								PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
										"Unable to set query predicate");
							}
						} else if (

								#if PHP_VERSION_ID < 70000
								  Z_LVAL_PP(index_type_pp
								#else
								  Z_LVAL_P(index_type_pp
								#endif
							) == AS_INDEX_TYPE_MAPKEYS) {
							if (
#if PHP_VERSION_ID < 70000
									!as_query_where(query_p, Z_STRVAL_PP(bin_pp),
										as_range(MAPKEYS, NUMERIC, Z_LVAL_PP(min_pp), Z_LVAL_PP(max_pp)),
										AS_INDEX_TYPE_MAPVALUES)
#else
									!as_query_where(query_p, Z_STRVAL_P(bin_pp),
										as_range(MAPKEYS, NUMERIC, Z_LVAL_P(min_pp), Z_LVAL_P(max_pp)),
										AS_INDEX_TYPE_MAPVALUES)
#endif
									) {
								DEBUG_PHP_EXT_DEBUG("Unable to set query predicate");
								PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
										"Unable to set query predicate");
							}
						} else if (

								#if PHP_VERSION_ID < 70000
									Z_LVAL_PP(index_type_pp
								#else
									Z_LVAL_P(index_type_pp
								#endif
							) == AS_INDEX_TYPE_LIST) {
							if (
#if PHP_VERSION_ID < 70000
									!as_query_where(query_p, Z_STRVAL_PP(bin_pp),
										as_range(LIST, NUMERIC, Z_LVAL_PP(min_pp), Z_LVAL_PP(max_pp)),
										AS_INDEX_TYPE_MAPVALUES)
#else
									!as_query_where(query_p, Z_STRVAL_P(bin_pp),
										as_range(LIST, NUMERIC, Z_LVAL_P(min_pp), Z_LVAL_P(max_pp)),
										AS_INDEX_TYPE_MAPVALUES)
#endif
									) {
								DEBUG_PHP_EXT_DEBUG("Unable to set query predicate");
								PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
										"Unable to set query predicate");
							}
						} else {
							DEBUG_PHP_EXT_DEBUG("Index type is invalid.");
							PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
									"Index type is invalid.");
							goto exit;
						}
					}
				}
			}
			if (!between_unpacked) {
				DEBUG_PHP_EXT_DEBUG("Predicate BETWEEN 'op' requires an array of (min,max) integers.");
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
						"Predicate BETWEEN 'op' requires an array of (min,max) integers.");
				goto exit;
			}
		} else {
			DEBUG_PHP_EXT_DEBUG("Unsupported 'op' in predicate");
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Unsupported 'op' in predicate");
			goto exit;
		}
	}

	if (module_p && function_p && (!as_query_apply(query_p, module_p,
					function_p, (as_list *) args_list_p))) {
		DEBUG_PHP_EXT_DEBUG("Unable to initiate UDF on the query");
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
				"Unable to initiate UDF on the query");
		goto exit;
	}

	PHP_EXT_SET_AS_ERR(error_p, DEFAULT_ERRORNO, DEFAULT_ERROR);

exit:
	return error_p->code;
}
/*
 ******************************************************************************************************
 * Check the progress of a background job running on the database.
 *
 * @param as_object_p           The C Client's aerospike object.
 * @param error_p               The C Client's as_error to be set to the
 *                              encountered error.
 * @param job_id                The id for the job, which can be used for
 *                              querying the status of the query.
 * @param job_info              Information about this job, to be populated
 *                              by this operation.
 * @param options_p             The optional policy.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status
aerospike_job_get_info(aerospike* as_object_p, as_error* error_p,
		uint64_t job_id, zval* job_info_p, char* module_p, zval* options_p TSRMLS_DC)
{
	as_job_info			 job_info;
	as_policy_info		  info_policy;

	set_policy(&as_object_p->config, NULL, NULL, NULL, NULL, &info_policy,
			NULL, NULL, NULL, options_p, error_p TSRMLS_CC);
	if (AEROSPIKE_OK != (error_p->code)) {
		DEBUG_PHP_EXT_DEBUG("Unable to set policy");
		goto exit;
	}

	if (AEROSPIKE_OK != (aerospike_job_info(as_object_p, error_p,
					&info_policy, module_p, job_id, false, &job_info))) {
		DEBUG_PHP_EXT_DEBUG("%s", error_p->message);
		goto exit;
	}

	add_assoc_long(job_info_p, PROGRESS_PCT, job_info.progress_pct);
	add_assoc_long(job_info_p, RECORDS_READ, job_info.records_read);
	add_assoc_long(job_info_p, STATUS, job_info.status);
exit:
	return error_p->code;
}
/*
 ******************************************************************************************************
 * Queries a set in the Aerospike DB and applies UDF on it.
 *
 * @param as_object_p               The C client's aerospike object.
 * @param error_p                   The C client's as_error to be set to the
 *                                  encountered error.
 * @param module_p                  The name of UDF module containing the
 *                                  function to execute.
 * @param function_p                The name of the function to be applies to
 *                                  the record.
 * @param args_pp                   An array of arguments for the UDF
 * @param namespace_p               The namespace to query.
 * @param set_p                     The set to query.
 * @param job_id_p                  The if for the query job, which can be used
 *                                  for querying the status of the query. This
 *                                  value shall be set by this function on
 *                                  success.
 * @param percent                   The percentage of data to scan.
 * @parama scan_priority            The priority levels for the scan operation.
 * @param concurrent                Whether to scan all nodes in parallel.
 * @param no_bins                   Whether to return only metadata (and no
 *                                  bins).
 * @param options_p                 The optional policy.
 * @param block                     Whether to block the query API until query
 *                                  job is completed or make as asynchronous
 *                                  call to scan and return ID.
 * @param serializer_policy_p       The serializer_policy value set in
 *                                  AerospikeObject structure.
 *                                  Value read from either INI or user provided
 *                                  options array.
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status
aerospike_query_run_background(Aerospike_object *as_object_p, as_error *error_p,
		char *module_p, char *function_p, zval **args_pp, char *namespace_p,
		char *set_p, HashTable *predicate_ht_p, zval *job_id_p, zval *options_p,
		bool block, int8_t *serializer_policy_p TSRMLS_DC)
{
	as_arraylist			args_list;
	as_arraylist*		   args_list_p = NULL;
	as_static_pool		  udf_pool = {0};
	int8_t				  serializer_policy = (serializer_policy_p) ? *serializer_policy_p : SERIALIZER_NONE;
	as_policy_write		 write_policy;
	as_policy_info		  info_policy;
	as_query				query;
	as_query*			   query_p = NULL;
	uint64_t				query_id = 0;

	if ((!as_object_p->as_ref_p->as_p) || (!error_p) || (!module_p) || (!function_p) ||
			(!namespace_p) || (!set_p) || (!job_id_p)) {
		DEBUG_PHP_EXT_DEBUG("Unable to initiate background query");
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to initiate background query");
		goto exit;
	}

	if ((*args_pp)) {
		#if PHP_VERSION_ID < 70000
		  as_arraylist_inita(&args_list,
				  zend_hash_num_elements(Z_ARRVAL_PP(args_pp)));
		#else
		  as_arraylist_inita(&args_list,
				  zend_hash_num_elements(Z_ARRVAL_P((zval*) *args_pp)));
		#endif
		args_list_p = &args_list;
		AS_LIST_PUT(as_object_p, NULL, args_pp, args_list_p, &udf_pool,
				serializer_policy, error_p TSRMLS_CC);
		if (AEROSPIKE_OK != (error_p->code)) {
			DEBUG_PHP_EXT_DEBUG("Unable to create args list for UDF");
			goto exit;
		}
	}

	query_p = &query;
	as_query_init(query_p, namespace_p, set_p);
	if (predicate_ht_p && zend_hash_num_elements(predicate_ht_p) != 0) {
		as_query_where_inita(&query, 1);
	}

	if (AEROSPIKE_OK != (aerospike_query_define(&query, error_p, namespace_p,
					set_p, predicate_ht_p, NULL, NULL, NULL TSRMLS_CC))) {
		DEBUG_PHP_EXT_DEBUG("Unable to define scan");
		goto exit;
	}

	set_policy_query_apply(&as_object_p->as_ref_p->as_p->config, &write_policy, options_p, error_p TSRMLS_CC);

	if (AEROSPIKE_OK != (error_p->code)) {
		DEBUG_PHP_EXT_DEBUG("Unable to set policy");
		goto exit;
	}

	if (module_p && function_p && (!as_query_apply(query_p, module_p,
					function_p, (as_list*)args_list_p))) {
		DEBUG_PHP_EXT_DEBUG("Unable to apply UDF on the query");
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
				"Unable to initiate background query");
		goto exit;
	}

	if (AEROSPIKE_OK != (aerospike_query_background(as_object_p->as_ref_p->as_p,
					error_p, &write_policy, query_p, &query_id))) {
		DEBUG_PHP_EXT_DEBUG("%s", error_p->message);
		goto exit;
	}

	if (block) {
		set_policy(&as_object_p->as_ref_p->as_p->config, NULL, NULL, NULL, NULL, &info_policy,
				NULL, NULL, NULL, options_p, error_p TSRMLS_CC);

		if (AEROSPIKE_OK != (error_p->code)) {
			DEBUG_PHP_EXT_DEBUG("Unable to set policy");
			goto exit;
		}

		if (AEROSPIKE_OK != aerospike_query_wait(as_object_p->as_ref_p->as_p,
					error_p, &info_policy, query_p, query_id, 0)) {
			DEBUG_PHP_EXT_DEBUG("%s", error_p->message);
			goto exit;
		}
	}
	ZVAL_LONG(job_id_p, query_id);

exit:
	if (args_list_p) {
		as_arraylist_destroy(args_list_p);
	}

	if (query_p) {
		as_query_destroy(query_p);
	}
	aerospike_helper_free_static_pool(&udf_pool);
	return error_p->code;
}

/*
 ******************************************************************************************************
 Executes a query in the Aerospike DB.
 *
 * @param as_object_p               The C client's aerospike object.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param namespace_p               The namespace to scan.
 * @param set_p                     The set to scan.
 * @param user_func_p               The user's callback to be applied per record
 *                                  that is scanned.
 * @param bins_ht_p                 The HashTable for optional filter bins array.
 * @param predicate_p               The HashTable for Query Predicate array.
 * @param options_p                 The optional policy.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status
aerospike_query_run(aerospike* as_object_p, as_error* error_p, char* namespace_p,
		char* set_p, userland_callback* user_func_p, HashTable* bins_ht_p,
		HashTable* predicate_ht_p, zval* options_p TSRMLS_DC)
{
  as_query			query;
	bool				is_init_query = false;
	as_policy_query	 query_policy;

	if ((!as_object_p) || (!error_p) || (!namespace_p)) {
		DEBUG_PHP_EXT_DEBUG("Unable to initiate query");
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to initiate query");
		goto exit;
	}
	set_policy(&as_object_p->config, NULL, NULL, NULL, NULL, NULL, NULL,
			&query_policy, NULL, options_p, error_p TSRMLS_CC);
	if (AEROSPIKE_OK != (error_p->code)) {
		DEBUG_PHP_EXT_DEBUG("Unable to set policy");
		goto exit;
	}
	as_query_init(&query, namespace_p, set_p);
	is_init_query = true;
	if (predicate_ht_p && zend_hash_num_elements(predicate_ht_p) != 0) {
		as_query_where_inita(&query, 1);
	}
	if (AEROSPIKE_OK != (aerospike_query_define(&query, error_p, namespace_p,
					set_p, predicate_ht_p, NULL, NULL, NULL TSRMLS_CC))) {
		DEBUG_PHP_EXT_DEBUG("Unable to define scan");
		goto exit;
	}

	if (bins_ht_p) {
		as_query_select_inita(&query, zend_hash_num_elements(bins_ht_p));
		HashPosition pos;
		#if PHP_VERSION_ID < 70000
      zval** bin_names_pp = NULL;
			AEROSPIKE_FOREACH_HASHTABLE(bins_ht_p, pos, bin_names_pp) {
				if (AEROSPIKE_Z_TYPE_P(bin_names_pp) != IS_STRING) {
					convert_to_string_ex(bin_names_pp);
				}
		#else
      zval* bin_names_pp = NULL;
			zend_string* z_str;
			ZEND_HASH_FOREACH_KEY_VAL(bins_ht_p, pos, z_str, bin_names_pp) {
			  if (!z_str) {
						convert_to_string_ex(bin_names_pp);
					}
		#endif

			if (!as_query_select(&query, AEROSPIKE_Z_STRVAL_P(bin_names_pp))) {
				DEBUG_PHP_EXT_DEBUG("Unable to apply filter bins to the query");
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
						"Unable to apply filter bins to the query");
				goto exit;
			}
		#if PHP_VERSION_ID < 70000
      }
		#else
      } ZEND_HASH_FOREACH_END();
		#endif
		if (AEROSPIKE_OK != (aerospike_query_foreach(as_object_p, error_p,
						&query_policy, &query,
						aerospike_helper_record_stream_callback,
						user_func_p))) {
			DEBUG_PHP_EXT_DEBUG("%s", error_p->message);
			goto exit;
		}
	} else if (AEROSPIKE_OK != (aerospike_query_foreach(as_object_p, error_p,
					NULL, &query, aerospike_helper_record_stream_callback,
					user_func_p))) {
		DEBUG_PHP_EXT_DEBUG("%s", error_p->message);
		goto exit;
	}
exit:
	if (is_init_query) {
		as_query_destroy(&query);
	}
	return error_p->code;
}

/*
 ******************************************************************************************************
 Executes a query aggregation in the Aerospike DB by applying the UDF.
 *
 * @param as_object_p               The C client's aerospike object.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param module_p                  The name of UDF module containing the function
 *                                  to execute.
 * @param function_p                The name of the function to be applied to
 *                                  the record.
 * @param args_pp                   An array of arguments for the UDF.
 * @param namespace_p               The namespace to scan.
 * @param set_p                     The set to scan.
 * @param bins_ht_p                 The HashTable for optional filter bins array.
 * @param predicate_p               The HashTable for Query Predicate array.
 * @param return_value_p            The return value of aggregation to be
 *                                  populated by this method.
 * @param options_p                 The optional policy.
 * @param serializer_policy_p       The serializer_policy value set in AerospikeObject structure.
 *                                  Either an INI read value or value from user provided options array.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status
aerospike_query_aggregate(Aerospike_object* as_object_p, as_error* error_p,
		const char* module_p, const char* function_p,
		#if PHP_VERSION_ID < 70000
			zval** args_pp
		#else
			zval* args_pp
		#endif
		,
		char* namespace_p, char* set_p, HashTable* bins_ht_p,
		HashTable* predicate_ht_p, zval* return_value_p,
		zval* options_p, int8_t* serializer_policy_p  TSRMLS_DC)
{
	as_arraylist                args_list;
	as_arraylist*               args_list_p = NULL;
	as_static_pool              udf_pool = {0};
	int8_t                      serializer_policy = (serializer_policy_p) ? *serializer_policy_p : SERIALIZER_NONE;
	as_policy_query             query_policy;
	as_query                    query;
	bool                        is_init_query = false;
	foreach_callback_udata      aggregate_result_callback_udata;
	bool                        return_value_assoc = false;

	if ((!as_object_p->as_ref_p->as_p) || (!error_p) || (!module_p) || (!function_p) ||
			(!args_pp && (!(
				#if PHP_VERSION_ID < 70000
					*args_pp
				#else
					args_pp
				#endif
			))) || (!namespace_p) || (!set_p) ||
			(!predicate_ht_p) || (!return_value_p)) {
		DEBUG_PHP_EXT_DEBUG("Unable to initiate query aggregation");
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to initiate query aggregation");
		goto exit;
	}

	set_policy(&as_object_p->as_ref_p->as_p->config, NULL, NULL, NULL, NULL, NULL, NULL, &query_policy,
		&serializer_policy, options_p, error_p TSRMLS_CC);
	if (AEROSPIKE_OK != (error_p->code)) {
		DEBUG_PHP_EXT_DEBUG("Unable to set policy");
		goto exit;
	}

#if PHP_VERSION_ID < 70000
    if ((*args_pp)) {
		as_arraylist_init(&args_list,
				zend_hash_num_elements(Z_ARRVAL_PP(args_pp)), 0);
#else
    if ((args_pp)) {
		as_arraylist_init(&args_list,
				zend_hash_num_elements(Z_ARRVAL_P(args_pp)), 0);
#endif
		args_list_p = &args_list;
		AS_LIST_PUT(as_object_p, NULL, args_pp, &args_list, &udf_pool,
				serializer_policy, error_p TSRMLS_CC);
		if (AEROSPIKE_OK != (error_p->code)) {
			DEBUG_PHP_EXT_DEBUG("Unable to create args list for UDF");
			goto exit;
		}
	}

	if (NULL == as_query_init(&query, namespace_p, set_p)) {
		DEBUG_PHP_EXT_DEBUG("Unable to initialize a query");
		error_p->code = AEROSPIKE_ERR_CLIENT;
		goto exit;
	}

	is_init_query = true;
	if (predicate_ht_p && zend_hash_num_elements(predicate_ht_p) != 0) {
		as_query_where_inita(&query, 1);
	}

	if (AEROSPIKE_OK != (aerospike_query_define(&query, error_p, namespace_p,
					set_p, predicate_ht_p, module_p, function_p,
					args_list_p TSRMLS_CC))) {
		DEBUG_PHP_EXT_DEBUG("Unable to define query");
		goto exit;
	}

	return_value_assoc = true;
	aggregate_result_callback_udata.udata_p = return_value_p;
	aggregate_result_callback_udata.error_p = error_p;
	aggregate_result_callback_udata.obj	 = as_object_p;

	if (bins_ht_p) {
		as_query_select_inita(&query, zend_hash_num_elements(bins_ht_p));
		HashPosition pos;
		#if PHP_VERSION_ID < 70000
      zval **bin_names_pp = NULL;
		#else
      zval *bin_names_pp = NULL;
		#endif
		AEROSPIKE_FOREACH_HASHTABLE(bins_ht_p, pos, bin_names_pp) {
			if (
					#if PHP_VERSION_ID < 70000
					  Z_TYPE_PP(bin_names_pp
					#else
						Z_TYPE_P(bin_names_pp
					#endif
				) != IS_STRING) {
				convert_to_string_ex(bin_names_pp);
			}
			if (
#if PHP_VERSION_ID < 70000
					!as_query_select(&query, Z_STRVAL_PP(bin_names_pp))
#else
					!as_query_select(&query, Z_STRVAL_P(bin_names_pp))
#endif
					) {
				DEBUG_PHP_EXT_DEBUG("Unable to apply filter bins to the query");
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
						"Unable to apply filter bins to the query");
				goto exit;
			}
		}

		if (AEROSPIKE_OK != (aerospike_query_foreach(as_object_p->as_ref_p->as_p, error_p,
						&query_policy, &query,
						aerospike_helper_aggregate_callback,
						&aggregate_result_callback_udata))) {
			DEBUG_PHP_EXT_DEBUG("%s", error_p->message);
			goto exit;
		}
	} else if (AEROSPIKE_OK != (aerospike_query_foreach(as_object_p->as_ref_p->as_p, error_p,
					&query_policy, &query, aerospike_helper_aggregate_callback,
					&aggregate_result_callback_udata))) {
		DEBUG_PHP_EXT_DEBUG("%s", error_p->message);
		goto exit;
	}

exit:
	if (args_list_p) {
		as_arraylist_destroy(args_list_p);
	}

	if (is_init_query) {
		as_query_destroy(&query);
	}

	if (error_p->code == AEROSPIKE_ERR_CLIENT) {
		if (return_value_p && !return_value_assoc) {
			zval_dtor(return_value_p);
		}
	}

	aerospike_helper_free_static_pool(&udf_pool);
	return error_p->code;
}
