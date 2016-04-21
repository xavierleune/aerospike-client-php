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
#include "php_aerospike.h"
#include "aerospike/as_log.h"
#include "aerospike/as_key.h"
#include "aerospike/as_config.h"
#include "aerospike/as_error.h"
#include "aerospike/as_status.h"
#include "aerospike/as_record.h"
#include "aerospike/aerospike.h"
#include "pthread.h"
#include "aerospike_common.h"

#define SAVE_PATH_DELIMITER "|"
#define IP_PORT_DELIMITER ":"
#define HOST_DELIMITER ","

/*
 *******************************************************************************************************
 * PHP Userland logger callback.
 *******************************************************************************************************
 */
zend_fcall_info       func_call_info;
zend_fcall_info_cache func_call_info_cache;
uint32_t              is_callback_registered;

#if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
zval                  *func_callback_retval_p = NULL;
#else
zval                  func_callback_retval_p;
#endif

/*
 *******************************************************************************************************
 * aerospike-client-php global log level
 *******************************************************************************************************
 */
#ifndef __AEROSPIKE_PHP_CLIENT_LOG_LEVEL__
as_log_level php_log_level_set = PHP_EXT_AS_LOG_LEVEL_OFF;
#else
as_log_level php_log_level_set = __AEROSPIKE_PHP_CLIENT_LOG_LEVEL__;
#endif

/*
 *******************************************************************************************************
 * Callback for C client's logger.
 * This function shall be invoked by:
 * 1. C client's logger statements.
 * 2. PHP client's logger statements.
 *
 * @param level             The as_log_level to be used by the callback.
 * @param func              The function name generating the log.
 * @param file              The file name containing the func generating the log.
 * @param line              The line number in file where the log was generated.
 * @param fmt               The format specifier for logger.
 *
 * @return true if log callback succeeds. Otherwise false.
 *******************************************************************************************************
 */
extern bool
aerospike_helper_log_callback(as_log_level level, const char * func TSRMLS_DC, const char * file, uint32_t line, const char * fmt, ...)
{
    if (level & 0x08) {
		char msg[1024] = {0};
		va_list ap;

		va_start(ap, fmt);
		vsnprintf(msg, 1024, fmt, ap);
		msg[1023] = '\0';
		va_end(ap);
		if (!is_callback_registered) {
			fprintf(stderr, "PHP EXTn: level %d func %s file %s line %d msg %s \n", level, func, file, line, msg);
		}
	}

	if (is_callback_registered) {
		INVOKE_CALLBACK_FUNCTION(level, func, file, line);
	}

	return true;
}

/*
 *******************************************************************************************************
 * Sets the private members error and errorno of the Aerospike class.
 *
 * @param ce_p              The zend_class_entry pointer for the Aerospike class.
 * @param object_p          The Aerospike object.
 * @param error_p           The as_error containing the recent error details.
 * @param reset_flag        The flag indicating whether to set/reset the class error.
 *
 *******************************************************************************************************
 */
extern void
aerospike_helper_set_error(zend_class_entry *ce_p, zval *object_p TSRMLS_DC)
{
	aerospike_global_error error_t = AEROSPIKE_G(error_g);
#if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
	zval*    err_code_p = NULL;
	zval*    err_msg_p = NULL;

	MAKE_STD_ZVAL(err_code_p);
	MAKE_STD_ZVAL(err_msg_p);

	if (error_t.reset) {
		AEROSPIKE_ZVAL_STRINGL(err_msg_p, DEFAULT_ERROR, strlen(DEFAULT_ERROR), 1);
		ZVAL_LONG(err_code_p, DEFAULT_ERRORNO);
	} else {
		AEROSPIKE_ZVAL_STRINGL(err_msg_p, error_t.error.message, strlen(error_t.error.message), 1);
		ZVAL_LONG(err_code_p, error_t.error.code);
	}

	zend_update_property(ce_p, object_p, "error", strlen("error"), err_msg_p TSRMLS_CC);
	zend_update_property(ce_p, object_p, "errorno", strlen("errorno"), err_code_p TSRMLS_CC);
#else
	zval    err_code_p;
	zval    err_msg_p;

	array_init(&err_code_p);
  array_init(&err_msg_p);
	if (error_t.reset) {
    AEROSPIKE_ZVAL_STRINGL(&err_msg_p, DEFAULT_ERROR, strlen(DEFAULT_ERROR), 1);
		ZVAL_LONG(&err_code_p, DEFAULT_ERRORNO);
	} else {
    AEROSPIKE_ZVAL_STRINGL(&err_msg_p, error_t.error.message, strlen(error_t.error.message), 1);
		ZVAL_LONG(&err_code_p, error_t.error.code);
	}

  zend_update_property(ce_p, object_p, "error", strlen("error"), &err_msg_p TSRMLS_CC);
	zend_update_property(ce_p, object_p, "errorno", strlen("errorno"), &err_code_p TSRMLS_CC);
#endif

  #if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
    AEROSPIKE_ZVAL_PTR_DTOR(err_code_p);
    AEROSPIKE_ZVAL_PTR_DTOR(err_msg_p);
  #else
    AEROSPIKE_ZVAL_PTR_DTOR(&err_code_p);
    AEROSPIKE_ZVAL_PTR_DTOR(&err_msg_p);
  #endif
}

