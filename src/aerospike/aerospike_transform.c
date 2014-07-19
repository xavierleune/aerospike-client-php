#include "php.h"

#include "aerospike/as_status.h"
#include "aerospike/as_config.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_hashmap.h"
#include "aerospike/as_stringmap.h"
#include "aerospike/as_arraylist.h"

#include "aerospike_common.h"
#include "aerospike_transform.h"

#define PHP_AS_KEY_DEFINE_FOR_HOSTS                   "hosts"
#define PHP_AS_KEY_DEFINE_FOR_HOSTS_LEN               5
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

#define PHP_IS_NOT_ARRAY(type) (IS_ARRAY != type)
#define PHP_COMPARE_KEY(key_const, key_const_len, key_obtained, key_obtained_len)    \
     ((key_const_len == key_obtained_len) && (0 == memcmp(key_obtained, key_const, key_const_len)))
#define PHP_IS_STRING(type) (IS_STRING == type)
#define PHP_IS_LONG(type) (IS_LONG == type)

as_status AS_DEFAULT_PUT(void *key, void *value, as_record *record, void *static_pool);
as_status AS_LIST_PUT(void *key, void *value, void *store, void *static_pool);
as_status AS_MAP_PUT(void *key, void *value, void *store, void *static_pool);
bool AS_LIST_GET_CALLBACK(as_val *value, void *array);
bool AS_MAP_GET_CALLBACK(as_val *key, as_val *value, void *array);

/* GET helper functions */

/* Wrappers for appeding datatype to List */

static as_status ADD_LIST_APPEND_NULL(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_next_index_null(*((zval**)array));
    return (status);
}

static as_status ADD_LIST_APPEND_BOOL(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_next_index_bool(*((zval**)array),
            (int8_t) as_boolean_get((as_boolean *) value));
    return (status);
}

static as_status ADD_LIST_APPEND_LONG(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_next_index_long(*((zval**)array),
            (long) as_integer_get((as_integer *) value));
    return (status);
}

static as_status ADD_LIST_APPEND_STRING(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_next_index_stringl(*((zval**)array),
            as_string_get((as_string *) value),
            strlen(as_string_get((as_string *) value)), 1);
    return (status);
}

static as_status ADD_LIST_APPEND_REC(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;

    return (status);
}

static as_status ADD_LIST_APPEND_PAIR(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;

    return (status);
}

static as_status ADD_LIST_APPEND_BYTES(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;

    return (status);
}

/* Wrappers for associating datatype with Map */

static as_status ADD_MAP_ASSOC_NULL(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_assoc_null(*((zval**)array), as_string_get((as_string *) key));
    return (status);
}

static as_status ADD_MAP_ASSOC_BOOL(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_assoc_bool(*((zval**)array), as_string_get((as_string *) key),
            (int) as_boolean_get((as_boolean *) value));
    return (status);
}

as_status ADD_MAP_ASSOC_LONG(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_assoc_long(*((zval**)array),  as_string_get((as_string *) key),
            (long) as_integer_get((as_integer *) value));
    return (status);
}

static as_status ADD_MAP_ASSOC_STRING(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_assoc_stringl(*((zval**)array), as_string_get((as_string *) key),
            as_string_get((as_string *) value),
            strlen(as_string_get((as_string *) value)), 1);
    return (status);
}

static as_status ADD_MAP_ASSOC_REC(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;

    return (status);
}

static as_status ADD_MAP_ASSOC_PAIR(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;

    return (status);
}

static as_status ADD_MAP_ASSOC_BYTES(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;

    return (status);
}

/* Wrappers for associating datatype with Record */

static as_status ADD_DEFAULT_ASSOC_NULL(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_assoc_null(((zval*)array), (char *) key);
    return (status);
}

static as_status ADD_DEFAULT_ASSOC_BOOL(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_assoc_bool(((zval*)array), (char*) key,
            (int) as_boolean_get((as_boolean *) value));
    return (status);
}

as_status ADD_DEFAULT_ASSOC_LONG(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_assoc_long(((zval*)array),  (char*) key,
            (long) as_integer_get((as_integer *) value));
    return (status);
}

