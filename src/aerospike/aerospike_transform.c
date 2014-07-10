#include "php.h"

#include "aerospike/as_status.h"
#include "aerospike/as_config.h"

#include "aerospike_common.h"

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
        !PHP_COMPARE_KEY(PHP_AS_KEY_DEFINE_FOR_HOSTS, PHP_AS_KEY_DEFINE_FOR_HOSTS_LEN, key_p, key_len_u32)) {
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
        PHP_COMPARE_KEY(PHP_AS_KEY_DEFINE_FOR_ADDR, PHP_AS_KEY_DEFINE_FOR_ADDR_LEN, key_p, key_len_u32)) {
        AS_CONFIG_ITER_MAP_SET_ADDR(as_config_iter_map_p, Z_STRVAL_PP(retdata_pp));
    } else if(PHP_IS_LONG(key_data_type_u32) &&
             PHP_COMPARE_KEY(PHP_AS_KEY_DEFINE_FOR_PORT, PHP_AS_KEY_DEFINE_FOR_PORT_LEN, key_p, key_len_u32)) {
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
        PHP_COMPARE_KEY(PHP_AS_KEY_DEFINE_FOR_NS, PHP_AS_KEY_DEFINE_FOR_NS_LEN, key_p, key_len_u32)) {
        as_put_key_data_map_p->namespace_p = Z_STRVAL_PP(retdata_pp);
    } else if(PHP_IS_STRING(key_data_type_u32) &&
             PHP_COMPARE_KEY(PHP_AS_KEY_DEFINE_FOR_SET, PHP_AS_KEY_DEFINE_FOR_SET_LEN, key_p, key_len_u32)) {
        as_put_key_data_map_p->set_p = Z_STRVAL_PP(retdata_pp);
    } else if(/*PHP_IS_STRING(key_data_type_u32) &&  -- need to check on the type*/
             PHP_COMPARE_KEY(PHP_AS_KEY_DEFINE_FOR_KEY, PHP_AS_KEY_DEFINE_FOR_KEY_LEN, key_p, key_len_u32)) {
        as_put_key_data_map_p->key_pp = retdata_pp;
    } else {
        status = AEROSPIKE_ERR_PARAM;
        goto exit;
    }

exit:
    return status;
}


extern as_status
aerospike_transform_iterate_for_put_key_params(HashTable* ht_p,
                                               int8_t** namespace_pp,
                                               int8_t** set_pp,
                                               zval** key_pp)
{
    as_status            status = AEROSPIKE_OK;
    as_put_key_data_map  put_key_data_map = {0};

    if (!(ht_p) || !(namespace_pp) || !(set_pp) || !(key_pp)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_transform_iterateKey(ht_p, NULL/*retdata_pp*/,
                                                                 &aerospike_transform_putkey_callback,
                                                                 (void *)&put_key_data_map, 0))) {
        goto exit;
    }

    if (!(put_key_data_map.namespace_p) || !(put_key_data_map.set_p) || !(put_key_data_map.key_pp)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    *namespace_pp = put_key_data_map.namespace_p;
    *set_pp = put_key_data_map.set_p;
    *key_pp = *(put_key_data_map.key_pp); /* need to check this part */
exit:
    return status;
}

typedef struct data_list_map {
    u_int32_t        current_list_idx_u32;
    as_arraylist     alloc_list[MAX_AS_LIST_SIZE];
    u_int32_t        current_map_idx_u32;
    as_hashmap       alloc_map[MAX_AS_MAP_SIZE];
} as_data_list_map_struct;