/*
 *******************************************************************************************************
 * This macro is defined to create a new C client's aerospike object.
 *******************************************************************************************************
 */
#define ZEND_CREATE_AEROSPIKE_REFERENCE_OBJECT()                              \
do {                                                                          \
    if (NULL != (as_object_p->as_ref_p = pemalloc(sizeof(aerospike_ref), 1))) \
    {                                                                         \
        as_object_p->as_ref_p->as_p = NULL;                                   \
        as_object_p->as_ref_p->ref_as_p = 0;                                  \
        as_object_p->as_ref_p->ref_hosts_entry = 0;                           \
    }                                                                         \
    as_object_p->as_ref_p->as_p = aerospike_new(conf);                        \
    as_object_p->as_ref_p->ref_as_p = 1;                                      \
} while(0)

/*
 *******************************************************************************************************
 * This macro is defined to register a new resource and to add hash to it.
 *******************************************************************************************************
 */
#if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
#define ZEND_HASH_CREATE_ALIAS_NEW(alias, alias_len, new_flag)                 \
do {                                                                           \
    ZEND_CREATE_AEROSPIKE_REFERENCE_OBJECT();                                  \
    ZEND_REGISTER_RESOURCE(rsrc_result, as_object_p->as_ref_p->as_p,           \
            val_persist);                                                      \
    new_le.ptr = as_object_p->as_ref_p;                                        \
    new_le.type = val_persist;                                                 \
    if (new_flag) {                                                            \
        pthread_rwlock_wrlock(&AEROSPIKE_G(aerospike_mutex));                  \
        zend_hash_add(persistent_list, alias, alias_len,                       \
                (void *) &new_le, sizeof(zend_rsrc_list_entry), NULL);         \
        ((aerospike_ref *) new_le.ptr)->ref_hosts_entry++;                     \
        pthread_rwlock_unlock(&AEROSPIKE_G(aerospike_mutex));                  \
    } else {                                                                   \
        pthread_rwlock_wrlock(&AEROSPIKE_G(aerospike_mutex));                  \
        zend_hash_update(persistent_list,                                      \
                alias, alias_len, (void *) &new_le,                            \
                sizeof(zend_rsrc_list_entry), (void **) &le);                  \
        ((aerospike_ref *) new_le.ptr)->ref_hosts_entry++;                     \
        pthread_rwlock_unlock(&AEROSPIKE_G(aerospike_mutex));                  \
    }                                                                          \
} while(0)
#else
/*
 * Required or not??
 * le = (zend_resource *) zend_hash_update(persistent_list, alias, &new_le);
 * */
#define ZEND_HASH_CREATE_ALIAS_NEW(alias, alias_len, new_flag)                 \
do {                                                                           \
    ZEND_CREATE_AEROSPIKE_REFERENCE_OBJECT();                                  \
    ZVAL_RES(&rsrc_result, zend_register_resource(as_object_p->as_ref_p->as_p, \
            val_persist));                                                     \
    new_le.ptr = as_object_p->as_ref_p;                                        \
    new_le.type = val_persist;                                                 \
    if (new_flag) {                                                            \
        pthread_rwlock_wrlock(&AEROSPIKE_G(aerospike_mutex));                  \
        zend_hash_add_new_ptr(persistent_list,                                 \
                zend_string_init(alias, alias_len, 0), (void *) &new_le);      \
        ((aerospike_ref *) new_le.ptr)->ref_hosts_entry++;                     \
        pthread_rwlock_unlock(&AEROSPIKE_G(aerospike_mutex));                  \
    } else {                                                                   \
        pthread_rwlock_wrlock(&AEROSPIKE_G(aerospike_mutex));                  \
        zend_hash_update(persistent_list, zend_string_init(alias,              \
                    strlen(alias), 0), (zval*) &new_le);                       \
        ((aerospike_ref *) new_le.ptr)->ref_hosts_entry++;                     \
        pthread_rwlock_unlock(&AEROSPIKE_G(aerospike_mutex));                  \
    }                                                                          \
} while(0)
#endif
/*
 *******************************************************************************************************
 * This macro is defined to match the config details with the stored object
 * details in the resource and if match use the existing one.
 *******************************************************************************************************
 */