static as_status ADD_DEFAULT_ASSOC_STRING(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_assoc_stringl(((zval*)array), (char*) key,
            as_string_get((as_string *) value),
            strlen(as_string_get((as_string *) value)), 1);
    return (status);
}

static as_status ADD_DEFAULT_ASSOC_REC(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;

    return (status);
}

static as_status ADD_DEFAULT_ASSOC_PAIR(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;

    return (status);
}

static as_status ADD_DEFAULT_ASSOC_BYTES(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;

    return (status);
}

/* GET helper functions with expanding macros */

static as_status ADD_LIST_APPEND_MAP(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    AS_APPEND_MAP_TO_LIST(key, value, array);
    return (status);
}

static as_status ADD_LIST_APPEND_LIST(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    AS_APPEND_LIST_TO_LIST(key, value, array);
    return (status);
}

static as_status ADD_MAP_ASSOC_MAP(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    AS_ASSOC_MAP_TO_MAP(key, value, array);
    return (status);
}

static as_status ADD_MAP_ASSOC_LIST(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    AS_ASSOC_LIST_TO_MAP(key, value, array);
    return (status);
}

static as_status ADD_DEFAULT_ASSOC_MAP(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    AS_ASSOC_MAP_TO_DEFAULT(key, value, array);
    return (status);
}

static as_status ADD_DEFAULT_ASSOC_LIST(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    AS_ASSOC_LIST_TO_DEFAULT(key, value, array);
    return (status);
}

/* GET callback methods where switch case will expand */

bool AS_DEFAULT_GET(const char *key, const as_val *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    AEROSPIKE_WALKER_SWITCH_CASE_GET_DEFAULT_ASSOC(status, NULL, (void *) key, (void *) value, array, exit);
exit:
    return (true);
}

bool AS_LIST_GET_CALLBACK(as_val *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    AEROSPIKE_WALKER_SWITCH_CASE_GET_LIST_APPEND(status, NULL, NULL, (void *) value, array, exit);
exit:
    return (true);
}
bool AS_MAP_GET_CALLBACK(as_val *key, as_val *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    AEROSPIKE_WALKER_SWITCH_CASE_GET_MAP_ASSOC(status, NULL, (void *) key, (void *) value, array, exit);
exit:
    return (status);
}

/* End of helper functions for GET */

/* PUT helper functions */

/* Misc SET calls for GET and PUT */

static void AS_LIST_SET_APPEND_LIST(void* outer_store, void* inner_store, void* bin_name)
{
    as_arraylist_append_list((as_arraylist *)outer_store, (as_list*) inner_store);
}

static void AS_LIST_SET_APPEND_MAP(void* outer_store, void* inner_store, void* bin_name)
{
    as_arraylist_append_map((as_arraylist *)outer_store, (as_map*) inner_store);
}

static void AS_DEFAULT_SET_ASSOC_LIST(void* outer_store, void* inner_store, void* bin_name)
{
    as_record_set_list((as_record *)outer_store, (int8_t*)bin_name, (as_list *) inner_store);
}

static void AS_DEFAULT_SET_ASSOC_MAP(void* outer_store, void* inner_store, void* bin_name)
{
    as_record_set_map((as_record *)outer_store, (int8_t*)bin_name, (as_map *) inner_store);
}

static void AS_MAP_SET_ASSOC_LIST(void* outer_store, void* inner_store, void* bin_name)
{
    as_stringmap_set_list((as_map *)outer_store, bin_name, (as_list *) inner_store);
}

static void AS_MAP_SET_ASSOC_MAP(void* outer_store, void* inner_store, void* bin_name)
{
    as_stringmap_set_map((as_map *)outer_store, bin_name, (as_map *)inner_store);
}

/* Wrappers for appeding datatype to List */

static as_status AS_SET_ERROR_CASE(void* key, void* value, void* array, void* static_pool)
{
    return AEROSPIKE_ERR;
}

