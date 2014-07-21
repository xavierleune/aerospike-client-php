#ifndef __AEROSPIKE_COMMON_H__
#define __AEROSPIKE_COMMON_H__

#include "dbg.h"

#define PHP_AEROSPIKE_GET_OBJECT    (Aerospike_object *)(zend_object_store_get_object(getThis() TSRMLS_CC))

#define foreach_hashtable(ht, position, datavalue)               \
    for (zend_hash_internal_pointer_reset_ex(ht, &position);     \
         zend_hash_get_current_data_ex(ht,                       \
                (void **) &datavalue, &position) == SUCCESS;     \
         zend_hash_move_forward_ex(ht, &position))

extern zend_fcall_info       func_call_info;
extern zend_fcall_info_cache func_call_info_cache;
extern zval                  *func_callback_retval_p;
extern uint32_t              is_callback_registered;

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
#define DEBUG_PHP_EXT_COMPARE_LEVEL(log_level, var_args, ...)      \
    if (!(AS_LOG_LEVEL_OFF == php_log_level_set))                  \
        if (php_log_level_set >= log_level)                        \
            aerospike_helper_log_callback((log_level | 0x08), __func__, __FILE__, __LINE__, var_args); /*replace this with our log function*/
#define DEBUG_PHP_EXT_ERROR(var_args, ...)          DEBUG_PHP_EXT_COMPARE_LEVEL(AS_LOG_LEVEL_ERROR, var_args, ...)
#define DEBUG_PHP_EXT_WARNING(var_args, ...)        DEBUG_PHP_EXT_COMPARE_LEVEL(AS_LOG_LEVEL_WARN, var_args, ...)
#define DEBUG_PHP_EXT_DEBUG(var_args, ...)          DEBUG_PHP_EXT_COMPARE_LEVEL(AS_LOG_LEVEL_DEBUG, var_args, ...)
#define DEBUG_PHP_EXT_INFO(var_args, ...)           DEBUG_PHP_EXT_COMPARE_LEVEL(AS_LOG_LEVEL_INFO, var_args, ...)
#else
#define DEBUG_PHP_EXT_ERROR(var_args, ...)
#define DEBUG_PHP_EXT_WARNING(var_args, ...)
#define DEBUG_PHP_EXT_DEBUG(var_args, ...)
#define DEBUG_PHP_EXT_INFO(var_args, ...)
#endif

#define AEROSPIKE_CONN_STATE_TRUE   1
#define AEROSPIKE_CONN_STATE_FALSE  0

#define PHP_TYPE_ISNULL(zend_val)     (IS_NULL == Z_TYPE_P(zend_val))
#define PHP_TYPE_ISARR(zend_val)      (IS_ARRAY == Z_TYPE_P(zend_val))
#define PHP_TYPE_ISNOTARR(zend_val)   !PHP_TYPE_ISARR(zend_val)

extern as_status
aerospike_transform_iterate_for_rec_key_params(HashTable* ht_p, as_key* as_key_p, int16_t* set_val_p);

extern as_status
aerospike_transform_iteratefor_name_port(HashTable* ht_p, as_config* as_config_p);

extern as_status
aerospike_transform_iteratefor_hostkey(HashTable* ht_p, zval** retdata_pp);

extern as_status
aerospike_transform_key_data_put(aerospike* as_object_p, zval **record_pp, as_key* as_key_p, as_error *error_p, zval* options_p);

as_status
aerospike_transform_get_record(aerospike* as_object_p,
                               as_key* get_rec_key_p,
                               zval* options_p,
                               as_error *error_p,
                               zval* get_record_p,
                               zval* bins_p);

extern as_status
aerospike_record_operations_exist(aerospike* as_object_p, as_key* as_key_p, as_error *error_p);
extern as_status
aerospike_record_operations_delete(aerospike* as_object_p, as_key* as_key_p, as_error *error_p);
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
#endif