#define ZEND_CONFIG_MATCH_USER_STORED(alias, alias_len)                       \
do {                                                                          \
    tmp_ref = le->ptr;                                                        \
    for(itr_user=0; itr_user < conf->hosts_size; itr_user++ ) {               \
        for(itr_stored=0; itr_stored < tmp_ref->as_p->config.hosts_size;      \
                itr_stored++) {                                               \
            if(strlen(conf->hosts[itr_user].addr) ==                          \
                    strlen(tmp_ref->as_p->config.hosts[itr_stored].addr) &&   \
                    (conf->hosts[itr_user].port ==                            \
                     tmp_ref->as_p->config.hosts[itr_stored].port &&          \
                     (strncasecmp(conf->hosts[itr_user].addr,                 \
                                  tmp_ref->as_p->config.hosts[                \
                                  itr_stored].addr,                           \
                                  strlen(conf->hosts[itr_user].addr           \
                                      )) == 0))) {                            \
                goto use_existing;                                            \
            }                                                                 \
        }                                                                     \
    }                                                                         \
    ZEND_HASH_CREATE_ALIAS_NEW(alias, alias_len, 0);                          \
} while(0)

#define MAX_PORT_SIZE 6

/*
 *******************************************************************************************************
 * This function will check if shm_key is unique or not.
 *
 * @param shm_key_to_match          The shm_key value to match.
 * @param shm_key_list              The hashtable pointing to the zend global shm key list.
 *
 * @returns 1 if paased shm key is unique. Otherwise 0.
 *******************************************************************************************************
 */
int is_unique_shm_key(int shm_key_to_match, HashTable *shm_key_list TSRMLS_DC)
{
	HashPosition        options_pointer;
	void*              options_value;

  #if PHP_VERSION_ID < 70000
    for (zend_hash_internal_pointer_reset_ex(shm_key_list, &options_pointer);
        zend_hash_get_current_data_ex(shm_key_list,
          (void **) &options_value, &options_pointer) == SUCCESS;
        zend_hash_move_forward_ex(shm_key_list, &options_pointer)) {
      if (shm_key_to_match == ((shared_memory_key *)(((zend_rsrc_list_entry*)options_value)->ptr))->key) {
        return 0;
      }
    }
  #else
    zval * data;
    ZEND_HASH_FOREACH_VAL(shm_key_list, data) {
      if (shm_key_to_match == ((shared_memory_key *)(((zend_resource*)options_value)->ptr))->key) {
        return 0;
      }
    } ZEND_HASH_FOREACH_END();
  #endif
	return 1;
}

/*
 *******************************************************************************************************
 * Function to set shm_key value from paased config and if it's not unique then it will
 * generate a new unique shm key.
 *
 * @param conf                      The as_config to be used for creating/retrieving aerospike object
 * @param shm_key_list              The hashtable pointing to the zend global shm key list
 * @param shm_key_counter           shm_key generator counter
 *
 * @return AEROSPIKE::OK if success. Otherwise AEROSPIKE_x.
 *******************************************************************************************************
 */