static as_status AS_LIST_PUT_APPEND_INT64(void* key, void *value, void *array, void *static_pool)
{
     as_arraylist_append_int64((as_arraylist *)value, (int64_t)Z_LVAL_P((zval*)array));
     return AEROSPIKE_OK;
}
 
static as_status AS_LIST_PUT_APPEND_STR(void *key, void *value, void *array, void *static_pool)
{
     as_arraylist_append_str((as_arraylist *)value, Z_STRVAL_P((zval*)array));
}

static as_status AS_LIST_PUT_APPEND_LIST(void *key, void *value, void *array, void *static_pool)
{
    as_status    status = AEROSPIKE_OK;
    AS_LIST_PUT(key, value, array, static_pool);
exit:
    return status;
}
 
static as_status AS_LIST_PUT_APPEND_MAP(void *key, void *value, void *array, void *static_pool) 
{
    as_status    status = AEROSPIKE_OK;
    AS_LIST_PUT(key, value, array, static_pool);
exit:
    return status;
}

/* Wrappers for associating datatype with Record */

static as_status AS_DEFAULT_PUT_ASSOC_NIL(void* key, void* value, void* array, void* static_pool)
{
    /* value holds the name of the bin*/
    as_record_set_nil((as_record *)(key), (int8_t *)value);
    return AEROSPIKE_OK;
}

static as_status AS_DEFAULT_PUT_ASSOC_INT64(void* key, void* value, void* array, void* static_pool)
{
    as_record_set_int64((as_record *)array, (int8_t *)key, (int64_t) Z_LVAL_P((zval*)value)); //changed from Z_LVAL_PP to Z_LVAL_P
    return AEROSPIKE_OK;
}

static as_status AS_DEFAULT_PUT_ASSOC_STR(void *key, void *value, void *array, void *static_pool)
{
    as_record_set_str((as_record *)array, (int8_t *)key, (char *) Z_STRVAL_P((zval*)value));
    return AEROSPIKE_OK;;
}

static as_status AS_DEFAULT_PUT_ASSOC_LIST(void *key, void *value, void *array, void *static_pool)
{
     as_status    status = AEROSPIKE_OK;
     AS_DEFAULT_PUT(key, value, array, static_pool);
exit:
    return status; 
}

static as_status AS_DEFAULT_PUT_ASSOC_MAP(void *key, void *value, void *array, void *static_pool)
{
     as_status    status = AEROSPIKE_OK;
     AS_DEFAULT_PUT(key, value, array, static_pool);
exit:
    return status;
}

/* Wrappers for associating datatype with MAP */

static as_status AS_MAP_PUT_ASSOC_INT64(void *key, void *value, void *store, void *static_pool)
{
    as_stringmap_set_int64((as_map*)store, (char *)key, Z_LVAL_PP((zval**)value));
    return AEROSPIKE_OK;
}

static as_status AS_MAP_PUT_ASSOC_STR(void *key, void *value, void *store, void *static_pool)
{
    as_stringmap_set_str((as_map*)store, (char*)key, Z_STRVAL_PP((zval**)value));
    return AEROSPIKE_ERR;
}

static as_status AS_MAP_PUT_ASSOC_MAP(void *key, void *value, void *store, void *static_pool)
{
    as_status    status = AEROSPIKE_OK;
    status = AS_MAP_PUT(key, value, store, static_pool);
exit:
    return status;
}

static as_status AS_MAP_PUT_ASSOC_LIST(void *key, void *value, void *store, void *static_pool)
{
    as_status    status = AEROSPIKE_OK;
    status = AS_MAP_PUT(key, value, store, static_pool);
exit:
     return status;
}

/* PUT functions whoes macros will expand */

static as_status AS_DEFAULT_PUT_ASSOC_ARRAY(void *key, void *value, void *store, void *static_pool);
static as_status AS_MAP_PUT_ASSOC_ARRAY(void *key, void *value, void *store, void *static_pool);
static as_status AS_LIST_PUT_APPEND_ARRAY(void *key, void *value, void *store, void *static_pool);

