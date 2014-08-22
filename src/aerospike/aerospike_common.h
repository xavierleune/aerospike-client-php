#ifndef __AEROSPIKE_COMMON_H__
#define __AEROSPIKE_COMMON_H__

#include "dbg.h"

#define PHP_AEROSPIKE_GET_OBJECT    (Aerospike_object *)(zend_object_store_get_object(getThis() TSRMLS_CC))

#define foreach_hashtable(ht, position, datavalue)               \
    for (zend_hash_internal_pointer_reset_ex(ht, &position);     \
         zend_hash_get_current_data_ex(ht,                       \
                (void **) &datavalue, &position) == SUCCESS;     \
         zend_hash_move_forward_ex(ht, &position))

typedef struct csdk_aerospike_obj {
    aerospike *as_p;
    int ref_as_p;
} aerospike_ref;

typedef struct Aerospike_object {
    zend_object std;
    bool is_persistent;
    aerospike_ref *as_ref_p;
    u_int16_t is_conn_16;
} Aerospike_object;

/*
 * Struct for user data to be passed to aerospike foreach callbacks.
 * (For example, to as_rec_foreach, as_list_foreach, as_map_foreach).
 * It contains the actual udata and as_error object.
 */
typedef struct foreach_callback_udata_t {
    zval        *udata_p;
    as_error    *error_p;
} foreach_callback_udata;

/* 
 * PHP Userland Logger callback
 */
extern zend_fcall_info       func_call_info;
extern zend_fcall_info_cache func_call_info_cache;
extern zval                  *func_callback_retval_p;
extern uint32_t              is_callback_registered;

/* 
 * PHP Userland Serializer callback
 */
extern zend_fcall_info       user_serializer_call_info;
extern zend_fcall_info_cache user_serializer_call_info_cache;
extern zval                  *user_serializer_callback_retval_p;
extern uint32_t              is_user_serializer_registered;

/* 
 * PHP Userland Deserializer callback
 */
extern zend_fcall_info       user_deserializer_call_info;
extern zend_fcall_info_cache user_deserializer_call_info_cache;
extern zval                  *user_deserializer_callback_retval_p;
extern uint32_t              is_user_deserializer_registered;

extern bool
aerospike_helper_log_callback(as_log_level level, const char * func, const char * file, uint32_t line, const char * fmt, ...);
extern int parseLogParameters(as_log *as_log_p);

/* need to re-direct the same to log function that we have written
 * if per Aerospike_obj has been decided then we have to pass the object
 * as well into the callback method
 */
