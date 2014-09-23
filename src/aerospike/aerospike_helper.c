#include "php.h"
#include "php_aerospike.h"
#include "aerospike/as_log.h"
#include "aerospike/as_key.h"
#include "aerospike/as_config.h"
#include "aerospike/as_error.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike.h"
#include "pthread.h"
#include "aerospike_common.h"

/*
 *******************************************************************************************************
 * PHP Userland logger callback.
 *******************************************************************************************************
 */
zend_fcall_info       func_call_info;
zend_fcall_info_cache func_call_info_cache;
zval                  *func_callback_retval_p;
uint32_t              is_callback_registered;
pthread_mutex_t      aerospike_mutex;
/*
 *******************************************************************************************************
 * aerospike-client-php global log level
 *******************************************************************************************************
 */
#ifndef __AEROSPIKE_PHP_CLIENT_LOG_LEVEL__
as_log_level php_log_level_set = AS_LOG_LEVEL_OFF;
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
        int16_t   iter = 0;
        zval**    params[4];
        zval*     z_func = NULL;
        zval*     z_file = NULL;
        zval*     z_line = NULL; 
        zval*     z_level = NULL;
        func_callback_retval_p = NULL;

        ALLOC_INIT_ZVAL(z_level);
        ZVAL_LONG(z_level, level);
        params[0] = &z_level;

        ALLOC_INIT_ZVAL(z_func);
        ZVAL_STRING(z_func, func, 1);
        params[1] = &z_func;

        ALLOC_INIT_ZVAL(z_file);
        ZVAL_STRING(z_file, file, 1);
        params[2] = &z_file;

        ALLOC_INIT_ZVAL(z_line);
        ZVAL_LONG(z_line, line);
        params[3] = &z_line;

        func_call_info.param_count = 4;
        func_call_info.params = params;
        func_call_info.retval_ptr_ptr = &func_callback_retval_p;
   
        if (zend_call_function(&func_call_info, &func_call_info_cache TSRMLS_CC) == SUCCESS && 
            func_call_info.retval_ptr_ptr && *func_call_info.retval_ptr_ptr) {
                //TODO: COPY_PZVAL_TO_ZVAL(*return_value, *func_call_info.retval_ptr_ptr);
        } else {
                // TODO: Handle failure in zend_call_function
        }

        for (iter = 0; iter < 4; iter++) {
            zval_ptr_dtor(params[iter]);
        }
    }

    return true;
}

/* 
 *******************************************************************************************************
 * Sets C client's logger callback.
 * 
 * @param as_log_p          The as_log to be set.
 * @return 1 if log set succeeds. Otherwise 0.
 *******************************************************************************************************
 */
extern int parseLogParameters(as_log* as_log_p)
{
    if (as_log_set_callback(as_log_p, &aerospike_helper_log_callback)) {
	is_callback_registered = 1;
        Z_ADDREF_P(func_call_info.function_name);
        return 1;
    } else {	
        return 0;;
    }	
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

    zval*    err_code_p = NULL;
    zval*    err_msg_p = NULL;
    aerospike_global_error error_t = AEROSPIKE_G(error_g);

    MAKE_STD_ZVAL(err_code_p);
    MAKE_STD_ZVAL(err_msg_p);

    if (error_t.reset) {
        ZVAL_STRINGL(err_msg_p, DEFAULT_ERROR, strlen(DEFAULT_ERROR), 1);
        ZVAL_LONG(err_code_p, DEFAULT_ERRORNO);
    } else {
        ZVAL_STRINGL(err_msg_p, error_t.error.message, strlen(error_t.error.message), 1);
        ZVAL_LONG(err_code_p, error_t.error.code);
    }

    zend_update_property(ce_p, object_p, "error", strlen("error"), err_msg_p TSRMLS_CC);
    zend_update_property(ce_p, object_p, "errorno", strlen("errorno"), err_code_p TSRMLS_CC);

    zval_ptr_dtor(&err_code_p);
    zval_ptr_dtor(&err_msg_p);
}

/* 
 *******************************************************************************************************
 * This macro is defined to create a new C client's aerospike object.
 *******************************************************************************************************
 */
#define ZEND_CREATE_AEROSPIKE_REFERENCE_OBJECT()                              \
do {                                                                          \
    if (NULL != (as_object_p->as_ref_p = ecalloc(1,                           \
                    sizeof(aerospike_ref)))) {                                \
        as_object_p->as_ref_p->as_p = NULL;                                   \
        as_object_p->as_ref_p->ref_as_p = 0;                                  \
    }                                                                         \
    as_object_p->as_ref_p->as_p = aerospike_new(conf);                        \
    as_object_p->as_ref_p->ref_as_p = 1;                                      \
} while(0)

/* 
 *******************************************************************************************************
 * This macro is defined to register a new resource and to add hash to it.
 *******************************************************************************************************
 */