as_status AS_DEFAULT_PUT(void *key, void *value, as_record *record, void *static_pool)
{
    as_status status;
    AEROSPIKE_WALKER_SWITCH_CASE_PUT_DEFAULT_ASSOC(status, static_pool,
            key, ((zval**)value), record, exit);
exit:
    return (true);
}

as_status AS_LIST_PUT(void *key, void *value, void *store, void *static_pool)
{
    as_status status = AEROSPIKE_OK;
    AEROSPIKE_WALKER_SWITCH_CASE_PUT_LIST_APPEND(status, static_pool,
            key, ((zval**)value), store, exit);
exit:
    return (status);
}

as_status AS_MAP_PUT(void *key, void *value, void *store, void *static_pool)
{
    as_status status = AEROSPIKE_OK;
    AEROSPIKE_WALKER_SWITCH_CASE_PUT_MAP_ASSOC(status, ((as_static_pool*)static_pool),
            key, ((zval**)value), store, exit);
exit:
    return (status);
}

static as_status AS_DEFAULT_PUT_ASSOC_ARRAY(void *key, void *value, void *store, void *static_pool)
{
    as_status status = AEROSPIKE_OK;
    AEROSPIKE_PROCESS_ARRAY_DEFAULT_ASSOC_MAP(key, value, store, status,
           static_pool, exit);
    AEROSPIKE_PROCESS_ARRAY_DEFAULT_ASSOC_LIST(key, value, store, status,
           static_pool, exit);
exit:
    return (status);
}

static as_status AS_MAP_PUT_ASSOC_ARRAY(void *key, void *value, void *store, void *static_pool)
{
    as_status status = AEROSPIKE_OK;
    AEROSPIKE_PROCESS_ARRAY_MAP_ASSOC_MAP(key, value, store, status,
           static_pool, exit);
    AEROSPIKE_PROCESS_ARRAY_MAP_ASSOC_LIST(key, value, store, status,
           static_pool, exit);
exit:
    return (status);
}

static as_status AS_LIST_PUT_APPEND_ARRAY(void *key, void *value, void *store, void *static_pool)
{
    as_status status = AEROSPIKE_OK;
    AEROSPIKE_PROCESS_ARRAY_MAP_ASSOC_MAP(key, value, store, status,
           static_pool, exit);
    AEROSPIKE_PROCESS_ARRAY_MAP_ASSOC_LIST(key, value, store, status,
           static_pool, exit);
exit:
    return (status);
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

#define AS_CONFIG_ITER_MAP_SET_ADDR(map_p, val) map_p->as_config_p->hosts[map_p->iter_count_u32].addr = val
#define AS_CONFIG_ITER_MAP_SET_PORT(map_p, val) map_p->as_config_p->hosts[map_p->iter_count_u32].port = val
#define AS_CONFIG_ITER_MAP_IS_ADDR_SET(map_p)   (map_p->as_config_p->hosts[map_p->iter_count_u32].addr)
#define AS_CONFIG_ITER_MAP_IS_PORT_SET(map_p)   (map_p->as_config_p->hosts[map_p->iter_count_u32].port)

static as_status 
aerospike_transform_iterateKey(HashTable* ht_p, zval** retdata_pp, 
                               aerospike_transform_key_callback keycallback_p,
                               void* data_p, int16_t single_iter_16t)
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

        if (single_iter_16t) {
            break;
        }
    }
exit:

    if ((retdata_pp) && (keyData_pp)) {
        *retdata_pp = *keyData_pp;
    }

    return status;
}

static as_status
aerospike_transform_hostkey_callback(HashTable* ht_p,
                                     u_int32_t key_data_type_u32,
                                     int8_t* key_p, u_int32_t key_len_u32,
                                     void* data_p, zval** retdata_pp)
{
    as_status      status = AEROSPIKE_OK;

    if (PHP_IS_NOT_ARRAY(key_data_type_u32) || 
        !PHP_COMPARE_KEY(PHP_AS_KEY_DEFINE_FOR_HOSTS, PHP_AS_KEY_DEFINE_FOR_HOSTS_LEN, key_p, key_len_u32 - 1)) {
        status = AEROSPIKE_ERR_PARAM;
        goto exit;
    }

exit:
    return status;
}