extern as_log_level   php_log_level_set;
#define __DEBUG_PHP__
#ifdef __DEBUG_PHP__
#define DEBUG_PHP_EXT_COMPARE_LEVEL(log_level, php_log_level, args...)                                \
    if (!(AS_LOG_LEVEL_OFF == php_log_level_set))                                                     \
        if (php_log_level_set >= log_level) {                                                         \
            php_error_docref(NULL TSRMLS_CC, php_log_level, ##args);                                  \
            aerospike_helper_log_callback((log_level | 0x08), __func__, __FILE__, __LINE__, ##args);  \
        }

#define DEBUG_PHP_EXT_ERROR(args...)          DEBUG_PHP_EXT_COMPARE_LEVEL(AS_LOG_LEVEL_ERROR, E_ERROR, args)
#define DEBUG_PHP_EXT_WARNING(args...)        DEBUG_PHP_EXT_COMPARE_LEVEL(AS_LOG_LEVEL_WARN, E_WARNING, args)
#define DEBUG_PHP_EXT_DEBUG(args...)          DEBUG_PHP_EXT_COMPARE_LEVEL(AS_LOG_LEVEL_DEBUG, E_NOTICE, args)
#define DEBUG_PHP_EXT_INFO(args...)           DEBUG_PHP_EXT_COMPARE_LEVEL(AS_LOG_LEVEL_INFO, E_NOTICE, args)
#else
#define DEBUG_PHP_EXT_ERROR(args...)
#define DEBUG_PHP_EXT_WARNING(args...)
#define DEBUG_PHP_EXT_DEBUG(args...)
#define DEBUG_PHP_EXT_INFO(args...)
#endif

#define AEROSPIKE_CONN_STATE_TRUE   1
#define AEROSPIKE_CONN_STATE_FALSE  0

#define PHP_IS_NULL(type)        (IS_NULL == type)
#define PHP_IS_NOT_NULL(type)    (IS_NULL != type)
#define PHP_IS_ARRAY(type)       (IS_ARRAY == type)
#define PHP_IS_NOT_ARRAY(type)   (IS_ARRAY != type)
#define PHP_IS_STRING(type)      (IS_STRING == type)
#define PHP_IS_NOT_STRING(type)  (IS_STRING != type)
#define PHP_IS_LONG(type)        (IS_LONG == type)


#define PHP_TYPE_ISNULL(zend_val)        PHP_IS_NULL(Z_TYPE_P(zend_val))
#define PHP_TYPE_ISSTR(zend_val)         PHP_IS_STRING(Z_TYPE_P(zend_val))
#define PHP_TYPE_ISARR(zend_val)         PHP_IS_ARRAY(Z_TYPE_P(zend_val))
#define PHP_TYPE_ISNOTNULL(zend_val)     PHP_IS_NOT_NULL(Z_TYPE_P(zend_val))
#define PHP_TYPE_ISNOTSTR(zend_val)      PHP_IS_NOT_STRING(Z_TYPE_P(zend_val))
#define PHP_TYPE_ISNOTARR(zend_val)      PHP_IS_NOT_ARRAY(Z_TYPE_P(zend_val))

#define PHP_IS_CONN_NOT_ESTABLISHED(conn_state)   (conn_state == AEROSPIKE_CONN_STATE_FALSE)

#define DEFAULT_ERRORNO 0
#define DEFAULT_ERROR ""

#define PHP_EXT_SET_AS_ERR(as_err_obj_p, code, msg)                     as_error_setall(as_err_obj_p, code, msg, __func__, __FILE__, __LINE__)
#define PHP_EXT_SET_AS_ERR_IN_CLASS(aerospike_class_p, as_err_obj_p)    aerospike_helper_set_error(aerospike_class_p, getThis(), as_err_obj_p, false)
#define PHP_EXT_RESET_AS_ERR_IN_CLASS(aerospike_class_p)                aerospike_helper_set_error(aerospike_class_p, getThis(), NULL, true)

extern bool AS_DEFAULT_GET(const char *key, const as_val *value, void *array);

extern as_status
aerospike_transform_iterate_for_rec_key_params(HashTable* ht_p, 
                                               as_key* as_key_p, 
                                               int16_t* set_val_p);

extern as_status
aerospike_transform_check_and_set_config(HashTable* ht_p, 
                                         zval** retdata_pp,
                                         as_config* config_p);

extern as_status
aerospike_transform_key_data_put(aerospike* as_object_p,
                                 zval **record_pp, 
                                 as_key* as_key_p,
                                 as_error *error_p,
                                 zval* options_p);

extern as_status
aerospike_transform_get_record(aerospike* as_object_p,
                               as_key* get_rec_key_p,
                               zval* options_p,
                               as_error *error_p,
                               zval* get_record_p,
                               zval* bins_p);

extern as_status
aerospike_record_operations_exists(aerospike* as_object_p, 
                                   as_key* as_key_p, 
                                   as_error *error_p, 
                                   zval* metadata_p, 
                                   zval* options_p);
extern as_status
aerospike_record_operations_remove(aerospike* as_object_p, 
                                   as_key* as_key_p, 
                                   as_error *error_p, 
                                   zval* options_p);
extern as_status
aerospike_record_operations_ops(aerospike* as_object_p,
                                as_key* as_key_p,
                                zval* options_p,
                                as_error* error_p,
                                int8_t* bin_name_p,
                                int8_t* str,
                                u_int64_t offset,
                                u_int64_t initial_value,
                                u_int64_t time_to_live,
                                u_int64_t operation);
extern as_status
aerospike_record_operations_remove_bin(aerospike* as_object_p, 
                                       as_key* as_key_p, 
                                       zval* bin_name_p, 
                                       as_error* error_p, 
                                       zval* options_p);

extern as_status
aerospike_php_exists_metadata(aerospike*  as_object_p, 
                              zval* key_record_p, 
                              zval* metadata_p, 
                              zval* options_p, 
                              as_error *error_p);

extern void 
aerospike_helper_set_error(zend_class_entry *ce_p, 
                           zval *object_p, 
                           as_error *error_p, 
                           bool reset_flag TSRMLS_DC);

extern as_status
aerospike_helper_object_from_alias_hash(Aerospike_object* as_object_p,
                                        bool persist_flag,
                                        as_config* conf,
                                        HashTable persistent_list,
                                        int persist);
#endif