#define ZEND_HASH_CREATE_ALIAS_NEW(alias, alias_len, new_flag)                 \
do {                                                                           \
    ZEND_CREATE_AEROSPIKE_REFERENCE_OBJECT();                                  \
    ZEND_REGISTER_RESOURCE(rsrc_result, as_object_p->as_ref_p->as_p,           \
            val_persist);                                                      \
    new_le.ptr = as_object_p->as_ref_p;                                        \
    new_le.type = val_persist;                                                 \
    if (new_flag) {                                                            \
        pthread_mutex_lock(&aerospike_mutex);                                  \
        DEBUG_PHP_EXT_DEBUG("Before:(%.4x) Elements in list are %d",           \
                persistent_list, persistent_list->nNumOfElements);             \
        zend_hash_add(persistent_list, alias, alias_len,                       \
                (void *) &new_le, sizeof(zend_rsrc_list_entry), NULL);         \
        DEBUG_PHP_EXT_DEBUG("After:(%.4x) Elements in list are %d",            \
                persistent_list, persistent_list->nNumOfElements);             \
        pthread_mutex_unlock(&aerospike_mutex);                                \
        goto exit;                                                             \
    } else {                                                                   \
        pthread_mutex_lock(&aerospike_mutex);                                  \
        zend_hash_update(persistent_list,                                      \
                alias, alias_len, (void *) &new_le,                            \
                sizeof(zend_rsrc_list_entry), (void **) &le);                  \
        pthread_mutex_unlock(&aerospike_mutex);                                \
        goto exit;                                                             \
    }                                                                          \
} while(0)


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
 *
 * @return AEROSPIKE::OK if success. Otherwise AEROSPIKE_x.
 *******************************************************************************************************
 */
extern as_status
aerospike_helper_object_from_alias_hash(Aerospike_object* as_object_p,
                                        bool persist_flag,
                                        as_config* conf,
                                        HashTable *persistent_list,
                                        int val_persist TSRMLS_DC)
{
    zend_rsrc_list_entry *le, new_le;
    zval* rsrc_result = NULL;
    as_status status = AEROSPIKE_OK;
    int itr_user = 0, itr_stored = 0;
    aerospike_ref *tmp_ref = NULL;
    char *alias_to_search = NULL;
    char *alias_to_hash = NULL;
    char port[MAX_PORT_SIZE];

    if (!(as_object_p) && !(conf))
    {
        status = AEROSPIKE_ERR_PARAM;
        goto exit;
    }

    if(persist_flag == false) {
        ZEND_CREATE_AEROSPIKE_REFERENCE_OBJECT();
        goto exit;
    }

    /*
     * Iterate over list of hosts and check if the one of them is already
     * hashed and can be reused.
     */

    alias_to_search = (char*) emalloc(strlen(conf->hosts[0].addr) + MAX_PORT_SIZE + 1);
    sprintf(port, "%d", conf->hosts[0].port);
    strcpy(alias_to_search, conf->hosts[0].addr);
    strcat(alias_to_search, ":");
    strcat(alias_to_search, port);
    for(itr_user=0; itr_user < conf->hosts_size; itr_user++) {
        pthread_mutex_lock(&aerospike_mutex);
        if (zend_hash_find(persistent_list, alias_to_search,
                strlen(alias_to_search), (void **) &le) == SUCCESS) {
            if (alias_to_search) {
                efree(alias_to_search);
                alias_to_search = NULL;
            }
            pthread_mutex_unlock(&aerospike_mutex);
            tmp_ref = le->ptr;
            goto use_existing;
        }
        pthread_mutex_unlock(&aerospike_mutex);
    }
   // pthread_mutex_unlock(&aerospike_mutex);

    ZEND_HASH_CREATE_ALIAS_NEW(alias_to_search, strlen(alias_to_search), 1);
    for(itr_user=1; itr_user < conf->hosts_size; itr_user++ ) {
        alias_to_hash = (char*) emalloc(strlen(conf->hosts[0].addr) + MAX_PORT_SIZE + 1);
        sprintf(port, "%d", conf->hosts[itr_user].port);
        strcpy(alias_to_hash, conf->hosts[itr_user].addr);
        strcat(alias_to_hash, ":");
        strcat(alias_to_hash, port);
        pthread_mutex_lock(&aerospike_mutex);
        zend_hash_add(persistent_list, alias_to_hash,
                strlen(alias_to_hash), (void *) &new_le, sizeof(zend_rsrc_list_entry), NULL);
        pthread_mutex_unlock(&aerospike_mutex);
        efree(alias_to_hash);
        alias_to_hash = NULL;
   }

use_existing:
    /* config details are matched, use the existing one obtained from the
     * storage
     */
    as_object_p->is_conn_16 = AEROSPIKE_CONN_STATE_TRUE;
    as_object_p->as_ref_p = tmp_ref;
    as_object_p->as_ref_p->ref_as_p++;
    DEBUG_PHP_EXT_DEBUG("\nCount is: %d",as_object_p->as_ref_p->ref_as_p);
    goto exit;
exit:
    if (alias_to_search) {
        efree(alias_to_search);
        alias_to_search = NULL;
    }
    return (status);
}

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

