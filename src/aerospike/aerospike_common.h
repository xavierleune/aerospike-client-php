#ifndef __AEROSPIKE_COMMON_H__
#define __AEROSPIKE_COMMON_H__
#include "aerospike/as_arraylist.h"
#include "aerospike/as_hashmap.h"
#include "aerospike/as_key.h"
#include "aerospike/as_node.h"

/*
 *******************************************************************************************************
 * MACRO TO RETRIEVE THE Aerospike_object FROM THE ZEND PERSISTENT STORE FOR THE
 * CURRENT OBJECT UPON WHICH THE API IS INVOKED.
 *******************************************************************************************************
 */
#define PHP_AEROSPIKE_GET_OBJECT    (Aerospike_object *)(zend_object_store_get_object(getThis() TSRMLS_CC))

/* 
 *******************************************************************************************************
 * MACRO TO ITERATE OVER A HASHTABLE.
 *
 * @param ht            Hashtable pointer.
 * @param position      HashPosition.
 * @param datavalue     zval ** which shall be populated with the current data.
 *******************************************************************************************************
 */
#define foreach_hashtable(ht, position, datavalue)               \
    for (zend_hash_internal_pointer_reset_ex(ht, &position);     \
         zend_hash_get_current_data_ex(ht,                       \
                (void **) &datavalue, &position) == SUCCESS;     \
         zend_hash_move_forward_ex(ht, &position))

/* 
 *******************************************************************************************************
 * MACROS FOR MAX STORE SIZE.
 *******************************************************************************************************
 */
#define AS_MAX_STORE_SIZE 1024
#define AS_MAX_LIST_SIZE AS_MAX_STORE_SIZE
#define AS_MAX_MAP_SIZE AS_MAX_STORE_SIZE

/* 
 *******************************************************************************************************
 * MACROS FOR UDF KEYS AND FILE READING BUFFER SIZE.
 *******************************************************************************************************
 */
#define UDF_MODULE_NAME "name"
#define UDF_MODULE_TYPE "type"
#define LUA_FILE_BUFFER_FRAME 512

/* 
 *******************************************************************************************************
 * MACRO TO RETRIEVE THE PHP INI ENTRIES FOR LUA SYSTEM AND USER PATHS IF
 * SPECIFIED, ELSE RETURN DEFAULTS.
 *******************************************************************************************************
 */
#define LUA_SYSTEM_PATH_PHP_INI INI_STR("aerospike.udf.lua_system_path") ? INI_STR("aerospike.udf.lua_system_path") : ""
#define LUA_USER_PATH_PHP_INI INI_STR("aerospike.udf.lua_user_path") ? INI_STR("aerospike.udf.lua_user_path") : ""

/* 
 *******************************************************************************************************
 * MACROS FOR PREDICATE ARRAY KEYS.
 *******************************************************************************************************
 */
#define BIN "bin"
#define OP "op"
#define VAL "val"

/*
 *******************************************************************************************************
 * EXPECTED KEYS IN INPUT FROM PHP USERLAND.
 *******************************************************************************************************
 */
