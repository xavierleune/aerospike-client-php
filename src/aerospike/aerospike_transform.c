#include "php.h"
#include "ext/standard/php_var.h"
#include "ext/standard/php_smart_str.h"

#include "aerospike/as_status.h"
#include "aerospike/as_config.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_hashmap.h"
#include "aerospike/as_arraylist.h"
#include "aerospike/as_bytes.h"

#include "aerospike_common.h"
#include "aerospike_transform.h"
#include "aerospike_policy.h"

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

#define PHP_COMPARE_KEY(key_const, key_const_len, key_obtained, key_obtained_len)    \
     ((key_const_len == key_obtained_len) && (0 == memcmp(key_obtained, key_const, key_const_len)))

void AS_DEFAULT_PUT(void *key, void *value, as_record *record,
                    void *static_pool, uint32_t serializer_policy, as_error *error_p);
void AS_LIST_PUT(void *key, void *value, void *store,
                 void *static_pool, uint32_t serializer_policy, as_error *error_p);
void AS_MAP_PUT(void *key, void *value, void *store,
                void *static_pool, uint32_t serializer_policy, as_error *error_p);
bool AS_LIST_GET_CALLBACK(as_val *value, void *array);
bool AS_MAP_GET_CALLBACK(as_val *key, as_val *value, void *array);
static as_status
aerospike_transform_iteratefor_addr_port(HashTable* ht_p, as_config* as_config_p);

/* 
 * PHP Userland Serializer callback
 */
zend_fcall_info       user_serializer_call_info;
zend_fcall_info_cache user_serializer_call_info_cache;
zval                  *user_serializer_callback_retval_p;
uint32_t              is_user_serializer_registered = 0;

/* 
 * PHP Userland Deserializer callback
 */
zend_fcall_info       user_deserializer_call_info;
zend_fcall_info_cache user_deserializer_call_info_cache;
zval                  *user_deserializer_callback_retval_p;
uint32_t              is_user_deserializer_registered = 0;

/*
 * Sets value of as_bytes with bytes from bytes_string.
 * Sets type of as_bytes to bytes_type.
 */
static void set_as_bytes(as_bytes *bytes,
                         uint8_t *bytes_string,
                         int32_t bytes_string_len,
                         int32_t bytes_type,
                         as_error *error_p)
{
    if((!bytes) || (!bytes_string)) {
        DEBUG_PHP_EXT_ERROR("Unable to set as_bytes");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to set as_bytes");
        goto exit;
    }

    as_bytes_init(bytes, bytes_string_len);

    if (!as_bytes_set(bytes, 0, bytes_string, bytes_string_len)) {
        DEBUG_PHP_EXT_ERROR("Unable to set as_bytes");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to set as_bytes");
    } else {
        as_bytes_set_type(bytes, bytes_type);
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);
    }
exit:
    return;
}

/*
 * If serialize_flag == true, executes the passed userland serializer callback,
 * by creating as_bytes (bytes) from the passed zval (value).
 * Else executes the passed userland deserializer callback,
 * by passing the as_bytes (bytes) to the deserializer and getting back
 * the corresponding zval (value)
 */
static void execute_user_callback(zend_fcall_info *user_callback_info,
                                  zend_fcall_info_cache *user_callback_info_cache,
                                  zval *user_callback_retval_p,
                                  as_bytes *bytes,
                                  zval **value,
                                  bool serialize_flag,
                                  as_error *error_p)
{
    zval**      params[1];
    zval*       bytes_string = NULL;
    int8_t*     bytes_val_p = bytes->value;

    ALLOC_INIT_ZVAL(bytes_string);
    ZVAL_STRINGL(bytes_string, bytes_val_p, bytes->size, 1);

    if (serialize_flag) {
        params[0] = value;
    } else {
        params[0] = &bytes_string;
    }

    user_callback_info->param_count = 1;
    user_callback_info->params = params;
    user_callback_info->retval_ptr_ptr = &user_callback_retval_p;

    if (zend_call_function(user_callback_info, user_callback_info_cache TSRMLS_CC) == SUCCESS &&
        user_callback_info->retval_ptr_ptr && *user_callback_info->retval_ptr_ptr) {

        if (serialize_flag) {
            COPY_PZVAL_TO_ZVAL(*bytes_string, *user_callback_info->retval_ptr_ptr);
            set_as_bytes(bytes, Z_STRVAL_P(bytes_string), 
                         bytes_string->value.str.len, AS_BYTES_BLOB, error_p);
        } else {
            COPY_PZVAL_TO_ZVAL(**value, *user_callback_info->retval_ptr_ptr);
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);
        }

    } else {
        if (serialize_flag) {
            DEBUG_PHP_EXT_ERROR("Unable to call user's registered serializer callback");
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to call user's registered serializer callback");
        } else {
            DEBUG_PHP_EXT_ERROR("Unable to call user's registered deserializer callback");
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to call user's registered deserializer callback");
        }
    }

    zval_ptr_dtor(&bytes_string);
}

/*
 * Checks serializer_policy.
 * Serializes zval (value) into as_bytes using serialization logic
 * based on serializer_policy.
 */
static void serialize_based_on_serializer_policy(int32_t serializer_policy,
                                                 as_bytes *bytes,
                                                 zval **value,
                                                 as_error *error_p)
{
    switch(serializer_policy) {
        case SERIALIZER_NONE:
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Cannot serialize: SERIALIZER_NONE selected");
            goto exit;
        case SERIALIZER_PHP:
            {
                php_serialize_data_t var_hash;
                smart_str buf = {0};
                PHP_VAR_SERIALIZE_INIT(var_hash);
                php_var_serialize(&buf, value, &var_hash TSRMLS_CC);
                PHP_VAR_SERIALIZE_DESTROY(var_hash);
                if (EG(exception)) {
                    smart_str_free(&buf);
                    DEBUG_PHP_EXT_ERROR("Unable to serialize using standard php serializer");
                    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to serialize using standard php serializer");
                    goto exit;
                } else if (buf.c) {
                    set_as_bytes(bytes, buf.c, buf.len, AS_BYTES_PHP, error_p);
                    if (AEROSPIKE_OK != (error_p->code)) {
                        goto exit;
                    }
                } else {
                    DEBUG_PHP_EXT_ERROR("Unable to serialize using standard php serializer");
                    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to serialize using standard php serializer");
                    goto exit;
                }
            }
            break;
        case SERIALIZER_JSON:
                /*
                 *   TODO:
                 *     Handle JSON serialization after support for AS_BYTES_JSON
                 *     is added in aerospike-client-c
                 */
                 DEBUG_PHP_EXT_ERROR("Unable to serialize using standard json serializer");
                 PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to serialize using standard json serializer");
                 goto exit;
        case SERIALIZER_USER:
            if (is_user_serializer_registered) {
                execute_user_callback(&user_serializer_call_info,
                                      &user_serializer_call_info_cache,
                                      user_serializer_callback_retval_p,
                                      bytes, value, true, error_p);
                if (AEROSPIKE_OK != (error_p->code)) {
                    goto exit;
                }
            } else {
                DEBUG_PHP_EXT_ERROR("No serializer callback registered");
                PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "No serializer callback registered");
                goto exit;
            }
            break;
        default:
            DEBUG_PHP_EXT_ERROR("Unsupported serializer");
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unsupported serializer");
            goto exit;
    }
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);
exit:
    return;
}

