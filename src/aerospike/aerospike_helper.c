#include "php.h"
#include "aerospike/as_log.h"
#include "aerospike/as_key.h"
#include "aerospike/as_config.h"
#include "aerospike/as_error.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike.h"

#include "aerospike_common.h"

zend_fcall_info       func_call_info;
zend_fcall_info_cache func_call_info_cache;
zval                  *func_callback_retval_p;
uint32_t              is_callback_registered;

#ifndef __AEROSPIKE_PHP_CLIENT_LOG_LEVEL__
as_log_level php_log_level_set = AS_LOG_LEVEL_OFF;
#else
as_log_level php_log_level_set = __AEROSPIKE_PHP_CLIENT_LOG_LEVEL__;
#endif

extern bool 
aerospike_helper_log_callback(as_log_level level, const char * func, const char * file, uint32_t line, const char * fmt, ...)
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

extern void 
aerospike_helper_set_error(zend_class_entry *ce_p, zval *object_p, as_error *error_p, bool reset_flag TSRMLS_DC)
{
    zval*    err_code_p = NULL;
    zval*    err_msg_p = NULL;

    MAKE_STD_ZVAL(err_code_p);
    MAKE_STD_ZVAL(err_msg_p);

    if (reset_flag) {
        ZVAL_STRINGL(err_msg_p, DEFAULT_ERROR, strlen(DEFAULT_ERROR), 1);
        ZVAL_LONG(err_code_p, DEFAULT_ERRORNO);
    } else {
        ZVAL_STRINGL(err_msg_p, error_p->message, strlen(error_p->message), 1);
        ZVAL_LONG(err_code_p, error_p->code);
    }

    zend_update_property(ce_p, object_p, "error", strlen("error"), err_msg_p TSRMLS_DC);
    zend_update_property(ce_p, object_p, "errorno", strlen("errorno"), err_code_p TSRMLS_DC);

    zval_ptr_dtor(&err_code_p);
    zval_ptr_dtor(&err_msg_p);
}

/* This macro is defined to register a new resource and to add hash to it 
 */
#define ZEND_HASH_CREATE_ALIAS_NEW(alias, alias_len, new_flag)                \
do {                                                                          \
    if (NULL != (as_object_p->as_ref_p = ecalloc(1,                           \
                    sizeof(aerospike_ref)))) {                                \
        as_object_p->as_ref_p->as_p = NULL;                                   \
        as_object_p->as_ref_p->ref_as_p = 0;                                  \
    }                                                                         \
    as_object_p->as_ref_p->as_p = aerospike_new(conf);                        \
    ZEND_REGISTER_RESOURCE(rsrc_result, as_object_p->as_ref_p->as_p,          \
            val_persist);                                                     \
    as_object_p->as_ref_p->ref_as_p = 1;                                      \
    new_le.ptr = as_object_p->as_ref_p;                                       \
    new_le.type = val_persist;                                                \
    if (new_flag) {                                                           \
        zend_hash_add(&EG(persistent_list), alias, alias_len,                 \
                (void *) &new_le, sizeof(list_entry), NULL);                  \
    } else {                                                                  \
        zend_hash_update(&EG(persistent_list), alias, alias_len,              \
                (void *) &new_le, sizeof(list_entry), (void **) &le);         \
        goto exit;                                                            \
    }                                                                         \
} while(0)


/* This macro is defined to match the config details with the stored object
 * details in the resource and if match use the existing one. 
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

extern as_status
aerospike_helper_object_from_alias_hash(Aerospike_object* as_object_p,
                                        int8_t * persistence_alias_p,
                                        int16_t persistence_alias_len,
                                        as_config* conf,
                                        HashTable persistent_list,
                                        int val_persist)
{
    list_entry *le, new_le;
    zval* rsrc_result = NULL;
    as_status status = AEROSPIKE_OK;
    int itr_user = 0, itr_stored = 0;
    aerospike_ref *tmp_ref = NULL;
    char* alias_null_p = "aerospike";

    if (!(as_object_p) && !(conf))
    {
        status = AEROSPIKE_ERR_PARAM;
        goto exit;
    }

    if (persistence_alias_len == 0) {
        /*TODO
         * Alias name is not given by User.
         * Most effective way is to use host as an alias
         * for empty alias case.
         * An algorithm needs to be written to iterate over
         * list of hosts and check if the one of them is already
         * hashed and can be reused.
         */
#ifdef __AEROSPIKE_ALIAS_COMP_IPADDR__
        for(itr_user=0; itr_user < conf->hosts_size; itr_user++ ) {
            if (zend_hash_find(&EG(persistent_list), conf->hosts[itr_user].addr,
                        strlen(conf->hosts[itr_user].addr), (void **) &le) == SUCCESS) {
                ZEND_CONFIG_MATCH_USER_STORED(conf->hosts[itr_user].addr, (strlen(conf->hosts[itr_user].addr)));
            }
        }
        ZEND_HASH_CREATE_ALIAS_NEW((conf->hosts[0].addr), (strlen(conf->hosts[0].addr)), 1);
        for(itr_user=1; itr_user < conf->hosts_size; itr_user++ ) {
            zend_hash_add(&EG(persistent_list),  conf->hosts[itr_user].addr,
                    strlen(conf->hosts[itr_user].addr), (void *) &new_le, sizeof(list_entry), NULL);
        }
#else
        /* Alias is not given by the User.
         * For such scenerio we will create a generic alias for the user and 
         * then use that alias for adding and fetching the value from the store
         */
        persistence_alias_p = (int8_t*) alias_null_p;
        persistence_alias_len = strlen(alias_null_p);
        if (zend_hash_find(&EG(persistent_list), persistence_alias_p,
                    persistence_alias_len, (void **) &le) == SUCCESS) {
            ZEND_CONFIG_MATCH_USER_STORED(persistence_alias_p, (persistence_alias_len));
        }
        goto create_new;


#endif
        goto exit;
    }

    /* if we are here then we are having alias passed in.
     * Lets check if the alias is already present in the storage and if yes then
     * check for the config details if match use the existing object else create
     * a new one 
     */
    if (zend_hash_find(&EG(persistent_list), persistence_alias_p,
                persistence_alias_len, (void **) &le) == SUCCESS) {
        ZEND_CONFIG_MATCH_USER_STORED(persistence_alias_p, (persistence_alias_len));
    }
    goto create_new;
use_existing:
    /* config details are matched, use the existing one obtained from the
     * storage
     */
    as_object_p->is_conn_16 = AEROSPIKE_CONN_STATE_TRUE;
    as_object_p->as_ref_p = tmp_ref;
    as_object_p->as_ref_p->ref_as_p++;
    goto exit;
create_new:
    /* Create a new object in the resorce.
     */
    ZEND_HASH_CREATE_ALIAS_NEW(persistence_alias_p, persistence_alias_len, 1);
exit:
    return (status);
}