extern as_status
aerospike_add_key_params(as_key* as_key_p,
                         int8_t* namespace_p, int8_t* set_p, zval** record_pp)
{
    as_status      status = AEROSPIKE_OK;

    if ((!as_key_p) || (!set_p) || (!record_pp)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    switch(key_type) {
        case IS_LONG:
            as_key_init_int64(as_key_p, namespace_p, set_p, (int64_t) Z_LVAL_PP(record_pp));
            break;
        case IS_STRING:
            as_key_init_str(&key, namespace, set, (int8_t *) Z_STRVAL_PP(record_pp));
            break;
        default:
            status = AEROSPIKE_ERR;
            break;
    }

exit:
    return status;
}

#define GET_PRE_ALLOC_LIST(list_p, pre_stackalloc_data_p)                                           \
     if (MAX_AS_LIST_SIZE > pre_stackalloc_data_p->current_list_idx_u32) {                          \
         list_p = pre_stackalloc_data_p->alloc_list[pre_stackalloc_data_p->current_list_idx_u32];   \
     }
#define GET_PRE_ALLOC_MAP(map_p, pre_stackalloc_data_p)                                             \
     if (MAX_AS_LIST_SIZE > pre_stackalloc_data_p->current_map_idx_u32) {                           \
         map_p = pre_stackalloc_data_p->alloc_map[pre_stackalloc_data_p->current_map_idx_u32];      \
     }
#define AS_APPEND_INT(as_val, data_int) \
     if (AS_MAP == as_val_type(as_val)) { \
     } else if (AS_LIST == as_val_type(as_val)) { \
         as_arraylist_append_int64(list, Z_LVAL_PP(data)); \
     } else {\
         status = AEROSPIKE_ERR; \
         goto exit;     \
     }
#define AS_APPEND_STR(as_val, data_int) \
     if (AS_MAP == as_val_type(as_val)) { \
     } else if (AS_LIST == as_val_type(as_val)) { \
         as_arraylist_append_str(list, Z_STRVAL_PP(data)); \
     } else {\
         status = AEROSPIKE_ERR; \
         goto exit;     \
     }

#define AS_LIST_I

#define AS_INIT_DATA_STORE(as_val) \
     if (AS_MAP == as_val_type(as_val)) { \
         as_hashmap_init((as_hashmap *)as_val); \
     } else if (AS_LIST == as_val_type(as_val)) { \
         as_arraylist_init((as_arraylist *)as_val); \
     } else {\
         status = AEROSPIKE_ERR; \
         goto exit;     \
     }

#define AS_ASSIGN_DATA_STORE_TO_PARENT(record_p, val_p, data_store_type_p) \
     if (record_p) { \
         
static as_status
aerospike_transform_fill_as_map_list(HashTable *ht_p, as_data_list_map_struct* pre_stackalloc_data_p,
                                     as_record* record_p, as_val* val_p)
{
    as_status        status = AEROSPIKE_OK;
    HashPosition     hashPosition_p = NULL;
    u_int64_t        index_id_u64 = 0;
    //as_arraylist*    list_p = NULL;
    //as_hashmap*      hashmap_p = NULL;
    as_val*          data_store_type_p = NULL;
    zval**           data_val_pp = NULL;

    if ((!ht_p) || (!pre_stackalloc_data_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    /* check for map OR list as objects to be used */
    zend_hash_internal_pointer_reset_ex(ht_p, &pointer);
    if (HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(ht_p, &key_id_p, &key_id_len, &index_id_u64, 0, &hashPosition_p)) {
        /* map identified */
        GET_PRE_ALLOC_MAP(data_store_type_p, pre_stackalloc_data_p)
    } else {
        /* list identified */
        GET_PRE_ALLOC_LIST(data_store_type_p, pre_stackalloc_data_p)
    }

    if (!data_store_type_p) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    AS_INIT_DATA_STORE(data_store_type_p)
    AS_ASSIGN_DATA_STORE_TO_PARENT(record_p, val_p, data_store_type_p)

    foreach_hashtable(ht_p, hashPosition_p, data_val_pp) {
        switch(Z_TYPE_PP(data_val_pp)) {
            case IS_LONG:
                AS_APPEND_INT(data_store_type_p, Z_LVAL_PP(data_val_pp))
                break;
            case IS_STRING:
                AS_APPEND_STR(data_store_type_p, Z_STRVAL_PP(data_val_pp))
                break;
            case IS_ARRAY:
#if 0
                inner_arr_hash = Z_ARRVAL_PP(data);
                zend_hash_internal_pointer_reset_ex(inner_arr_hash, &inner_pointer);

                if (zend_hash_get_current_key_ex(inner_arr_hash, &inner_key, &inner_arrkey_len, &inner_index, 0, &inner_pointer) == HASH_KEY_IS_STRING) {
                    inner_map = as_hashmap_new(32);
                    as_map *inner_m = (as_map *) inner_map;
                    handle_put_map(data, inner_m);
                    as_arraylist_append_map(list, inner_m);
                } else {
                    inner_array_len = zend_hash_num_elements(inner_arr_hash);
                    inner_list = as_arraylist_new(inner_array_len, 0);
                    handle_put_list(data, inner_list);
                    as_arraylist_append_list(list, (as_list *) inner_list);
                }
#endif
                break;
            default:
                status = AEROSPIKE_ERR;
                break;
        }
    }

exit:
    return status;
}

static as_status
aerospike_transform_fill_asrecord(HashTable* ht_p, as_record* record_p, int8_t* bin_name_p, zval** bin_value_pp,
                                  as_data_list_map_struct*  pre_stackalloc_data_p)
{
    as_status         status = AEROSPIKE_OK;

    if ((!ht_p) || (!bin_value_pp) || (!bin_name_p) || (!record_p) || (!pre_stackalloc_data)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    switch(Z_TYPE_PP(bin_value_pp)) {
        case IS_NULL:
            as_record_set_nil(record_p, bin_name_p);
            break;
        case IS_LONG:
            as_record_set_int64(record_p, bin_name_p, (int64_t) Z_LVAL_PP(bin_value_pp));
            break;
        case IS_STRING:
            as_record_set_str(record_p, bin_name_p, (int8_t *) Z_STRVAL_PP(bin_value_pp));
            break;
        case IS_ARRAY:
            break;
        default:
            status = AEROSPIKE_ERR;
            break;
    }
}

static as_status
aerospike_transform_iterate_records(HashTable* ht_p, as_record* record_p, as_data_list_map_struct*  pre_stackalloc_data_p)
{
    as_status          status = AEROSPIKE_OK;
    HashPosition       hashPosition_p = NULL;
    zval**             record_pp = NULL;

    if ((!ht_p) || !(record_p) || !(pre_stackalloc_data)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    foreach_hashtable(ht_p, hashPosition_p, record_pp) {
        u_int32_t     bin_name_len_u32 = 0;
        int8_t*       bin_name_p = NULL;
        u_int64_t     index_u64 = 0;
        u_int32_t     key_type_u32 = zend_hash_get_current_key_ex(ht_p, &bin_name_p, &bin_name_len_u32,
                                                                  &index_u64, 0, &hashPosition_p);

        if (AEROSPIKE_OK != (status = aerospike_transform_fill_asrecord(ht_p, record_p, bin_name_p, 
                                                                        record_pp, pre_stackalloc_data_p))) {
            goto exit;
        }

    }

exit:
    return status;
}

extern as_status
aerospike_transform_key_data_put(HashTable* ht_p, as_record* record_p, as_key* as_key_p)
{
    as_status                   status = AEROSPIKE_OK;
    as_data_list_map_struct     list_map_data = {0};

    if ((!ht_p) || (!record_p) || (!as_key_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }


exit:
    return status;
}