extern as_status
aerospike_transform_iteratefor_hostkey(HashTable* ht_p, zval** retdata_pp)
{
    as_status      status = AEROSPIKE_OK;

    if (!ht_p) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterateKey(ht_p, retdata_pp, 
                                                                 &aerospike_transform_hostkey_callback,
                                                                 NULL, 1))) {
        goto exit;
    }

exit:
    return status;
}

static
as_status aerospike_transform_nameport_callback(HashTable* ht_p,
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
                                                                 &aerospike_transform_nameport_callback,
                                                                 (void *)data_p, 0))) {
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

extern as_status
aerospike_transform_iteratefor_name_port(HashTable* ht_p, as_config* as_config_p)
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
                                                                 (void *)&config_iter_map, 0))) {
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
                                                                     (void *)&put_key_data_map, 0))) {
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

static as_status
aerospike_transform_iterate_records(zval **record_pp, as_record* record, as_static_pool*  static_pool)
{
    as_status          status = AEROSPIKE_OK;
    char*               key = NULL;

    if ((!record_pp) || !(record) || !(static_pool)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    /* switch case statements for put for zend related data types */
    if (AEROSPIKE_OK != (status = AS_DEFAULT_PUT(key, record_pp, record, static_pool))) {
        goto exit;
    }

exit:
    return status;
}

extern as_status
aerospike_transform_key_data_put(aerospike* as_object_p,
                                 zval **record_pp,
                                 as_key* as_key_p,
                                 as_error *error_p,
                                 zval* options_p)
{
    as_status                   status = AEROSPIKE_OK;
    as_policy_write             write_policy;
    as_static_pool              static_pool = {0};
    as_record*                  record;
    u_int32_t                   iter = 0;

    if ((!record_pp) || (!as_key_p) || (!error_p) || (!as_object_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }


    if (AEROSPIKE_OK != (status = aerospike_transform_iterate_records(record_pp, record, &static_pool))) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    if (AEROSPIKE_OK != (status = set_policy(NULL, &write_policy, options_p))) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_key_put(as_object_p, error_p, &write_policy, as_key_p, record))) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

exit:
    /* clean up the as_* objects that were initialised */
    for (iter = 0; iter < static_pool.current_list_id; iter++) {
        as_arraylist_destroy(&static_pool.alloc_list[iter]);
    }

    for (iter = 0; iter < static_pool.current_map_id; iter++) {
        as_hashmap_destroy(&static_pool.alloc_map[iter]);
    }

    /*policy_write, should it be destroyed ??? */
    if (record) {
        as_record_destroy(record);
    }

    return status;
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

as_status
aerospike_transform_get_record(aerospike* as_object_p,
                               as_key* get_rec_key_p,
                               zval* options_p,
                               as_error *error_p,
                               zval* get_record_p,
                               zval* bins_p)
{
    as_status         status = AEROSPIKE_OK;
    as_policy_read    read_policy;
    as_record         *get_record = NULL;

    if ((!as_object_p) || (!get_rec_key_p) || (!error_p) || (!get_record_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    if (AEROSPIKE_OK != (status = set_policy(&read_policy, NULL, options_p))) {
        goto exit;
    }

    if (bins_p != NULL && (AEROSPIKE_OK != (status = aerospike_transform_filter_bins_exists(
            as_object_p, Z_ARRVAL_P(bins_p), &get_record, error_p, get_rec_key_p,
            &read_policy)))) {
        goto exit;
    } else if (AEROSPIKE_OK != (status = aerospike_key_get(as_object_p, error_p, &read_policy, get_rec_key_p,
                &get_record))) {
        goto exit;
    }
    if (!as_record_foreach(get_record, (as_rec_foreach_callback) AS_DEFAULT_GET,
        get_record_p)) {
        status = AEROSPIKE_ERR_SERVER;
        goto exit;
    }

exit:
    if (get_record) {
        as_record_destroy(get_record);
    }
    return status;
}
