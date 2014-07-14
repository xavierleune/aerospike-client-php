#ifndef __AEROSPIKE_COMMON_H__
#define __AEROSPIKE_COMMON_H__

#define PHP_AEROSPIKE_GET_OBJECT    (Aerospike_object *)(zend_object_store_get_object(getThis() TSRMLS_CC))

#define foreach_hashtable(ht, position, datavalue)               \
    for (zend_hash_internal_pointer_reset_ex(ht, &position);     \
         zend_hash_get_current_data_ex(ht,                       \
                (void **) &datavalue, &position) == SUCCESS;     \
         zend_hash_move_forward_ex(ht, &position))

/* need to re-direct the same to log function that we have written
 * if per Aerospike_obj has been decided then we have to pass the object
 * as well into the callback method
 */
as_log_level   php_log_level_set = AS_LOG_LEVEL_OFF;
#ifdef __DEBUG_PHP__
#define DEBUG_PHP_EXT_COMPARE_LEVEL(log_level, var_args, ...)      \
    if (!(AS_LOG_LEVEL_OFF == php_log_level_set))                  \
        if (php_log_level_set >= log_level)                        \
            printf("hey i am in logs"); /*replace this with our log function*/
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

extern as_status
aerospike_transform_iterate_for_rec_key_params(HashTable* ht_p, as_key* as_key_p, int16_t* set_val_p);

extern as_status
aerospike_transform_iteratefor_name_port(HashTable* ht_p, as_config* as_config_p);

extern as_status
aerospike_transform_iteratefor_hostkey(HashTable* ht_p, zval** retdata_pp);

extern as_status
aerospike_transform_key_data_put(aerospike* as_object_p, HashTable* ht_p, as_key* as_key_p, as_error *error_p, zval* options_p);

#endif