/*
 * Checks as_bytes->type.
 * Unserializes as_bytes into zval (retval) using unserialization logic
 * based on as_bytes->type. 
 */
static void unserialize_based_on_as_bytes_type(as_bytes  *bytes,
                                               zval      **retval,
                                               as_error  *error_p)
{
    int8_t*     bytes_val_p = NULL;

    if (!bytes || !(bytes->value)) {
        DEBUG_PHP_EXT_DEBUG("Invalid bytes");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Invalid bytes");
        goto exit;
    }

    bytes_val_p = bytes->value;

    ALLOC_INIT_ZVAL(*retval);

    switch(as_bytes_get_type(bytes)) {
        case AS_BYTES_PHP: {
                php_unserialize_data_t var_hash;
                PHP_VAR_UNSERIALIZE_INIT(var_hash);
                if (1 != php_var_unserialize(retval, (const unsigned char **) &(bytes_val_p),
                         (char *) bytes_val_p + bytes->size, &var_hash TSRMLS_CC)) {
                    DEBUG_PHP_EXT_ERROR("Unable to unserialize bytes using standard php unserializer");
                    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to unserialize bytes using standard php unserializer");
                    goto exit;
                }
                PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
            }
            break;
        case AS_BYTES_BLOB: {
                if (is_user_deserializer_registered) {
                    execute_user_callback(&user_deserializer_call_info,
                                          &user_deserializer_call_info_cache,
                                          user_deserializer_callback_retval_p,
                                          bytes, retval, false, error_p);
                    if(AEROSPIKE_OK != (error_p->code)) {
                        goto exit;
                    }
                } else {
                    DEBUG_PHP_EXT_ERROR("No unserializer callback registered");
                    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "No unserializer callback registered");
                    goto exit;
                }
            }
            break;
        default:
            DEBUG_PHP_EXT_ERROR("Unable to unserialize bytes");
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to unserialize bytes");
            goto exit;
    }

    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);
exit:
    return;
}

/* GET helper functions */

/* Wrappers for appeding datatype to List */