static as_status
set_shm_key_from_alias_hash_or_generate(
										as_config* conf,
										HashTable *shm_key_list,
										int* shm_key_counter TSRMLS_DC)
{
  #if PHP_VERSION_ID < 70000
    zend_rsrc_list_entry *le, new_shm_entry;
    zval* rsrc_result = NULL;
  #else
    zend_resource *le, new_shm_entry;
    zval rsrc_result;
  #endif
	as_status status = AEROSPIKE_OK;
	int itr_user = 0;
	char *alias_to_search = NULL;
	char port[MAX_PORT_SIZE];
	int alias_length = 0;
	shared_memory_key *shm_key_ptr = NULL;

	for (itr_user=0; itr_user < conf->hosts_size; itr_user++) {
		alias_length += strlen(conf->hosts[itr_user].addr) + strlen(conf->user) + MAX_PORT_SIZE + 3;
	}

	alias_to_search =(char*) emalloc(alias_length);
	memset( alias_to_search, '\0', alias_length);
	for (itr_user=0; itr_user < conf->hosts_size; itr_user++) {
		sprintf(port, "%d", conf->hosts[itr_user].port);
		strcat(alias_to_search, conf->hosts[itr_user].addr);
		strcat(alias_to_search, ":");
		strcat(alias_to_search, port);
		strcat(alias_to_search, ":");
		strcat(alias_to_search, conf->user);
		if (itr_user != conf->hosts_size - 1) {
			strcat(alias_to_search, ";");
		}
	}
	pthread_rwlock_rdlock(&AEROSPIKE_G(aerospike_mutex));
	if (zend_hash_num_elements(shm_key_list) == 0) {
		shm_key_ptr = pemalloc(sizeof(shared_memory_key), 1);
		shm_key_ptr->key = conf->shm_key;
    #if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
      ZEND_REGISTER_RESOURCE(rsrc_result, shm_key_ptr, 1);
    #else
      ZVAL_RES(&rsrc_result, zend_register_resource(shm_key_ptr, 1));
    #endif
		new_shm_entry.ptr = shm_key_ptr;
		new_shm_entry.type = 1;
    #if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
      zend_hash_add(shm_key_list, alias_to_search, strlen(alias_to_search),
        (void *) &new_shm_entry, sizeof(zend_rsrc_list_entry*), NULL);
    #else
      zend_hash_add_new_ptr(shm_key_list,                                 \
        zend_string_init(alias_to_search, strlen(alias_to_search), 0), (void *) &new_shm_entry);
    #endif
		pthread_rwlock_unlock(&AEROSPIKE_G(aerospike_mutex));
		goto exit;
	}
 #if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
   if (AEROSPIKE_ZEND_HASH_FIND(shm_key_list, alias_to_search,
  					strlen(alias_to_search), (void **) &le) == SUCCESS) {
 #else
   if (NULL != (le = (zend_resource *) AEROSPIKE_ZEND_HASH_FIND(shm_key_list,
      alias_to_search, strlen(alias_to_search), (void **) &le))) {
 #endif
		if ((le->ptr) != NULL) {
			conf->shm_key = ((shared_memory_key *)(le->ptr))->key;
		}
	} else {
		if (!(is_unique_shm_key(conf->shm_key, shm_key_list TSRMLS_CC))) {
			while (true) {
				//generating unique shm_key
				if (is_unique_shm_key(*shm_key_counter, shm_key_list TSRMLS_CC)) {
					conf->shm_key = *shm_key_counter;
					break;
				} else{
					(*shm_key_counter)++;
				}
			}
		}

		shm_key_ptr = pemalloc(sizeof(shared_memory_key), 1);
		shm_key_ptr->key = conf->shm_key;

    #if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
      ZEND_REGISTER_RESOURCE(rsrc_result, shm_key_ptr, 1);
    #else
      ZVAL_RES(&rsrc_result, zend_register_resource(shm_key_ptr, 1));
    #endif

		new_shm_entry.ptr = shm_key_ptr;
		new_shm_entry.type = 1;
    #if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
      zend_hash_add(shm_key_list, alias_to_search, strlen(alias_to_search),
        (void *) &new_shm_entry, sizeof(zend_rsrc_list_entry*), NULL);
    #else
      zend_hash_add_new_ptr(shm_key_list,                                 \
        zend_string_init(alias_to_search, strlen(alias_to_search), 0), (void *) &new_shm_entry);
    #endif
	}
	pthread_rwlock_unlock(&AEROSPIKE_G(aerospike_mutex));
	if (alias_to_search) {
		efree(alias_to_search);
		alias_to_search = NULL;
	}
exit:
	if (alias_to_search) {
		efree(alias_to_search);
		alias_to_search = NULL;
	}
	return (status);
}

/*
 *******************************************************************************************************
 * Function to retrieve a C Client's aerospike object either from the zend
 * persistent store if an already hashed object (with the addr+port as the hash) exists, or by
 * creating a new aerospike object if it doesn't and pushing it on the zend persistent store
 * for further reuse.
 *
 * @param as_object_p               The instance of Aerospike_object structure containing
 *                                  the C Client's aerospike object.
 * @param persist_flag              The flag which indicates whether to persist the C Client's
 *                                  aerospike object.
 * @param conf                      The as_config to be used for creating/retrieving aerospike object.
 * @param persistent_list           The hashtable pointing to the zend global persistent list.
 * @param val_persist               The resource handler for persistent list.
 * @param shm_key_list              The hashtable pointing to the zend global shm key list.
 *
 * @return AEROSPIKE::OK if success. Otherwise AEROSPIKE_x.
 *******************************************************************************************************
 */
extern as_status
aerospike_helper_object_from_alias_hash(Aerospike_object* as_object_p,
										bool persist_flag,
										as_config* conf,
										HashTable *shm_key_list,
										HashTable *persistent_list,
										int val_persist TSRMLS_DC)
{
#if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
	zend_rsrc_list_entry *le, new_le;
	zval* rsrc_result = NULL;
#else
	zend_resource *le, new_le;
	zval rsrc_result;
	array_init(&rsrc_result);
#endif
	as_status status = AEROSPIKE_OK;
	int itr_user = 0, itr_stored = 0;
	aerospike_ref *tmp_ref = NULL;
	char *alias_to_search = NULL;
	char *alias_to_hash = NULL;
	char port[MAX_PORT_SIZE];
	static int shm_key_counter = 0xA5000000;

	if (!(as_object_p) && !(conf)) {
		status = AEROSPIKE_ERR_PARAM;
		goto exit;
	}

	if (persist_flag == false) {
		ZEND_CREATE_AEROSPIKE_REFERENCE_OBJECT();
		goto exit;
	}

	/*
	 * Iterate over list of hosts and check if the one of them is already
	 * hashed and can be reused.
	 */

	for (itr_user=0; itr_user < conf->hosts_size; itr_user++) {
		alias_to_search = (char*) emalloc(strlen(conf->hosts[itr_user].addr) + strlen(conf->user) + MAX_PORT_SIZE + 2);
		sprintf(port, "%d", conf->hosts[itr_user].port);
		strcpy(alias_to_search, conf->hosts[itr_user].addr);
		strcat(alias_to_search, ":");
		strcat(alias_to_search, port);
		strcat(alias_to_search, ":");
		strcat(alias_to_search, conf->user);
		pthread_rwlock_rdlock(&AEROSPIKE_G(aerospike_mutex));
#if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
		if (AEROSPIKE_ZEND_HASH_FIND(persistent_list, alias_to_search,
					strlen(alias_to_search), (void **) &le) == SUCCESS) {
			if (alias_to_search) {
				efree(alias_to_search);
				alias_to_search = NULL;
			}
			pthread_rwlock_unlock(&AEROSPIKE_G(aerospike_mutex));
			tmp_ref = le->ptr;
			goto use_existing;
		}
#else
		if (NULL != (le = (zend_resource *) AEROSPIKE_ZEND_HASH_FIND(persistent_list,
						alias_to_search, strlen(alias_to_search), (void **) &le))) {
			if (alias_to_search) {
				efree(alias_to_search);
				alias_to_search = NULL;
			}
			pthread_rwlock_unlock(&AEROSPIKE_G(aerospike_mutex));
			tmp_ref = le->ptr;
			goto use_existing;
		}
#endif
		pthread_rwlock_unlock(&AEROSPIKE_G(aerospike_mutex));
		if (alias_to_search) {
			efree(alias_to_search);
			alias_to_search = NULL;
		}
	}

	alias_to_search = (char*) emalloc(strlen(conf->hosts[0].addr) + strlen(conf->user) + MAX_PORT_SIZE + 2);
	sprintf(port, "%d", conf->hosts[0].port);
	strcpy(alias_to_search, conf->hosts[0].addr);
	strcat(alias_to_search, ":");
	strcat(alias_to_search, port);
	strcat(alias_to_search, ":");
	strcat(alias_to_search, conf->user);
	ZEND_HASH_CREATE_ALIAS_NEW(alias_to_search, strlen(alias_to_search), 1);

	/*
	 * Iterate over remaining list of hosts and hash them into the persistent
	 * list, each pointing to the same aerospike_ref object.
	 * Increment corresponding ref_hosts_entry within the aerospike_ref object.
	 */

	for (itr_user=1; itr_user < conf->hosts_size; itr_user++ ) {
		alias_to_hash = (char*) emalloc(strlen(conf->hosts[itr_user].addr) + strlen(conf->user) + MAX_PORT_SIZE + 2);
		sprintf(port, "%d", conf->hosts[itr_user].port);
		strcpy(alias_to_hash, conf->hosts[itr_user].addr);
		strcat(alias_to_hash, ":");
		strcat(alias_to_hash, port);
		strcat(alias_to_hash, ":");
		strcat(alias_to_hash, conf->user);
		pthread_rwlock_wrlock(&AEROSPIKE_G(aerospike_mutex));
#if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
		zend_hash_add(persistent_list, alias_to_hash,
				strlen(alias_to_hash), (void *) &new_le, sizeof(zend_rsrc_list_entry), NULL);
#else
		zend_hash_add_new(persistent_list, zend_string_init(alias_to_hash, strlen(alias_to_hash), 0),
				(zval *) &new_le);
#endif
		((aerospike_ref *) new_le.ptr)->ref_hosts_entry++;
		pthread_rwlock_unlock(&AEROSPIKE_G(aerospike_mutex));
		efree(alias_to_hash);
		alias_to_hash = NULL;
	}
	goto exit;

use_existing:
	/*
	 * config details have matched, use the existing one obtained from the
	 * storage.
	 * Increment corresponding ref_as_p of the aerospike_ref object.
	 */
	as_object_p->is_conn_16 = AEROSPIKE_CONN_STATE_TRUE;
	as_object_p->as_ref_p = tmp_ref;
	as_object_p->as_ref_p->ref_as_p++;
	goto exit;
exit:
	if (conf->use_shm) {
		status = set_shm_key_from_alias_hash_or_generate(conf,shm_key_list,
				&shm_key_counter TSRMLS_CC);
	}

	if (alias_to_search) {
		efree(alias_to_search);
		alias_to_search = NULL;
	}
	return (status);
}

/*
 *******************************************************************************************************
 * Function to destroy all as_* types initiated within the as_static_pool.
 * To be called if as_static_pool has been initialized after the use of pool is
 * complete.
 *
 * @param static_pool               The as_static_pool object to be freed.
 *******************************************************************************************************
 */
extern void
aerospike_helper_free_static_pool(as_static_pool *static_pool)
{
	uint32_t iter = 0;

	/* clean up the as_* objects that were initialised */
	for (iter = 0; iter < static_pool->current_str_id; iter++) {
		as_string_destroy(&static_pool->string_pool[iter]);
	}

	for (iter = 0; iter < static_pool->current_int_id; iter++) {
		as_integer_destroy(&static_pool->integer_pool[iter]);
	}

	for (iter = 0; iter < static_pool->current_bytes_id; iter++) {
		as_bytes_destroy(&static_pool->bytes_pool[iter]);
	}

	for (iter = 0; iter < static_pool->current_list_id; iter++) {
		as_arraylist_destroy(&static_pool->alloc_list[iter]);
	}

	for (iter = 0; iter < static_pool->current_map_id; iter++) {
		as_hashmap_destroy(&static_pool->alloc_map[iter]);
	}
}

/*
 *******************************************************************************************************
 * Callback for as_scan_foreach and as_query_foreach functions.
 * It processes the as_val and translates it into an equivalent zval array.
 * It then calls the user registered callback passing the zval array as an
 * argument.
 *
 * @param p_val             The current as_val to be passed on to the user
 *                          callback as an argument.
 * @param udata             The userland_callback instance filled with fci and
 *                          fcc.
 * @return true if callback is successful; else false.
 *******************************************************************************************************
 */

extern bool
aerospike_helper_record_stream_callback(const as_val* p_val, void* udata)
{
	as_status               status = AEROSPIKE_OK;
	as_error                error;
#if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
	zval                    *record_p = NULL;
	zval                    *retval = NULL;
	zval                    **args[1];
	zval                    *outer_container_p = NULL;
#else
	zval                    record_p;
	zval                    retval;
	zval                    args[1];
	zval                    outer_container_p;
#endif
	bool                    do_continue = true;
	foreach_callback_udata  foreach_record_callback_udata;
	userland_callback       *user_func_p = (userland_callback *) udata;

	TSRMLS_FETCH_FROM_CTX(user_func_p->ts);

	if (!p_val) {
		DEBUG_PHP_EXT_INFO("callback is null; stream complete.");
		return true;
	}
	as_record* current_as_rec = as_record_fromval(p_val);
	if (!current_as_rec) {
		DEBUG_PHP_EXT_WARNING("stream returned a non-as_record object to the callback.");
		return true;
	}

	pthread_rwlock_wrlock(&AEROSPIKE_G(query_cb_mutex));
#if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
	MAKE_STD_ZVAL(record_p);
	array_init(record_p);
	foreach_record_callback_udata.udata_p = record_p;
#else
	array_init(&record_p);
	foreach_record_callback_udata.udata_p = &record_p;
#endif

	foreach_record_callback_udata.error_p = &error;
	if (!as_record_foreach(current_as_rec, (as_rec_foreach_callback) AS_DEFAULT_GET,
				&foreach_record_callback_udata)) {
		DEBUG_PHP_EXT_WARNING("stream callback failed to transform the as_record to an array zval.");
		zval_ptr_dtor(&record_p);
		pthread_rwlock_unlock(&AEROSPIKE_G(query_cb_mutex));
		return true;
	}

#if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
	MAKE_STD_ZVAL(outer_container_p);
	array_init(outer_container_p);
#else
	array_init(&outer_container_p);
#endif

#if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
	if (AEROSPIKE_OK != (status = aerospike_get_key_meta_bins_of_record(NULL, current_as_rec,
					&(current_as_rec->key), outer_container_p, NULL, false TSRMLS_CC)))
#else
	if (AEROSPIKE_OK != (status = aerospike_get_key_meta_bins_of_record(NULL, current_as_rec,
					&(current_as_rec->key), &outer_container_p, NULL, false TSRMLS_CC)))
#endif
		{
			DEBUG_PHP_EXT_DEBUG("Unable to get a record and metadata");
			zval_ptr_dtor(&record_p);
			zval_ptr_dtor(&outer_container_p);
			pthread_rwlock_unlock(&AEROSPIKE_G(query_cb_mutex));
			return true;
		}

#if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
	if (0 != add_assoc_zval(outer_container_p, PHP_AS_RECORD_DEFINE_FOR_BINS, record_p))
#else
		if (0 != add_assoc_zval(&outer_container_p, PHP_AS_RECORD_DEFINE_FOR_BINS, &record_p))
#endif
		{
			DEBUG_PHP_EXT_DEBUG("Unable to get a record");
			zval_ptr_dtor(&record_p);
			zval_ptr_dtor(&outer_container_p);
			pthread_rwlock_unlock(&AEROSPIKE_G(query_cb_mutex));
			return true;
		}

	/*
	 * Call the userland function with the array representing the record.
	 */

	user_func_p->fci.param_count = 1;
	user_func_p->fci.params = args;
#if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
	args[0] = &outer_container_p;
	user_func_p->fci.retval_ptr_ptr = &retval;
#else
	args[0] = outer_container_p;
	user_func_p->fci.retval = &retval;
#endif

	if (zend_call_function(&user_func_p->fci, &user_func_p->fcc TSRMLS_CC) == FAILURE) {
		DEBUG_PHP_EXT_WARNING("stream callback could not invoke the userland function.");
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "stream callback could not invoke userland function.");
		zval_ptr_dtor(&outer_container_p);
		pthread_rwlock_unlock(&AEROSPIKE_G(query_cb_mutex));
		return true;
	}

	zval_ptr_dtor(&outer_container_p);

#if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 70000)
	if (retval) {
		if ((Z_TYPE_P(retval) == IS_BOOL) && !Z_BVAL_P(retval)) {
			do_continue = false;
		} else {
			do_continue = true;
		}
		zval_ptr_dtor(&retval);
	}
#else
	if (Z_TYPE_P(&retval) == IS_FALSE) {
		do_continue = false;
	} else if (Z_TYPE_P(&retval) == IS_TRUE) {
		do_continue = true;
	}
	zval_ptr_dtor(&retval);
#endif

	pthread_rwlock_unlock(&AEROSPIKE_G(query_cb_mutex));
	return do_continue;
}