#define PHP_AS_KEY_DEFINE_FOR_HOSTS                   "hosts"
#define PHP_AS_KEY_DEFINE_FOR_HOSTS_LEN               5
#define PHP_AS_KEY_DEFINE_FOR_USER                    "user"
#define PHP_AS_KEY_DEFINE_FOR_USER_LEN                4
#define PHP_AS_KEY_DEFINE_FOR_PASSWORD                "pass"
#define PHP_AS_KEY_DEFINE_FOR_PASSWORD_LEN            4
#define PHP_AS_KEY_DEFINE_FOR_ADDR                    "addr"
#define PHP_AS_KEY_DEFINE_FOR_ADDR_LEN                4
#define PHP_AS_KEY_DEFINE_FOR_PORT                    "port"
#define PHP_AS_KEY_DEFINE_FOR_PORT_LEN                4
#define PHP_AS_KEY_DEFINE_FOR_NS                      "ns"
#define PHP_AS_KEY_DEFINE_FOR_NS_LEN                  2
#define PHP_AS_KEY_DEFINE_FOR_SET                     "set"
#define PHP_AS_KEY_DEFINE_FOR_SET_LEN                 3
#define PHP_AS_KEY_DEFINE_FOR_KEY                     "key"
#define PHP_AS_KEY_DEFINE_FOR_KEY_LEN                 3
#define PHP_AS_KEY_DEFINE_FOR_DIGEST                  "digest"
#define PHP_AS_KEY_DEFINE_FOR_DIGEST_LEN              6
#define PHP_AS_RECORD_DEFINE_FOR_TTL                  "ttl"
#define PHP_AS_RECORD_DEFINE_FOR_TTL_LEN              3
#define PHP_AS_RECORD_DEFINE_FOR_GENERATION           "generation"
#define PHP_AS_RECORD_DEFINE_FOR_GENERATION_LEN       10
#define PHP_AS_RECORD_DEFINE_FOR_METADATA             "metadata"
#define PHP_AS_RECORD_DEFINE_FOR_METADATA_LEN         8
#define PHP_AS_RECORD_DEFINE_FOR_BINS                 "bins"
#define PHP_AS_RECORD_DEFINE_FOR_BINS_LEN             4

#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46
#define INET_PORT 5
#define IP_PORT_SEPARATOR_LEN 1
#define IP_PORT_MAX_LEN INET6_ADDRSTRLEN + INET_PORT + IP_PORT_SEPARATOR_LEN
/*
 *******************************************************************************************************
 * Static pool maintained to avoid runtime mallocs.
 * It comprises of following pools:
 * 1. Pool for Arraylist
 * 2. Pool for Hashmap
 * 3. Pool for Strings
 * 4. Pool for Integers
 * 5. Pool for Bytes
 *******************************************************************************************************
 */
typedef struct list_map_static_pool {
    u_int32_t        current_list_id;
    as_arraylist     alloc_list[AS_MAX_LIST_SIZE];
    u_int32_t        current_map_id;
    as_hashmap       alloc_map[AS_MAX_MAP_SIZE];
    as_string        string_pool[AS_MAX_STORE_SIZE];
    u_int32_t        current_str_id;
    as_integer       integer_pool[AS_MAX_STORE_SIZE];
    u_int32_t        current_int_id;
    as_bytes         bytes_pool[AS_MAX_STORE_SIZE];
    u_int32_t        current_bytes_id;
} as_static_pool;

/* 
 *******************************************************************************************************
 * Structure containing C client's aerospike object and its reference counter.
 *******************************************************************************************************
 */
typedef struct csdk_aerospike_obj {
    aerospike *as_p;
    int ref_as_p;
} aerospike_ref;

/* 
 *******************************************************************************************************
 * Structure to map the zend Aerospike object with the C client's aerospike object ref structure.
 *******************************************************************************************************
 */
typedef struct Aerospike_object {
    zend_object std;
    bool is_persistent;
    aerospike_ref *as_ref_p;
    u_int16_t is_conn_16;
#ifdef ZTS
    void ***ts;
#endif
} Aerospike_object;

/*
 *******************************************************************************************************
 * Struct for user data to be passed to aerospike foreach callbacks.
 * (For example, to as_rec_foreach, as_list_foreach, as_map_foreach).
 * It contains the actual udata and as_error object.
 *******************************************************************************************************
 */
typedef struct foreach_callback_udata_t {
    zval        *udata_p;
    as_error    *error_p;
    Aerospike_object *obj;
} foreach_callback_udata;

/*
 *******************************************************************************************************
 * Struct for user data to be passed to aerospike foreach callbacks.
 * (For example, to as_rec_foreach, as_list_foreach, as_map_foreach).
 * It contains the actual udata and as_error object.
 *******************************************************************************************************
 */
typedef struct foreach_callback_info_udata_t {
    zval        *udata_p;
    HashTable   *host_lookup_p;
} foreach_callback_info_udata;

/*
 *******************************************************************************************************
 * PHP Userland Logger callback
 *******************************************************************************************************
 */
extern zend_fcall_info       func_call_info;
extern zend_fcall_info_cache func_call_info_cache;
extern zval                  *func_callback_retval_p;
extern uint32_t              is_callback_registered;