static void ADD_LIST_APPEND_NULL(void *key, void *value, void *array, void *err)
{
    add_next_index_null(*((zval**)array));
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_LIST_APPEND_BOOL(void *key, void *value, void *array, void *err)
{
    add_next_index_bool(*((zval**)array),
            (int8_t) as_boolean_get((as_boolean *) value));
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_LIST_APPEND_LONG(void *key, void *value, void *array, void *err)
{
    add_next_index_long(*((zval**)array),
            (long) as_integer_get((as_integer *) value));
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_LIST_APPEND_STRING(void *key, void *value, void *array, void *err)
{
    add_next_index_stringl(*((zval**)array),
            as_string_get((as_string *) value),
            strlen(as_string_get((as_string *) value)), 1);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_LIST_APPEND_REC(void *key, void *value, void *array, void *err)
{
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_LIST_APPEND_PAIR(void *key, void *value, void *array, void *err)
{
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_LIST_APPEND_BYTES(void *key, void *value, void *array, void *err)
{
    zval        *unserialized_zval = NULL;

    unserialize_based_on_as_bytes_type((as_bytes *) value,
                                &unserialized_zval, (as_error *) err);

    if (AEROSPIKE_OK != ((as_error *) err)->code) {
        DEBUG_PHP_EXT_ERROR("Unable to unserialize bytes");
        goto exit;
    }
    add_next_index_zval(*((zval**)array), unserialized_zval);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}

/* Wrappers for associating datatype with Map with string key*/

static void ADD_MAP_ASSOC_NULL(void *key, void *value, void *array, void *err)
{
    add_assoc_null(*((zval**)array), as_string_get((as_string *) key));
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_MAP_ASSOC_BOOL(void *key, void *value, void *array, void *err)
{
    add_assoc_bool(*((zval**)array), as_string_get((as_string *) key),
            (int) as_boolean_get((as_boolean *) value));
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_MAP_ASSOC_LONG(void *key, void *value, void *array, void *err)
{
    add_assoc_long(*((zval**)array),  as_string_get((as_string *) key),
            (long) as_integer_get((as_integer *) value));
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_MAP_ASSOC_STRING(void *key, void *value, void *array, void *err)
{
    add_assoc_stringl(*((zval**)array), as_string_get((as_string *) key),
            as_string_get((as_string *) value),
            strlen(as_string_get((as_string *) value)), 1);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_MAP_ASSOC_REC(void *key, void *value, void *array, void *err)
{
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_MAP_ASSOC_PAIR(void *key, void *value, void *array, void *err)
{
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_MAP_ASSOC_BYTES(void *key, void *value, void *array, void *err)
{
    zval        *unserialized_zval = NULL;

    unserialize_based_on_as_bytes_type((as_bytes *) value,
                                &unserialized_zval, (as_error *) err);
    if (AEROSPIKE_OK != ((as_error *) err)->code) {
        DEBUG_PHP_EXT_ERROR("Unable to unserialize bytes");
        goto exit;
    }
    add_assoc_zval(*((zval**)array), as_string_get((as_string *) key), unserialized_zval);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}

/* Wrappers for associating datatype with Map with integer key */

static void ADD_MAP_INDEX_NULL(void *key, void *value, void *array, void *err)
{
    add_index_null(*((zval**)array), (uint) as_integer_get((as_integer *) key));
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_MAP_INDEX_BOOL(void *key, void *value, void *array, void *err)
{
    add_index_bool(*((zval**)array), (uint) as_integer_get((as_integer *) key),
            (int) as_boolean_get((as_boolean *) value));
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_MAP_INDEX_LONG(void *key, void *value, void *array, void *err)
{
    add_index_long(*((zval**)array), (uint) as_integer_get((as_integer *) key),
            (long) as_integer_get((as_integer *) value));
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_MAP_INDEX_STRING(void *key, void *value, void *array, void *err)
{
    add_index_stringl(*((zval**)array), (uint) as_integer_get((as_integer *) key),
            as_string_get((as_string *) value),
            strlen(as_string_get((as_string *) value)), 1);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_MAP_INDEX_REC(void *key, void *value, void *array, void *err)
{
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_MAP_INDEX_PAIR(void *key, void *value, void *array, void *err)
{
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_MAP_INDEX_BYTES(void *key, void *value, void *array, void *err)
{
    zval        *unserialized_zval = NULL;

    unserialize_based_on_as_bytes_type((as_bytes *) value,
                                &unserialized_zval, (as_error *) err);
    if (AEROSPIKE_OK != ((as_error *) err)->code) {
        DEBUG_PHP_EXT_ERROR("Unable to unserialize bytes");
        goto exit;
    }
    add_index_zval(*((zval**)array), (uint) as_integer_get((as_integer *) key), 
            unserialized_zval);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}

/* Wrappers for associating datatype with Record */

static void ADD_DEFAULT_ASSOC_NULL(void *key, void *value, void *array, void *err)
{
    add_assoc_null(((zval*)array), (char *) key);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_DEFAULT_ASSOC_BOOL(void *key, void *value, void *array, void *err)
{
    add_assoc_bool(((zval*)array), (char*) key,
            (int) as_boolean_get((as_boolean *) value));
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_DEFAULT_ASSOC_LONG(void *key, void *value, void *array, void *err)
{
    add_assoc_long(((zval*)array),  (char*) key,
            (long) as_integer_get((as_integer *) value));
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_DEFAULT_ASSOC_STRING(void *key, void *value, void *array, void *err)
{
    add_assoc_stringl(((zval*)array), (char*) key,
            as_string_get((as_string *) value),
            strlen(as_string_get((as_string *) value)), 1);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_DEFAULT_ASSOC_REC(void *key, void *value, void *array, void *err)
{
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_DEFAULT_ASSOC_PAIR(void *key, void *value, void *array, void *err)
{
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_DEFAULT_ASSOC_BYTES(void *key, void *value, void *array, void *err)
{
    zval        *unserialized_zval = NULL;

    unserialize_based_on_as_bytes_type((as_bytes *) value,
                                &unserialized_zval, (as_error *) err);
    if (AEROSPIKE_OK != ((as_error *) err)->code) {
        DEBUG_PHP_EXT_ERROR("Unable to unserialize bytes");
        goto exit;
    }
    add_assoc_zval(((zval*)array), (char*) key, unserialized_zval);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}

/* GET helper functions with expanding macros */

static void ADD_LIST_APPEND_MAP(void *key, void *value, void *array, void *err)
{
    AS_APPEND_MAP_TO_LIST(key, value, array, err);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_LIST_APPEND_LIST(void *key, void *value, void *array, void *err)
{
    AS_APPEND_LIST_TO_LIST(key, value, array, err);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_MAP_ASSOC_MAP(void *key, void *value, void *array, void *err)
{
    AS_ASSOC_MAP_TO_MAP(key, value, array, err);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_MAP_ASSOC_LIST(void *key, void *value, void *array, void *err)
{
    AS_ASSOC_LIST_TO_MAP(key, value, array, err);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_MAP_INDEX_MAP(void *key, void *value, void *array, void *err)
{
    AS_INDEX_MAP_TO_MAP(key, value, array, err);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_MAP_INDEX_LIST(void *key, void *value, void *array, void *err)
{
    AS_INDEX_LIST_TO_MAP(key, value, array, err);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_DEFAULT_ASSOC_MAP(void *key, void *value, void *array, void *err)
{
    AS_ASSOC_MAP_TO_DEFAULT(key, value, array, err);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

static void ADD_DEFAULT_ASSOC_LIST(void *key, void *value, void *array, void *err)
{
    AS_ASSOC_LIST_TO_DEFAULT(key, value, array, err);
    PHP_EXT_SET_AS_ERR((as_error *) err, AEROSPIKE_OK, DEFAULT_ERROR);
}

/* GET callback methods where switch case will expand */

extern bool AS_DEFAULT_GET(const char *key, const as_val *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    AEROSPIKE_WALKER_SWITCH_CASE_GET_DEFAULT_ASSOC(((foreach_callback_udata *) array)->error_p, 
            NULL, (void *) key, (void *) value, ((foreach_callback_udata *) array)->udata_p, exit);

exit:
    return (((((foreach_callback_udata *) array)->error_p)->code == AEROSPIKE_OK) ? true : false);
}

bool AS_LIST_GET_CALLBACK(as_val *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    AEROSPIKE_WALKER_SWITCH_CASE_GET_LIST_APPEND(((foreach_callback_udata *) array)->error_p,
            NULL, NULL, (void *) value, ((foreach_callback_udata *) array)->udata_p, exit);

exit:
    return (((((foreach_callback_udata *) array)->error_p)->code == AEROSPIKE_OK) ? true : false);
}

bool AS_MAP_GET_CALLBACK(as_val *key, as_val *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    if (FETCH_VALUE_GET(key) == AS_INTEGER) {
        AEROSPIKE_WALKER_SWITCH_CASE_GET_MAP_INDEX(((foreach_callback_udata *) array)->error_p,
                NULL, (void *) key, (void *) value, ((foreach_callback_udata *) array)->udata_p,
                exit);
    } else {
        AEROSPIKE_WALKER_SWITCH_CASE_GET_MAP_ASSOC(((foreach_callback_udata *) array)->error_p,
                NULL, (void *) key, (void *) value, ((foreach_callback_udata *) array)->udata_p,
                exit);
    }

exit:
    return (((((foreach_callback_udata *) array)->error_p)->code == AEROSPIKE_OK) ? true : false);
}

/* End of helper functions for GET */

/* PUT helper functions */

/* Misc SET calls for GET and PUT */

static void AS_LIST_SET_APPEND_LIST(void* outer_store, void* inner_store,
        void* bin_name, as_error *error_p)
{
    if (AEROSPIKE_OK != ((error_p->code) =
                as_arraylist_append_list((as_arraylist *)outer_store, (as_list*) inner_store))) {
        DEBUG_PHP_EXT_DEBUG("Unable to append list to list");
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to append list to list");
        goto exit;
    }
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}

static void AS_LIST_SET_APPEND_MAP(void* outer_store, void* inner_store,
        void* bin_name, as_error *error_p)
{
    if (AEROSPIKE_OK != ((error_p->code) =
                as_arraylist_append_map((as_arraylist *)outer_store, (as_map*) inner_store))) {
        DEBUG_PHP_EXT_DEBUG("Unable to append map to list");
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to append map to list");
        goto exit;
    }
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}

static void AS_DEFAULT_SET_ASSOC_LIST(void* outer_store, void* inner_store,
        void* bin_name, as_error *error_p)
{
    if (!(as_record_set_list((as_record *)outer_store, (int8_t*)bin_name, (as_list *) inner_store))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set record to a list");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to set record to a list");
        goto exit;
    }
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}

static void AS_DEFAULT_SET_ASSOC_MAP(void* outer_store, void* inner_store,
        void* bin_name, as_error *error_p)
{
    if (!(as_record_set_map((as_record *)outer_store, (int8_t*)bin_name, (as_map *) inner_store))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set record to a map");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to set record to a map");
        goto exit;
    }
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}

static void AS_MAP_SET_ASSOC_LIST(void* outer_store, void* inner_store,
        void* bin_name, as_error *error_p)
{
    if (AEROSPIKE_OK != ((error_p->code) =
                as_hashmap_set((as_hashmap*)outer_store, bin_name, (as_val*)((as_list *) inner_store)))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set list to as_hashmap");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to set list to a hashmap");
        goto exit;
    }
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}

static void AS_MAP_SET_ASSOC_MAP(void* outer_store, void* inner_store,
        void* bin_name, as_error *error_p)
{
    if (AEROSPIKE_OK != ((error_p->code) = 
                as_hashmap_set((as_hashmap*)outer_store, bin_name, (as_val*)((as_map *) inner_store)))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set map to as_hashmap");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to set map to as_hashmap");
        goto exit;
    }
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}

/* Wrappers for appeding datatype to List */

static void AS_SET_ERROR_CASE(void* key, void* value, void* array,
                              void* static_pool, uint32_t serializer_policy, as_error *error_p)
{
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Error");
}

static void AS_LIST_PUT_APPEND_INT64(void* key, void *value, void *array,
        void *static_pool, uint32_t serializer_policy, as_error *error_p)
{
    if (AEROSPIKE_OK != (error_p->code =
                as_arraylist_append_int64((as_arraylist *) array, (int64_t)Z_LVAL_PP((zval**) value)))) {
        DEBUG_PHP_EXT_DEBUG("Unable to append integer to list");
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to append integer to list");
        goto exit;
    }
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}
 
static void AS_LIST_PUT_APPEND_STR(void *key, void *value, void *array,
        void *static_pool, uint32_t serializer_policy, as_error *error_p)
{
    if (AEROSPIKE_OK != (error_p->code =
                as_arraylist_append_str((as_arraylist *) array, Z_STRVAL_PP((zval**) value)))) {
        DEBUG_PHP_EXT_DEBUG("Unable to append string to list");
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to append string to list");
        goto exit;
    }
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}

static void AS_LIST_PUT_APPEND_LIST(void *key, void *value, void *array,
        void *static_pool, uint32_t serializer_policy, as_error *error_p)
{
    AS_LIST_PUT(key, value, array, static_pool, serializer_policy, error_p);
}
 
static void AS_LIST_PUT_APPEND_MAP(void *key, void *value, void *array,
        void *static_pool, uint32_t serializer_policy, as_error *error_p)
{
    AS_MAP_PUT(key, value, array, static_pool, serializer_policy, error_p);
}

/* Wrappers for associating datatype with Record */

static void AS_DEFAULT_PUT_ASSOC_NIL(void* key, void* value, void* array,
        void* static_pool, uint32_t serializer_policy, as_error *error_p)
{
    /* value holds the name of the bin*/
    if (!(as_record_set_nil((as_record *)(key), (int8_t *) Z_LVAL_PP((zval**) value)))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set record to nil");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to set record to nil");
        goto exit;
    }
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}

static void AS_DEFAULT_PUT_ASSOC_BYTES(void* key, void* value,
        void* array, void* static_pool, uint32_t serializer_policy, as_error *error_p)
{
    as_bytes     *bytes;
    GET_BYTES_POOL(bytes, static_pool, error_p, exit);

    serialize_based_on_serializer_policy(serializer_policy, bytes, (zval **) value, error_p);
    if (AEROSPIKE_OK != (error_p->code)) {
        goto exit;
    }

    if (!(as_record_set_bytes((as_record *)array, (int8_t *)key, bytes))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set record to bytes");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to set record to bytes");
        goto exit;
    }
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}

static void AS_DEFAULT_PUT_ASSOC_INT64(void* key, void* value, void* array,
        void* static_pool, uint32_t serializer_policy, as_error *error_p)
{
    if (!(as_record_set_int64((as_record *)array, (int8_t *)key, (int64_t) Z_LVAL_PP((zval**) value)))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set record to an int");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to set record to int");
        goto exit;
    }
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}

static void AS_DEFAULT_PUT_ASSOC_STR(void *key, void *value, void *array,
        void *static_pool, uint32_t serializer_policy, as_error *error_p)
{
    if (!(as_record_set_str((as_record *)array, (int8_t *)key, (char *) Z_STRVAL_PP((zval**) value)))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set record to a string");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to set record to a string");
        goto exit;
    }
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}

static void AS_DEFAULT_PUT_ASSOC_LIST(void *key, void *value, void *array,
        void *static_pool, uint32_t serializer_policy, as_error *error_p)
{
    AS_LIST_PUT(key, value, array, static_pool, serializer_policy, error_p);
}

static void AS_DEFAULT_PUT_ASSOC_MAP(void *key, void *value, void *array,
        void *static_pool, uint32_t serializer_policy, as_error *error_p)
{
    AS_MAP_PUT(key, value, array, static_pool, serializer_policy, error_p);
}

/* Wrappers for associating datatype with MAP */

static void AS_MAP_PUT_ASSOC_INT64(void *key, void *value, void *store,
        void *static_pool, uint32_t serializer_policy, as_error *error_p)
{
    as_integer  *map_int;
    GET_INT_POOL(map_int, static_pool, error_p, exit);
    as_integer_init(map_int, Z_LVAL_PP((zval**)value));
    if (AEROSPIKE_OK != ((error_p->code) =
                as_hashmap_set((as_hashmap*)store, (as_val *) key, (as_val *)(map_int)))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set integer value to as_hashmap");
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to set integer value to as_hashmap");
    } else {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);
    }
exit:
    return;
}

static void AS_MAP_PUT_ASSOC_STR(void *key, void *value, void *store,
        void *static_pool, uint32_t serializer_policy, as_error *error_p)
{
    as_string   *map_str;
    GET_STR_POOL(map_str, static_pool, error_p, exit);
    as_string_init(map_str, Z_STRVAL_PP((zval**)value), false);
    if (AEROSPIKE_OK != ((error_p->code) =
                as_hashmap_set((as_hashmap*)store, (as_val *) key, (as_val *)(map_str)))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set string value to as_hashmap");
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to set string value to as_hashmap");
    } else {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);
    }
exit:
    return;
}

static void AS_MAP_PUT_ASSOC_MAP(void *key, void *value, void *store,
        void *static_pool, uint32_t serializer_policy, as_error *error_p)
{
    AS_MAP_PUT(key, value, store, static_pool, serializer_policy, error_p);
}

static void AS_MAP_PUT_ASSOC_LIST(void *key, void *value, void *store,
        void *static_pool, uint32_t serializer_policy, as_error *error_p)
{
    AS_LIST_PUT(key, value, store, static_pool, serializer_policy, error_p);
}

static void AS_MAP_PUT_ASSOC_BYTES(void *key, void *value, void *store,
        void *static_pool, uint32_t serializer_policy, as_error *error_p)
{
    as_bytes     *bytes;
    GET_BYTES_POOL(bytes, static_pool, error_p, exit);

    serialize_based_on_serializer_policy(serializer_policy, bytes, (zval **) value, error_p);
    if (AEROSPIKE_OK != (error_p->code)) {
        goto exit;
    }

    if (AEROSPIKE_OK != ((error_p->code) =
                as_hashmap_set((as_hashmap*)store, (as_val *) key, (as_val *)(bytes)))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set byte value to as_hashmap");
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to set string value to as_hashmap");
    } else {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);
    }

exit:
    return;
}

static void AS_DEFAULT_PUT_ASSOC_ARRAY(void *key, void *value, void *store,
        void *static_pool, uint32_t serializer_policy, as_error *error_p);
static void AS_MAP_PUT_ASSOC_ARRAY(void *key, void *value, void *store,
        void *static_pool, uint32_t serializer_policy, as_error *error_p);
static void AS_LIST_PUT_APPEND_ARRAY(void *key, void *value, void *store,
        void *static_pool, uint32_t serializer_policy, as_error *error_p);
static void AS_DEFAULT_PUT_ASSOC_BYTES(void *key, void *value, void *store,
        void *static_pool, uint32_t serializer_policy, as_error *error_p);
static void AS_MAP_PUT_ASSOC_BYTES(void *key, void *value, void *store,
        void *static_pool, uint32_t serializer_policy, as_error *error_p);
static void AS_LIST_PUT_APPEND_BYTES(void *key, void *value, void *array,
        void *static_pool, uint32_t serializer_policy, as_error *error_p);

/* PUT functions whose macros will expand */
void AS_DEFAULT_PUT(void *key, void *value, as_record *record, void *static_pool,
        uint32_t serializer_policy, as_error *error_p)
{
    AEROSPIKE_WALKER_SWITCH_CASE_PUT_DEFAULT_ASSOC(error_p, static_pool,
            key, ((zval**)value), record, exit, serializer_policy);
exit:
    return;
}

void AS_LIST_PUT(void *key, void *value, void *store, void *static_pool,
        uint32_t serializer_policy, as_error *error_p)
{
    AEROSPIKE_WALKER_SWITCH_CASE_PUT_LIST_APPEND(error_p, static_pool,
            key, ((zval**)value), store, exit, serializer_policy);
exit:
    return;
}

void AS_MAP_PUT(void *key, void *value, void *store, void *static_pool,
        uint32_t serializer_policy, as_error *error_p)
{
    as_val *map_key = NULL;
    AEROSPIKE_WALKER_SWITCH_CASE_PUT_MAP_ASSOC(error_p, static_pool,
            map_key, ((zval**)value), store, exit, serializer_policy);
exit:
    return;
}

static void AS_DEFAULT_PUT_ASSOC_ARRAY(void *key, void *value, void *store,
        void *static_pool, uint32_t serializer_policy, as_error *error_p)
{
    AEROSPIKE_PROCESS_ARRAY(DEFAULT, ASSOC, exit, key, value, store, 
            error_p, static_pool, serializer_policy);
exit:
    return;
}

static void AS_MAP_PUT_ASSOC_ARRAY(void *key, void *value, void *store,
        void *static_pool, uint32_t serializer_policy, as_error *error_p)
{
    AEROSPIKE_PROCESS_ARRAY(MAP, ASSOC, exit, key, value, store, 
            error_p, static_pool, serializer_policy);
exit:
    return;
}

static void AS_LIST_PUT_APPEND_ARRAY(void *key, void *value, void *store,
        void *static_pool, uint32_t serializer_policy, as_error *error_p)
{
    AEROSPIKE_PROCESS_ARRAY(LIST, APPEND, exit, key, value, store, 
            error_p, static_pool, serializer_policy);
exit:
    return;
}

static void AS_LIST_PUT_APPEND_BYTES(void* key, void *value, void *array,
        void *static_pool, uint32_t serializer_policy, as_error *error_p)
{
    as_bytes     *bytes;
    GET_BYTES_POOL(bytes, static_pool, error_p, exit);

    serialize_based_on_serializer_policy(serializer_policy, bytes, (zval **) value, error_p);
    if(AEROSPIKE_OK != (error_p->code)) {
        goto exit;
    }

    if (AEROSPIKE_OK != ((error_p->code) =
                as_arraylist_append_bytes((as_arraylist *) array, bytes))) {
        DEBUG_PHP_EXT_DEBUG("Unable to append integer to list");
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to append integer to list");
        goto exit;
    }
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, DEFAULT_ERROR);

exit:
    return;
}

/* End of PUT helper functions */

typedef as_status (*aerospike_transform_key_callback)(HashTable* ht_p,
                                                      u_int32_t key_data_type_u32, 
                                                      int8_t* key_p, u_int32_t key_len_u32,
                                                      void* data_p, zval** retdata_pp);

typedef struct asputkeydatamap {
    int8_t* namespace_p;
    int8_t* set_p;
    zval**  key_pp;
} as_put_key_data_map;

typedef struct asconfig_iter {
    u_int32_t     iter_count_u32;
    as_config*    as_config_p;
}as_config_iter_map;

#define AS_CONFIG_ITER_MAP_SET_ADDR(map_p, val)                                \
do {                                                                           \
    map_p->as_config_p->hosts[map_p->iter_count_u32].addr = val;               \
    map_p->as_config_p->hosts_size++;                                          \
} while(0)    

#define AS_CONFIG_ITER_MAP_SET_PORT(map_p, val)  map_p->as_config_p->hosts[map_p->iter_count_u32].port = val
#define AS_CONFIG_SET_USER(as_config_p, val)     strcpy(as_config_p->user, val)
#define AS_CONFIG_SET_PASSWORD(as_config_p, val) strcpy(as_config_p->password, val)
#define AS_CONFIG_ITER_MAP_IS_ADDR_SET(map_p)    (map_p->as_config_p->hosts[map_p->iter_count_u32].addr)
#define AS_CONFIG_ITER_MAP_IS_PORT_SET(map_p)    (map_p->as_config_p->hosts[map_p->iter_count_u32].port)

static as_status
aerospike_transform_set_user_in_config(char *user_p, as_config *config_p)
{
    as_status      status = AEROSPIKE_OK;
    
    if (!user_p || !config_p) {
        status = AEROSPIKE_ERR;
        goto exit;
    }
    /*
     * TODO: Uncomment below line to set the user_p in config_p
     */
    /*AS_CONFIG_SET_USER(config_p, user_p);*/

exit:
    return status;
}

static as_status
aerospike_transform_set_password_in_config(char *password_p, as_config *config_p)
{
    as_status      status = AEROSPIKE_OK;
    
    if (!password_p || !config_p) {
        status = AEROSPIKE_ERR;
        goto exit;
    }
    /*
     * TODO: Uncomment below line to set the password_p in config_p
     */
    /*AS_CONFIG_SET_PASSWORD(config_p, password_p);*/

exit:
    return status;
}

static as_status 
aerospike_transform_iterateKey(HashTable* ht_p, zval** retdata_pp, 
                               aerospike_transform_key_callback keycallback_p,
                               void* data_p)
{
    as_status        status = AEROSPIKE_OK;
    HashPosition     hashPosition_p = NULL;
    zval**           keyData_pp = NULL;

    foreach_hashtable(ht_p, hashPosition_p, keyData_pp) {
        int8_t*     key_value_p = NULL;
        u_int32_t   key_len_u32 = 0;
        u_int64_t   index_u64 = 0;
        u_int32_t   key_type_u32 = zend_hash_get_current_key_ex(ht_p, 
                                                                (char **)&key_value_p,
                                                                &key_len_u32,
                                                                &index_u64, 0,
                                                                &hashPosition_p);

        /* check for key type , need to know what it is*/

        if(keycallback_p) {
            if (AEROSPIKE_OK != (status = keycallback_p(ht_p,
                                                        Z_TYPE_PP(keyData_pp),
                                                        key_value_p, key_len_u32,
                                                        data_p, keyData_pp))) {
                goto exit;
            }
        }
    }
exit:

    if ((retdata_pp) && (keyData_pp)) {
        *retdata_pp = *keyData_pp;
    }

    return status;
}

static as_status
aerospike_transform_config_callback(HashTable* ht_p,
                                    u_int32_t key_data_type_u32,
                                    int8_t* key_p, u_int32_t key_len_u32,
                                    void* data_p, zval** retdata_pp)
{
    as_status      status = AEROSPIKE_OK;

    if (PHP_IS_ARRAY(key_data_type_u32) && 
        PHP_COMPARE_KEY(PHP_AS_KEY_DEFINE_FOR_HOSTS, PHP_AS_KEY_DEFINE_FOR_HOSTS_LEN, key_p, key_len_u32 - 1)) {
            status = aerospike_transform_iteratefor_addr_port(Z_ARRVAL_PP(retdata_pp), (as_config *) data_p);
    } else if (PHP_IS_STRING(key_data_type_u32) && 
        PHP_COMPARE_KEY(PHP_AS_KEY_DEFINE_FOR_USER, PHP_AS_KEY_DEFINE_FOR_USER_LEN, key_p, key_len_u32 -1)) {
            status = aerospike_transform_set_user_in_config(Z_STRVAL_PP(retdata_pp), (as_config *) data_p);
    } else if (PHP_IS_STRING(key_data_type_u32) && 
        PHP_COMPARE_KEY(PHP_AS_KEY_DEFINE_FOR_PASSWORD, PHP_AS_KEY_DEFINE_FOR_PASSWORD_LEN, key_p, key_len_u32 -1)) {
            status = aerospike_transform_set_password_in_config(Z_STRVAL_PP(retdata_pp), (as_config *) data_p);
    } else {
        status = AEROSPIKE_ERR_PARAM;
        goto exit;
    }

exit:
    return status;
}

extern as_status
aerospike_transform_check_and_set_config(HashTable* ht_p, zval** retdata_pp, as_config *config_p)
{
    as_status      status = AEROSPIKE_OK;

    if (!ht_p) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterateKey(ht_p, NULL/*retdata_pp*/, 
                                                                 &aerospike_transform_config_callback,
                                                                 config_p))) {
        goto exit;
    }

exit:
    return status;
}

static
as_status aerospike_transform_addrport_callback(HashTable* ht_p,
                                                u_int32_t key_data_type_u32,
                                                int8_t* key_p, u_int32_t key_len_u32,
                                                void* data_p, zval** retdata_pp)
{
    as_status                status = AEROSPIKE_OK;
    as_config_iter_map*      as_config_iter_map_p = (as_config_iter_map *)(data_p);

    if ((!as_config_iter_map_p) || (!retdata_pp) || (!as_config_iter_map_p->as_config_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    if (PHP_IS_STRING(key_data_type_u32) &&
        PHP_COMPARE_KEY(PHP_AS_KEY_DEFINE_FOR_ADDR, PHP_AS_KEY_DEFINE_FOR_ADDR_LEN, key_p, key_len_u32 - 1)) {
        AS_CONFIG_ITER_MAP_SET_ADDR(as_config_iter_map_p, Z_STRVAL_PP(retdata_pp));
    } else if(PHP_IS_LONG(key_data_type_u32) &&
             PHP_COMPARE_KEY(PHP_AS_KEY_DEFINE_FOR_PORT, PHP_AS_KEY_DEFINE_FOR_PORT_LEN, key_p, key_len_u32 - 1)) {
        AS_CONFIG_ITER_MAP_SET_PORT(as_config_iter_map_p, Z_LVAL_PP(retdata_pp));
    } else {
        status = AEROSPIKE_ERR_PARAM;
        goto exit;
    }

exit:
    return status;
}

static
as_status aerospike_transform_array_callback(HashTable* ht_p,
                                             u_int32_t key_data_type_u32,
                                             int8_t* key_p, u_int32_t key_len_u32,
                                             void* data_p, zval** retdata_pp)
{
    as_status      status = AEROSPIKE_OK;
    zval**         nameport_data_pp = NULL;

    if (PHP_IS_NOT_ARRAY(key_data_type_u32) || (!data_p) || (!retdata_pp)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterateKey(Z_ARRVAL_PP(retdata_pp),
                                                                 nameport_data_pp, 
                                                                 &aerospike_transform_addrport_callback,
                                                                 (void *)data_p))) {
        goto exit;
    }

    if ((!AS_CONFIG_ITER_MAP_IS_ADDR_SET(((as_config_iter_map *)data_p))) &&
        (!AS_CONFIG_ITER_MAP_IS_PORT_SET(((as_config_iter_map *)data_p)))) {
        /* address and port are not set so give error */
        status = AEROSPIKE_ERR_PARAM;
        goto exit;
    }

    ((as_config_iter_map *)data_p)->iter_count_u32++;

exit:
    return status;
}

static as_status
aerospike_transform_iteratefor_addr_port(HashTable* ht_p, as_config* as_config_p)
{
    as_status             status = AEROSPIKE_OK;
    as_config_iter_map    config_iter_map;

    if ((!ht_p) || (!as_config_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    config_iter_map.iter_count_u32 = 0;
    config_iter_map.as_config_p    = as_config_p;
    if (AEROSPIKE_OK != (status = aerospike_transform_iterateKey(ht_p, NULL/*retdata_pp*/,
                                                                 &aerospike_transform_array_callback,
                                                                 (void *)&config_iter_map))) {
        goto exit;
    }

    if (0 == config_iter_map.iter_count_u32) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

exit:
    return status;
}

static as_status
aerospike_add_key_params(as_key* as_key_p, u_int32_t key_type, int8_t* namespace_p, int8_t* set_p, zval** record_pp)
{
    as_status      status = AEROSPIKE_OK;

    if ((!as_key_p) || (!namespace_p) || (!set_p) || (!record_pp)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    switch(key_type) {
        case IS_LONG:
            as_key_init_int64(as_key_p, namespace_p, set_p, (int64_t) Z_LVAL_PP(record_pp));
            break;
        case IS_STRING:
            as_key_init_str(as_key_p, namespace_p, set_p, (int8_t *) Z_STRVAL_PP(record_pp));
            break;
        default:
            status = AEROSPIKE_ERR;
            break;
    }

exit:
    return status;
}

static as_status
aerospike_transform_putkey_callback(HashTable* ht_p,
                                    u_int32_t key_data_type_u32,
                                    int8_t* key_p, u_int32_t key_len_u32,
                                    void* data_p, zval** retdata_pp) {
    as_status                 status = AEROSPIKE_OK;
    as_put_key_data_map*      as_put_key_data_map_p = (as_put_key_data_map *)(data_p);

    if (!as_put_key_data_map_p) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    if (PHP_IS_STRING(key_data_type_u32) &&
        PHP_COMPARE_KEY(PHP_AS_KEY_DEFINE_FOR_NS, PHP_AS_KEY_DEFINE_FOR_NS_LEN, key_p, key_len_u32 - 1)) {
        as_put_key_data_map_p->namespace_p = Z_STRVAL_PP(retdata_pp);
    } else if(PHP_IS_STRING(key_data_type_u32) &&
             PHP_COMPARE_KEY(PHP_AS_KEY_DEFINE_FOR_SET, PHP_AS_KEY_DEFINE_FOR_SET_LEN, key_p, key_len_u32 - 1)) {
        as_put_key_data_map_p->set_p = Z_STRVAL_PP(retdata_pp);
    } else if(/*PHP_IS_STRING(key_data_type_u32) &&  -- need to check on the type*/
             PHP_COMPARE_KEY(PHP_AS_KEY_DEFINE_FOR_KEY, PHP_AS_KEY_DEFINE_FOR_KEY_LEN, key_p, key_len_u32 - 1)) {
        as_put_key_data_map_p->key_pp = retdata_pp;
    } else {
        status = AEROSPIKE_ERR_PARAM;
        goto exit;
    }

exit:
    return status;
}

extern as_status
aerospike_transform_iterate_for_rec_key_params(HashTable* ht_p, as_key* as_key_p, int16_t *set_val_p)
{
    as_status            status = AEROSPIKE_OK;
    as_put_key_data_map  put_key_data_map = {0};
    HashPosition         hashPosition_p = NULL;
    zval*                key_record_p = NULL;
    u_int64_t            index_u64 = 0;

    if ((!ht_p) || (!as_key_p) || (!as_key_p) || (!set_val_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    foreach_hashtable(ht_p, hashPosition_p, index_u64) {
        if (AEROSPIKE_OK != (status = aerospike_transform_iterateKey(ht_p, &key_record_p,
                                                                     &aerospike_transform_putkey_callback,
                                                                     (void *)&put_key_data_map))) {
            goto exit;
        }
    }

    if (!(put_key_data_map.namespace_p) || !(put_key_data_map.set_p) || !(put_key_data_map.key_pp) || (!key_record_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    if(AEROSPIKE_OK != (status = aerospike_add_key_params(as_key_p,
                                                          Z_TYPE_P(key_record_p),
                                                          put_key_data_map.namespace_p,
                                                          put_key_data_map.set_p,
                                                          &key_record_p))) {
        goto exit;
    }

    *set_val_p = 1;

exit:
    return status;
}

static void
aerospike_transform_iterate_records(zval **record_pp,
                                    as_record* record,
                                    as_static_pool* static_pool,
                                    uint32_t serializer_policy,
                                    as_error *error_p)
{
    char*              key = NULL;

    if ((!record_pp) || !(record) || !(static_pool)) {
        DEBUG_PHP_EXT_DEBUG("Unable to put record");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to put record");
        goto exit;
    }

    /* switch case statements for put for zend related data types */
    AS_DEFAULT_PUT(key, record_pp, record, static_pool, serializer_policy, error_p);

exit:
    return;
}

extern as_status
aerospike_transform_key_data_put(aerospike* as_object_p,
                                 zval **record_pp,
                                 as_key* as_key_p,
                                 as_error *error_p,
                                 zval* options_p)
{
    as_policy_write             write_policy;
    uint32_t                    serializer_policy = -1;
    as_static_pool              static_pool = {0};
    as_record                   record;
    int16_t                     init_record = 0;
    u_int32_t                   iter = 0;

    if ((!record_pp) || (!as_key_p) || (!error_p) || (!as_object_p)) {
        DEBUG_PHP_EXT_DEBUG("Unable to put record");
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to put record");
        goto exit;
    }

    as_record_inita(&record, zend_hash_num_elements(Z_ARRVAL_PP(record_pp)));
    init_record = 1;

    set_policy(NULL, &write_policy, NULL, NULL, &serializer_policy, options_p, error_p);
    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    aerospike_transform_iterate_records(record_pp, &record, &static_pool, serializer_policy, error_p);
    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to put record");
        goto exit;
    }

    aerospike_key_put(as_object_p, error_p, &write_policy, as_key_p, &record);

exit:
    /* clean up the as_* objects that were initialised */
    for (iter = 0; iter < static_pool.current_str_id; iter++) {
        as_string_destroy(&static_pool.string_pool[iter]);
    }

    for (iter = 0; iter < static_pool.current_int_id; iter++) {
        as_integer_destroy(&static_pool.integer_pool[iter]);
    }

    for (iter = 0; iter < static_pool.current_bytes_id; iter++) {
        as_bytes_destroy(&static_pool.bytes_pool[iter]);
    }

    for (iter = 0; iter < static_pool.current_list_id; iter++) {
        as_arraylist_destroy(&static_pool.alloc_list[iter]);
    }

    for (iter = 0; iter < static_pool.current_map_id; iter++) {
        as_hashmap_destroy(&static_pool.alloc_map[iter]);
    }

    /*policy_write, should it be destroyed ??? */
    if (init_record) {
        as_record_destroy(&record);
    }

    return error_p->code;
}

extern as_status
aerospike_transform_filter_bins_exists(aerospike *as_object_p,
                                       HashTable *bins_array_p,
                                       as_record **get_record_p,
                                       as_error *error_p,
                                       as_key *get_rec_key_p,
                                       as_policy_read *read_policy_p)
{
    int                 bins_count = zend_hash_num_elements(bins_array_p);
    as_status           status = AEROSPIKE_OK;
    uint                sel_cnt = 0;
    const char          *select[bins_count];
    HashPosition        pointer;
    zval                **bin_names;
    
    foreach_hashtable (bins_array_p, pointer, bin_names) {
        switch (Z_TYPE_PP(bin_names)) {
            case IS_STRING:
                select[sel_cnt++] = Z_STRVAL_PP(bin_names);
                break;
            default:
                status = AEROSPIKE_ERR_PARAM;
                goto exit;
        }
    }
    
    select[bins_count] = NULL;
    if (AEROSPIKE_OK != (status = aerospike_key_select(as_object_p, error_p, read_policy_p, get_rec_key_p,
        select, get_record_p))) {
        goto exit;
    }
exit:
    return status;
}

extern as_status
aerospike_transform_get_record(aerospike* as_object_p,
                               as_key* get_rec_key_p,
                               zval* options_p,
                               as_error *error_p,
                               zval* get_record_p,
                               zval* bins_p)
{
    as_status               status = AEROSPIKE_OK;
    as_policy_read          read_policy;
    as_record               *get_record = NULL;
    foreach_callback_udata  foreach_record_callback_udata;

    foreach_record_callback_udata.udata_p = get_record_p;
    foreach_record_callback_udata.error_p = error_p;

    if ((!as_object_p) || (!get_rec_key_p) || (!error_p) || (!get_record_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    set_policy(&read_policy, NULL, NULL, NULL, NULL, options_p, error_p);
    if (AEROSPIKE_OK != (status = (error_p->code))) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    if (bins_p != NULL && (AEROSPIKE_OK != (status = aerospike_transform_filter_bins_exists(
            as_object_p, Z_ARRVAL_P(bins_p), &get_record, error_p, get_rec_key_p,
            &read_policy)))) {
        goto exit;
    } else if (AEROSPIKE_OK != (status = aerospike_key_get(as_object_p, error_p, &read_policy,
                    get_rec_key_p, &get_record))) {
        goto exit;
    }
    if (!as_record_foreach(get_record, (as_rec_foreach_callback) AS_DEFAULT_GET,
        &foreach_record_callback_udata)) {
        status = AEROSPIKE_ERR_SERVER;
        goto exit;
    }

exit:
    if (get_record) {
        as_record_destroy(get_record);
    }
    return status;
}