/*
 *******************************************************************************************************
 * Callback for as_query_foreach function in case of Aerospike::aggregate().
 * It processes the as_val and translates it into an equivalent zval.
 * It then populates the return zval with the same.
 *
 * @param val_p             The current as_val to be passed on to the user
 *                          callback as an argument.
 * @param udata_p           The zval return value to be filled with the result
 *                          of aggregation.
 * @return true if callback is successful; else false.
 *******************************************************************************************************
 */
extern bool
aerospike_helper_aggregate_callback(const as_val* val_p, void* udata_p)
{
	TSRMLS_FETCH();
	if (!val_p) {
		DEBUG_PHP_EXT_INFO("callback is null; stream complete.");
		return true;
	}

	AS_AGGREGATE_GET(((foreach_callback_udata*)udata_p)->obj, NULL, val_p, (foreach_callback_udata *) udata_p);
exit:
	return true;
}

extern void
aerospike_helper_check_and_configure_shm(as_config *config_p TSRMLS_DC) {
	if (SHM_USE_PHP_INI) {
		config_p->use_shm = true;
		config_p->shm_max_nodes = (uint32_t) SHM_MAX_NODES_PHP_INI;
		config_p->shm_max_namespaces = (uint32_t) SHM_MAX_NAMESPACES_PHP_INI;
		config_p->shm_takeover_threshold_sec = (uint32_t) SHM_TAKEOVER_THRESHOLD_SEC_PHP_INI;
		config_p->shm_key = (uint32_t) SHM_KEY_PHP_INI;
	} else {
		config_p->use_shm = false;
	}
}