/* 
 *******************************************************************************************************
 * PHP Userland Serializer callback
 *******************************************************************************************************
 */
extern zend_fcall_info       user_serializer_call_info;
extern zend_fcall_info_cache user_serializer_call_info_cache;
extern zval                  *user_serializer_callback_retval_p;
extern uint32_t              is_user_serializer_registered;

/* 
 *******************************************************************************************************
 * PHP Userland Deserializer callback
 *******************************************************************************************************
 */
extern zend_fcall_info       user_deserializer_call_info;
extern zend_fcall_info_cache user_deserializer_call_info_cache;
extern zval                  *user_deserializer_callback_retval_p;
extern uint32_t              is_user_deserializer_registered;
/*
 ****************************************************************************
 * A wrapper for the two structs zend_fcall_info and zend_fcall_info_cache
 * that allows for userland function callbacks from within a C-callback
 * context, by having both passed within this struct as a void *udata.
 ****************************************************************************
 */
typedef struct _userland_callback {
    zend_fcall_info *fci_p;
    zend_fcall_info_cache *fcc_p;
    Aerospike_object *obj;
} userland_callback;

/*
 *******************************************************************************************************
 * Decision Structure for as_config/zval to be populated by
 * aerospike_transform_check_and_set_config method.
 *******************************************************************************************************
 */
enum config_transform_result_type {
    TRANSFORM_INTO_AS_CONFIG    = 0,
    TRANSFORM_INTO_ZVAL         = 1
};

/*
 *******************************************************************************************************
 * Union for transform result iterator.
 * Holds either as_config or a zval.
 *******************************************************************************************************
 */
typedef union _transform_result {
    as_config* as_config_p;
    HashTable* host_lookup_p;
} transform_result_t;

/*
 *******************************************************************************************************
 * Structure for PHP config array to transformed value and its type.
 * Possible transformed values are as_config or zval.
 *******************************************************************************************************
 */
typedef struct _transform_zval_config_into {
    enum config_transform_result_type       transform_result_type;
    transform_result_t                      transform_result;
} transform_zval_config_into;

extern bool
aerospike_helper_log_callback(as_log_level level, const char * func TSRMLS_DC, const char * file, uint32_t line, const char * fmt, ...);
extern int parseLogParameters(as_log *as_log_p);
extern bool
aerospike_helper_record_stream_callback(const as_val* p_val, void* udata);
extern bool
aerospike_helper_aggregate_callback(const as_val* val_p, void* udata_p);
extern bool
aerospike_info_callback(const as_error* err, const as_node* node, char* request,
        char* response, void* udata);

/*
 * Need to re-direct the same to log function that we have written
 * if per Aerospike_obj has been decided then we have to pass the object
 * as well into the callback method
 *
 */
extern as_log_level   php_log_level_set;
/* 
 *******************************************************************************************************
 * MACRO TO COMPARE LOG LEVEL.
 *
 * @param log_level          The as_loglevel to be compared with.
 * @param php_log_level      The PHP client's log_level set.
 * @param args               The args to be passed to the logger callback.
 *                           (Typically the log message string)
 *******************************************************************************************************
 */