/*
 *******************************************************************************************************
 * Function called from Aerospike::close().
 * It decrements ref_as_p which indicates the no. of references for internal C
 * SDK aerospike object being held by the various PHP userland Aerospike
 * objects.
 * It DOES NOT actually close the connection to server or free as_ref_p as other
 * PHP userland aerospike objects may re-use it in future.
 *
 * @param as_obj_p          The Aerospike_object upon which close() is invoked
 *                          currently.
 * @param error_p           The C SDK's as_error object to be populated by this
 *                          method in case of any errors if encountered.
 *
 * @return AEROSPIKE::OK if success. Otherwise AEROSPIKE_x.
 *******************************************************************************************************
 */
extern as_status
aerospike_helper_close_php_connection(Aerospike_object *as_obj_p,
		as_error *error_p TSRMLS_DC)
{
	as_error_init(error_p);
	DEBUG_PHP_EXT_DEBUG("In aerospike_helper_close_php_connection");
	if (as_obj_p->as_ref_p) {
		if (as_obj_p->as_ref_p->ref_as_p >= 1) {
			as_obj_p->as_ref_p->ref_as_p--;
		} else if (as_obj_p->as_ref_p->ref_as_p <= 0) {
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
					"Connection already closed!");
			DEBUG_PHP_EXT_ERROR("Connection already closed!");
		}
		DEBUG_PHP_EXT_INFO("Connection successfully closed!");
	} else {
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
				"Connection already closed and destroyed!");
		DEBUG_PHP_EXT_ERROR("Connection already closed and destroyed!");
	}
	return error_p->code;
}

/*
 *******************************************************************************************************
 * Function that trims leading and trailing white spaces in a given string.
 *
 * @param str               The input string to be trimmed.
 * @param len               The length of input string.
 *
 * @return len of trimmed str string on success. Otherwise 1.
 *******************************************************************************************************
 */
static int
trim_white_space(const char *str, size_t len)
{
	if (len == 0) {
		return 0;
	}

	const char *end;
	int out_size;

	/* Trim leading space */
	while(isspace(*str)) {
		str++;
	}

	if(*str == 0) {
		/* All spaces? */
		return 1;
	}

	/* Trim trailing space */
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)) {
		end--;
	}
	end++;

	/* Set output size to minimum of trimmed string length and buffer size minus 1 */
	out_size = (end - str) < len-1 ? (end - str) : len-1;

	/* Copy trimmed string and add null terminator */

	return out_size;
}

/*
 *******************************************************************************************************
 * Function to parse the session save path.
 * It sets the ns and set in aerospike_session by allocating memory to it.
 * It also sets the host within as_config.
 *
 * @param save_path         The session save path to be parsed.
 * @param session_p         The aerospike_session object whose ns_p and set_p
 *                          are to be set using the save_path.
 * @param config_p          The as_config whose hosts are to be set using the
 *                          save_path.
 * @param error_p           The C SDK's as_error object to be populated by this
 *                          method in case of any errors if encountered.
 *
 * @return AEROSPIKE::OK if success. Otherwise AEROSPIKE_x.
 *******************************************************************************************************
 */