#define __DEBUG_PHP__
#ifdef __DEBUG_PHP__
#define DEBUG_PHP_EXT_COMPARE_LEVEL(log_level, php_log_level, args...)                                \
do {                                                                                                  \
    if (!(AS_LOG_LEVEL_OFF == php_log_level_set))                                                     \
        if (php_log_level_set >= log_level) {                                                         \
            php_error_docref(NULL TSRMLS_CC, php_log_level, args);                                    \
            aerospike_helper_log_callback((log_level | 0x08), __func__ TSRMLS_CC,                     \
                    __FILE__, __LINE__, ##args);                                                      \
        }                                                                                             \
} while(0)

/*
 *******************************************************************************************************
 * MACROS TO SET LOGGING MESSAGES FOR THE VARIOUS LOG LEVELS.
 *
 * @param args               The args to be passed to the logger callback.
 *                           (Typically the log message string)
 *******************************************************************************************************
 */
#define DEBUG_PHP_EXT_ERROR(args...)          DEBUG_PHP_EXT_COMPARE_LEVEL(AS_LOG_LEVEL_ERROR, E_ERROR, args)
#define DEBUG_PHP_EXT_WARNING(args...)        DEBUG_PHP_EXT_COMPARE_LEVEL(AS_LOG_LEVEL_WARN, E_WARNING, args)
#define DEBUG_PHP_EXT_DEBUG(args...)          DEBUG_PHP_EXT_COMPARE_LEVEL(AS_LOG_LEVEL_DEBUG, E_NOTICE, args)
#define DEBUG_PHP_EXT_INFO(args...)           DEBUG_PHP_EXT_COMPARE_LEVEL(AS_LOG_LEVEL_INFO, E_NOTICE, args)
#else
#define DEBUG_PHP_EXT_ERROR(TSRM, args...)
#define DEBUG_PHP_EXT_WARNING(TSRM, args...)
#define DEBUG_PHP_EXT_DEBUG(TSRM, args...)
#define DEBUG_PHP_EXT_INFO(TSRM, args...)
#endif

/*
 *******************************************************************************************************
 * AEROSPIKE DB CONNECTION STATES.
 *******************************************************************************************************
 */
#define AEROSPIKE_CONN_STATE_TRUE   1
#define AEROSPIKE_CONN_STATE_FALSE  0

/* 
 *******************************************************************************************************
 * MACROS TO COMPARE PHP TYPE OF THE SPECIFIED TYPE.
 *
 * @param type             The php type to be checked.
 *******************************************************************************************************
 */
#define PHP_IS_NULL(type)        (IS_NULL == type)
#define PHP_IS_NOT_NULL(type)    (IS_NULL != type)
#define PHP_IS_ARRAY(type)       (IS_ARRAY == type)
#define PHP_IS_NOT_ARRAY(type)   (IS_ARRAY != type)
#define PHP_IS_STRING(type)      (IS_STRING == type)
#define PHP_IS_NOT_STRING(type)  (IS_STRING != type)
#define PHP_IS_LONG(type)        (IS_LONG == type)
#define PHP_IS_NOT_LONG(type)    (IS_LONG != type)

/* 
 *******************************************************************************************************
 * MACROS TO CHECK PHP TYPE OF A zval*.
 *
 * @param zend_val         The zval * whose type is to be checked.
 *******************************************************************************************************
 */
#define PHP_TYPE_ISNULL(zend_val)        PHP_IS_NULL(Z_TYPE_P(zend_val))
#define PHP_TYPE_ISSTR(zend_val)         PHP_IS_STRING(Z_TYPE_P(zend_val))
#define PHP_TYPE_ISLONG(zend_val)        PHP_IS_LONG(Z_TYPE_P(zend_val))
#define PHP_TYPE_ISARR(zend_val)         PHP_IS_ARRAY(Z_TYPE_P(zend_val))
#define PHP_TYPE_ISNOTNULL(zend_val)     PHP_IS_NOT_NULL(Z_TYPE_P(zend_val))
#define PHP_TYPE_ISNOTSTR(zend_val)      PHP_IS_NOT_STRING(Z_TYPE_P(zend_val))
#define PHP_TYPE_ISNOTLONG(zend_val)     PHP_IS_NOT_LONG(Z_TYPE_P(zend_val))
#define PHP_TYPE_ISNOTARR(zend_val)      PHP_IS_NOT_ARRAY(Z_TYPE_P(zend_val))
 
/* 
 *******************************************************************************************************
 * MACRO TO CHECK IF GIVEN CONNECTION TO AEROSPIKE DB IS ESTABLISHED.
 *
 * @param conn_state        The connection state to be compared (0/1).
 *******************************************************************************************************
 */
#define PHP_IS_CONN_NOT_ESTABLISHED(conn_state)   (conn_state == AEROSPIKE_CONN_STATE_FALSE)

/*
 *******************************************************************************************************
 * AEROSPIKE DEFAULT ERROR VALUES FOR CODE AND MESSAGE (in case of no error).
 *******************************************************************************************************
 */
#define DEFAULT_ERRORNO 0
#define DEFAULT_ERROR ""

/*
 *******************************************************************************************************
 * MACRO TO SET ERROR INTO AN as_error OBJECT.
 *
 * @param as_err_obj_p         The C client's as_error object to be set.
 * @param code                 The AEROSPIKE_x error code to be set.
 * @param msg                  The error message string to be set.
 *******************************************************************************************************
 */
#define PHP_EXT_SET_AS_ERR(as_err_obj_p, code, msg)                     as_error_setall(as_err_obj_p, code, msg, __func__, __FILE__, __LINE__)

/* 
 *******************************************************************************************************
 * MACROS TO SET ERROR IN AEROSPIKE CLASS MEMBERS.
 *
 * @param aerospike_class_p         The zend class entry for Aerospike class.
 * @param as_err_obj_p              The C client's as_error whose value is to be
 *                                  read and set into the class member.
 *******************************************************************************************************
 */
//#define PHP_EXT_SET_AS_ERR_IN_CLASS(aerospike_class_p, as_err_obj_p)    aerospike_helper_set_error(aerospike_class_p, getThis(), as_err_obj_p, false)
//#define PHP_EXT_RESET_AS_ERR_IN_CLASS(aerospike_class_p)                aerospike_helper_set_error(aerospike_class_p, getThis(), NULL, true)

#define PHP_EXT_SET_AS_ERR_IN_CLASS(as_err_obj_p) \
    memcpy(&(AEROSPIKE_G(error_g.error)), as_err_obj_p, sizeof(as_error)); \
    AEROSPIKE_G(error_g.reset) = 0;

#define PHP_EXT_RESET_AS_ERR_IN_CLASS() \
    memset(&(AEROSPIKE_G(error_g.error)), 0, sizeof(as_error)); \
    AEROSPIKE_G(error_g.reset) = 1;
/*
 *******************************************************************************************************
 * Extern declarations of transform functions.
 *******************************************************************************************************
 */
extern bool AS_DEFAULT_GET(const char *key, const as_val *value, void *array);

extern as_status
aerospike_transform_iterate_for_rec_key_params(HashTable* ht_p,
        as_key* as_key_p, int16_t* set_val_p);

extern as_status
aerospike_transform_check_and_set_config(HashTable* ht_p, zval** retdata_pp,
        void* config_p);

extern as_status
aerospike_transform_key_data_put(aerospike* as_object_p,
                                 zval **record_pp,
                                 as_key* as_key_p,
                                 as_error *error_p,
                                 u_int32_t ttl_u32,
                                 zval* options_p TSRMLS_DC);

extern as_status
aerospike_transform_get_record(Aerospike_object* aerospike_object_p,
                               as_key* get_rec_key_p,
                               zval* options_p,
                               as_error *error_p,
                               zval* get_record_p,
                               zval* bins_p TSRMLS_DC);

/*
 *******************************************************************************************************
 * Extern declarations of record operation functions.
 *******************************************************************************************************
 */
extern as_status
aerospike_record_operations_exists(aerospike* as_object_p,
                                   as_key* as_key_p,
                                   as_error *error_p,
                                   zval* metadata_p,
                                   zval* options_p TSRMLS_DC);
extern as_status
aerospike_record_operations_remove(Aerospike_object* aerospike_object_p,
                                   as_key* as_key_p,
                                   as_error *error_p,
                                   zval* options_p);
extern as_status
aerospike_record_operations_ops(Aerospike_object* aerospike_object_p,
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
aerospike_record_operations_remove_bin(Aerospike_object* aerospike_object_p,
                                       as_key* as_key_p,
                                       zval* bin_name_p,
                                       as_error* error_p,
                                       zval* options_p);

extern as_status
aerospike_php_exists_metadata(Aerospike_object*  aerospike_object_p,
                              zval* key_record_p,
                              zval* metadata_p,
                              zval* options_p,
                              as_error *error_p);

/*
 *******************************************************************************************************
 * Extern declarations of helper functions.
 *******************************************************************************************************
 */
extern void
aerospike_helper_set_error(zend_class_entry *ce_p,
                           zval *object_p TSRMLS_DC);

extern as_status
aerospike_helper_object_from_alias_hash(Aerospike_object* as_object_p,
                                        bool persist_flag,
                                        as_config* conf,
                                        HashTable *persistent_list,
                                        int persist TSRMLS_DC);

extern void
aerospike_helper_free_static_pool(as_static_pool *static_pool);

/*
 ******************************************************************************************************
 * Extern declarations of UDF functions.
 ******************************************************************************************************
 */
extern as_status
aerospike_udf_register(Aerospike_object* aerospike_obj_p, as_error* error_p,
        char *path_p, char *module_p, long language, zval *options_p);

extern as_status
aerospike_udf_deregister(Aerospike_object* aerospike_obj_p, as_error* error_p,
        char *module_p, zval *options_p);

extern as_status
aerospike_udf_apply(Aerospike_object* aerospike_obj_p, as_key* as_key_p,
        as_error* error_p, char* module_p, char* function_p, zval** args_pp,
        zval* return_value_p, zval* options_p);

extern as_status
aerospike_list_registered_udf_modules(Aerospike_object* aerospike_obj_p,
        as_error *error_p, zval* array_of_modules_p, long language,
        zval* options_p);

extern as_status
aerospike_get_registered_udf_module_code(Aerospike_object* aerospike_obj_p,
        as_error *error_p, char* module_p, zval* udf_code_p, long language,
        zval* options_p);

/*
 ******************************************************************************************************
 * Extern declarations of scan functions.
 ******************************************************************************************************
 */
extern as_status
aerospike_scan_run(aerospike* as_object_p, as_error* error_p,
        char* namespace_p, char* set_p, userland_callback* user_func_p,
        HashTable* bins_ht_p, /*long percent, long scan_priority,
        bool concurrent, bool no_bins,*/ zval* options_p TSRMLS_DC);

extern as_status
aerospike_scan_run_background(aerospike* as_object_p, as_error* error_p,
        char *module_p, char *function_p, zval** args_pp, char* namespace_p,
        char* set_p, zval* scan_id_p, /*long percent,
        long scan_priority, bool concurrent, bool no_bins,*/ zval *options_p TSRMLS_DC);

extern as_status
aerospike_scan_get_info(aerospike* as_object_p, as_error* error_p,
        uint64_t scan_id, zval* scan_info_p, zval* options_p TSRMLS_DC);

/*
 ******************************************************************************************************
 * Extern declarations of query functions.
 ******************************************************************************************************
 */
extern as_status
aerospike_query_run(aerospike* as_object_p, as_error* error_p, char* namespace_p,
        char* set_p, userland_callback* user_func_p, HashTable* bins_ht_p,
        HashTable* predicate_ht_p, zval* options_p TSRMLS_DC);

extern as_status
aerospike_query_aggregate(aerospike* as_object_p, as_error* error_p,
        const char* module_p, const char* function_p, zval** args_pp,
        char* namespace_p, char* set_p, HashTable* bins_ht_p,
        HashTable* predicate_ht_p, zval* return_value_p, zval* options_p TSRMLS_DC);

/*
 ******************************************************************************************************
 * Extern declarations of index functions.
 ******************************************************************************************************
 */
extern as_status
aerospike_index_create_php(aerospike* as_object_p, as_error *error_p,
        char* ns_p, char* set_p, char* bin_p, uint32_t type,
        char *name_p, zval* options_p TSRMLS_DC);

extern as_status
aerospike_index_remove_php(aerospike* as_object_p, as_error *error_p,
        char* ns_p, char *name_p, zval* options_p TSRMLS_DC);

/*
 ******************************************************************************************************
 * Extern declarations of info functions.
 ******************************************************************************************************
 */
extern as_status
aerospike_info_specific_host(aerospike* as_object_p, as_error* error_p,
        char* request, zval* response_p, zval* host, zval* options_p TSRMLS_DC);

extern as_status
aerospike_info_request_multiple_nodes(aerospike* as_object_p,
        as_error* error_p, char* request_str_p, zval* config_p,
        zval* return_value_p, zval* options_p);

extern as_status
aerospike_info_get_cluster_nodes(aerospike* as_object_p,
        as_error* error_p, zval* return_p, zval* host, zval* options_p TSRMLS_DC);
#endif