static as_status
parse_save_path(char *save_path, aerospike_session *session_p,
		as_config *config_p, as_error *error_p TSRMLS_DC)
{
	char        *tok = NULL;
	char        *saved = NULL;
	char        port[INET_PORT];
	int16_t     iter_host = 0;
	char        *copy = NULL;

	copy = (char *) emalloc(strlen(save_path) + 1);
	strncpy(copy, save_path, strlen(save_path) + 1);

	tok = strtok_r(copy, SAVE_PATH_DELIMITER, &saved);

	if (tok == NULL) {
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
				"Could not read SAVE_PATH settings");
		DEBUG_PHP_EXT_DEBUG("Could not read SAVE_PATH settings");
		goto exit;
	}

	strncpy(session_p->ns_p, tok, strlen(tok));
	session_p->ns_p[strlen(tok)] = '\0';

	tok = strtok_r(NULL, SAVE_PATH_DELIMITER, &saved);
	if (tok == NULL) {
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
				"Could not read SAVE_PATH settings");
		DEBUG_PHP_EXT_DEBUG("Could not read SAVE_PATH settings");
		goto exit;
	}

	strncpy(session_p->set_p, tok, strlen(tok));
	session_p->set_p[strlen(tok)] = '\0';

	while (tok != NULL) {
		tok = strtok_r(NULL, IP_PORT_DELIMITER, &saved);
		if (tok == NULL) {
			if (iter_host == 0) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
						"Could not read SAVE_PATH settings");
				DEBUG_PHP_EXT_DEBUG("Could not read SAVE_PATH settings");
				goto exit;
			} else {
				break;
			}
		}
		trim_white_space(tok, strlen(tok) + 1);
		char *addr = (char *) emalloc(strlen(tok) + 1);
		strncpy(addr, tok, strlen(tok) + 1);
		config_p->hosts[iter_host].addr = addr;

		tok = strtok_r(NULL, HOST_DELIMITER, &saved);
		if (tok == NULL) {
			if (iter_host == 0) {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
						"Could not read SAVE_PATH settings");
				DEBUG_PHP_EXT_DEBUG("Could not read SAVE_PATH settings");
				goto exit;
			} else {
				break;
			}
		}
		trim_white_space(tok, strlen(tok) + 1);
		config_p->hosts[iter_host].port = atoi(tok);
		config_p->hosts_size++;
		iter_host++;
	}

exit:
	if (copy) {
		efree(copy);
	}
	return error_p->code;
}

/*
 *******************************************************************************************************
 * Function to check save path and set config and session object.
 * It uses the save path passed to it. If NULL, checks for PHP INI entries.
 * It sets the ns and set in aerospike_session by allocating memory to it.
 * It also sets the host within as_config.
 *
 * @param config_p          The as_config whose hosts are to be set using the
 *                          save_path.
 * @param save_path         The session save path to be parsed.
 * @param session_p         The aerospike_session object whose ns_p and set_p
 *                          are to be set using the save_path.
 * @param error_p           The C SDK's as_error object to be populated by this
 *                          method in case of any errors if encountered.
 *
 * @return AEROSPIKE::OK if success. Otherwise AEROSPIKE_x.
 *******************************************************************************************************
 */
extern as_status
aerospike_helper_check_and_set_config_for_session(as_config *config_p,
		char *save_path, aerospike_session *session_p,
		as_error *error_p TSRMLS_DC)
{
	char        *ip = NULL;
	uint16_t    port = 0;

	as_error_init(error_p);

	as_config_init(config_p);
	strcpy(config_p->lua.system_path, LUA_SYSTEM_PATH_PHP_INI);
	strcpy(config_p->lua.user_path, LUA_USER_PATH_PHP_INI);

	char *save_handler = SAVE_HANDLER_PHP_INI;
	if (save_handler != NULL) {
		if (!strncmp(save_handler, AEROSPIKE_SESSION, AEROSPIKE_SESSION_LEN)) {
			if (!save_path) {
				save_path = SAVE_PATH_PHP_INI;
			}

			if (save_path) {
				if (AEROSPIKE_OK != parse_save_path(save_path, session_p,
							config_p, error_p TSRMLS_CC)) {
					goto exit;
				}
			} else {
				PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Could not read SAVE_PATH settings");
				DEBUG_PHP_EXT_ERROR("Could not read SAVE_PATH settings");
				goto exit;
			}
		}
	} else {
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Could not read SAVE_HANDLER settings");
		DEBUG_PHP_EXT_ERROR("Could not read SAVE_HANDLER settings");
		goto exit;
	}

exit:
	return error_p->code;
}
