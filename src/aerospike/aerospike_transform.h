/*
 * For GET and PUT method.
 * Both methods may have LIST and MAP datatypes.
 * To iterate over it, we need to have callbacks.
 * To use below macros for code generation, callbacks
 * should have specific nomenclature.
 *
 * Following are the basic actions beased on caller.
 * callback for LIST => APPEND
 * callback for MAP => ASSOC
 * initial call for this macro => ASSOC
 *
 * Callback name has following specification:
 *      "AEROSPIKE_##level_##method_##action_##datatype"
 *
 *      level => DEFAULT, MAP, LIST
 *      method => GET, PUT
 *      action => ASSOC, APPEND
 *      datatype => Data types in GET and PUT switch cases
 *                  exclusing prefixes but complete names.
 *
 */

#ifndef __AEROSPIKE_TRANSFORM_H__
#define __AEROSPIKE_TRANSFORM_H__

#define PREFIX_GET AS
#define PREFIX_PUT IS

#define FETCH_VALUE_GET(val) as_val_type(val)
#define FETCH_VALUE_PUT(val) Z_TYPE_PP(val) // do we want to keep it Z_TYPE_P

#define EXPAND_CASE(level, method, action, datatype, key, value,        \
                            array, err, static_pool, label)             \
        case PREFIX_##method_##datatype:                                \
            if (AEROSPIKE_OK != (err.code =                             \
                        AEROSPIKE_##level_##method_##action_##datatype( \
                    key, value, array, static_pool))) {                 \
                goto label;                                             \
            }                                                           \
            break;



#define AEROSPIKE_WALKER_SWITCH_CASE(method, level,              \
        action, err, static_pool, key, value, array, label)             \
            AEROSPIKE_WALKER_SWITCH_CASE_##method(method,        \
                level, action, err, static_pool, key, value, array,     \
                    label)

/*
 * DEFAULT:
 * key => bin_name
 * value=>bin_value
 * array=>record
 * LIST:
 * key=>
 * value=>
 * array=>
 * MAP:
 * key=>
 * value=>
 * array=>
 */
/*value holds a zval* right now*/ 
#define AEROSPIKE_WALKER_SWITCH_CASE_PUT(method, level, action,  \
        err, static_pool, key, value, array, label)                            \
    switch (FETCH_VALUE_##method(value)) {                              \
        EXPAND_CASE(level, method, action, ARRAY, key, value,           \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, STRING, key, value,          \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, LONG, key, value,            \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, NULL, key, value,            \
                array, err, static_pool, label)                         \
        default:                                                        \
            err.code = AEROSPIKE_ERR_PARAM;                             \
            goto label;                                                 \
    }

/*note : INTEGER, it does not translate to IS_LONG ??? */
#define AEROSPIKE_WALKER_SWITCH_CASE_GET(value, method, level, action,  \
        err, static_pool, key, value, label)                            \
    switch (FETCH_VALUE_##method(value)) {                              \
        EXPAND_CASE(level, method, action, UNDEF, key, value,           \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, UNKNOWN, key, value,         \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, NIL, key, value,             \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, BOOLEAN, key, value,         \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, INTEGER, key, value,         \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, STRING, key, value,          \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, LIST, key, value,            \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, MAP, key, value,             \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, REC, key, value,             \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, PAIR, key, value,            \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, BYTES, key, value,           \
                array, err, static_pool, label)                         \
        default:                                                        \
            err.code = AEROSPIKE_ERR_PARAM;                             \
            goto label;                                                 \
    }

typedef struct data_list_map {
    u_int32_t        current_list_idx_u32;
    as_arraylist     alloc_list[MAX_AS_LIST_SIZE];
    u_int32_t        current_map_idx_u32;
    as_hashmap       alloc_map[MAX_AS_MAP_SIZE];
} as_data_list_map_struct;

#define MAX_AS_LIST_SIZE   100
#define MAX_AS_MAP_SIZE    MAX_AS_LIST_SIZE

#define AEROSPIKE_HASHMAP_BUCKET_SIZE     32
#define AEROSPIKE_ASLIST_BLOCK_SIZE       0

#define GET_PRE_ALLOC_LIST(stack_data_p, list_p, num_elem)                                                                                          \
     if (MAX_AS_LIST_SIZE > ((as_data_list_map_struct *)stack_data_p)->current_list_idx_u32) {                                                      \
         list_p = (void *)(((as_data_list_map_struct *)stack_data_p)->alloc_list[((as_data_list_map_struct *)stack_data_p)->current_list_idx_u32]); \
         list_p = as_arraylist_init((as_arraylist *)list_p, num_elem, AEROSPIKE_ASLIST_BLOCK_SIZE)                                                  \
         if (list_p)                                                                                                                                \
             ((as_data_list_map_struct *)stack_data_p)->current_list_idx_u32++;                                                                     \
     }

#define GET_PRE_ALLOC_MAP(stack_data_p, map_p, num_elem)                                                                                            \
     if (MAX_AS_MAP_SIZE > ((as_data_list_map_struct *)stack_data_p)->current_map_idx_u32) {                                                        \
         map_p = (void *)(((as_data_list_map_struct *)stack_data_p)->alloc_map[((as_data_list_map_struct *)stack_data_p)->current_map_idx_u32]);    \
         map_p = (void *)(as_hashmap_init((as_hashmap *)map_p, AEROSPIKE_HASHMAP_BUCKET_SIZE));                                                     \
         if (map_p)                                                                                                                                 \
             ((as_data_list_map_struct *)stack_data_p)->current_map_idx_u32++;                                                                      \
     }

#define IS_MAP(hash, key, key_len, index, pointer)                      \
    (zend_hash_get_current_key_ex(hash, &(key), &(key_len), &(index),   \
            0, &(pointer)) == HASH_KEY_IS_STRING)

#define IS_LIST(hash, key, key_len, index, pointer)                     \
    (zend_hash_get_current_key_ex(hash, &(key), &(key_len), &(index),   \
            0, &(pointer)) != HASH_KEY_IS_STRING)

#define INIT_LIST(static_pool, inner_store, size) GET_PRE_ALLOC_LIST(static_pool, inner_store, size)   
#define INIT_MAP(static_pool, inner_store, size)  GET_PRE_ALLOC_MAP(static_pool, inner_store, 0)   

/*level => where are you at present */
#define AEROSPIKE_PROCESS_ARRAY(datatype, level, action, label, hash, key, key_len, index, pointer, outer_key, value, store, static_pool, num_elem) \
    if (IS_##datatype(hash, key, key_len, index, pointer)) {                                                                                        \
        void  *inner_store = NULL;                                                                                                                  \
        INIT_##datatype(static_pool, inner_store, num_elem);                                                                                        \
        if ((!inner_store) || (AEROSPIKE_OK != (error_code =                                                                                        \
                    AEROSPIKE_##level_PUT_##action_##datatype(key, value, inner_store, static_pool)))) {                                            \
            goto label;                                                                                                                             \
        }                                                                                                                                           \
        AEROSPIKE_##level_SET_##action_##datatype(store, inner_store, outer_key)                                                                    \
    }

#define AEROSPIKE_ITERATE_RECORDS_WITHIN(array, as_holder, level, action, status, static_pool)                               \
{                                                                                                                            \
    HashPosition   hashPosition_p = NULL;                                                                                    \
    zval**         record_pp = NULL;                                                                                         \
    foreach_hashtable((HashTable *)array, hashPosition_p, record_pp) {                                                       \
        AEROSPIKE_WALKER_SWITCH_CASE_PUT(PUT, level, action, status, alloc_pool, NULL, Z_TYPE_P(record_pp), as_holder, exit) \
    }                                                                                                                        \
}

#define AEROSPIKE_RESET_AND_PROCESS_ISARRAY(key_p, value_p, hash_table_p, array_p, static_pool_p, level, action)             \
{                                                                                                                            \
    HashTable*             inner_ht_p = (HashTable *)(hash_table);                                                           \
    HashPosition           hashPosition_p;                                                                                   \
    int8_t*                inner_key_p;                                                                                      \
    int32_t                inner_key_len_32;                                                                                 \
    u_int64_t              inner_idx_u64;                                                                                    \
    u_int32_t              num_elem = 0;                                                                                     \
                                                                                                                             \
    zend_hash_internal_pointer_reset_ex(inner_ht_p, &hashPosition_p);                                                        \
    num_elem = zend_hash_num_elements(inner_ht_p);                                                                           \
    AEROSPIKE_PROCESS_ARRAY(MAP, level, action, exit, inner_ht_p, inner_key_p, inner_key_len_32, inner_idx_u64,              \
                            hashPosition_p, key_p, value_p, array_p, static_pool, num_elem)                                  \
    AEROSPIKE_PROCESS_ARRAY(LIST, level, action, exit, inner_ht_p, inner_key_p, inner_key_len_32, inner_idx_u64,             \
                            hashPosition_p, key_p, value_p, array_p, static_pool, num_elem)                                  \
}


#define AEROSPIKE_LIST_PUT_APPEND_NULL(key, value, array, static_pool)       AS_SET_ERROR_CASE(key, value, array, static_pool) 
#define AEROSPIKE_LIST_PUT_APPEND_LONG(key, value, array, static_pool)       AS_ARRAYLIST_APPEND_INT64(key, value, array, static_pool) 
#define AEROSPIKE_LIST_PUT_APPEND_STRING(key, value, array, static_pool)     AS_ARRAYLIST_APPEND_STR(key, value, array, static_pool)
#define AEROSPIKE_LIST_PUT_APPEND_ARRAY(key, value, array, static_pool)      AS_LIST_PUT_APPEND_ARRAY(key, value, array, static_pool) 
#define AEROSPIKE_LIST_PUT_APPEND_LIST(key, value, array, static_pool)       AS_ARRAYLIST_APPEND_LIST(key, value, array, static_pool) 
#define AEROSPIKE_LIST_PUT_APPEND_MAP(key, value, array, static_pool)        AS_ARRAYLIST_APPEND_MAP(key, value, array, static_pool)
#define AEROSPIKE_LIST_SET_APPEND_LIST(store, data_to_be_added, bin_name)    AS_ARRAYLIST_SET_APPEND_LIST(store, data_to_be_added, bin_name)
#define AEROSPIKE_LIST_SET_APPEND_MAP(store, data_to_be_added, bin_name)     AS_ARRAYLIST_SET_APPEND_MAP(store, data_to_be_added, bin_name)

#define AEROSPIKE_DEFAULT_PUT_ASSOC_NULL(key, value, array, static_pool)     AS_RECORD_SET_NIL(key, value, array, static_pool) 
#define AEROSPIKE_DEFAULT_PUT_ASSOC_LONG(key, value, array, static_pool)     AS_RECORD_SET_INT64(key, value, array, static_pool)
#define AEROSPIKE_DEFAULT_PUT_ASSOC_STRING(key, value, array, static_pool)   AS_RECORD_SET_STR(key, value, array, static_pool)
#define AEROSPIKE_DEFAULT_PUT_ASSOC_ARRAY(key, value, array, static_pool)    AS_DEFAULT_PUT_ASSOC_ARRAY(key, value, array, static_pool)
#define AEROSPIKE_DEFAULT_PUT_ASSOC_LIST(key, value, array, static_pool)     AS_DEFAULT_PUT_ASSOC_LIST(key, value, array, static_pool)
#define AEROSPIKE_DEFAULT_PUT_ASSOC_MAP(key, value, array, static_pool)      AS_DEFAULT_PUT_ASSOC_MAP(key, value, array, static_pool)
#define AEROSPIKE_DEFAULT_SET_ASSOC_LIST(store, data_to_be_added, bin_name)  AS_DEFAULT_SET_ASSOC_LIST(store, data_to_be_added, bin_name)
#define AEROSPIKE_DEFAULT_SET_ASSOC_MAP(store, data_to_be_added, bin_name)   AS_DEFAULT_SET_ASSOC_MAP(store, data_to_be_added, bin_name)

#define AEROSPIKE_MAP_PUT_ASSOC_NULL(key, value, array, static_pool)         AS_SET_ERROR_CASE() 
#define AEROSPIKE_MAP_PUT_ASSOC_LONG(key, value, array, static_pool)         AS_STRINGMAP_SET_INT64(key, value, array, static_pool)
#define AEROSPIKE_MAP_PUT_ASSOC_STRING(key, value, array, static_pool)       AS_STRINGMAP_SET_STR(key, value, array, static_pool)
#define AEROSPIKE_MAP_PUT_ASSOC_ARRAY(key, value, array, static_pool)        AS_MAP_PUT_ASSOC_ARRAY(key, value, array, static_pool)
#define AEROSPIKE_MAP_PUT_ASSOC_LIST(key, value, array, static_pool)         AS_STRINGMAP_SET_LIST(key, value, array, static_pool) 
#define AEROSPIKE_MAP_PUT_ASSOC_MAP(key, value, array, static_pool)          AS_STRINGMAP_SET_MAP(key, value, array, static_pool) 
#define AEROSPIKE_MAP_SET_ASSOC_LIST(store, data_to_be_added, bin_name)      AS_STRINGMAP_SET_ASSOC_LIST(store, data_to_be_added, bin_name)
#define AEROSPIKE_MAP_SET_ASSOC_MAP(store, data_to_be_added, bin_name)       AS_STRINGMAP_SET_ASSOC_MAP(store, data_to_be_added, bin_name)

#define AEROSPIKE_LIST_GET_APPEND_UNDEF(key, value, array, static_pool)    ADD_APPEND_NULL(key, value, array, static_pool) 
#define AEROSPIKE_LIST_GET_APPEND_UNKNOWN(key, value, array, static_pool)  ADD_APPEND_NULL(key, value, array, static_pool) 
#define AEROSPIKE_LIST_GET_APPEND_NIL(key, value, array, static_pool)      ADD_APPEND_NULL(key, value, array, static_pool) 
#define AEROSPIKE_LIST_GET_APPEND_BOOLEAN(key, value, array, static_pool)  ADD_APPEND_BOOL(key, value, array, static_pool) 
#define AEROSPIKE_LIST_GET_APPEND_INTEGER(key, value, array, static_pool)  ADD_APPEND_LONG(key, value, array, static_pool) 
#define AEROSPIKE_LIST_GET_APPEND_STRING(key, value, array, static_pool)   ADD_APPEND_STRING(key, value, array, static_pool)
#define AEROSPIKE_LIST_GET_APPEND_LIST(key, value, array, static_pool)     ADD_APPEND_LIST(key, value, array, static_pool) 
#define AEROSPIKE_LIST_GET_APPEND_MAP(key, value, array, static_pool)      ADD_APPEND_MAP(key, value, array, static_pool) 
#define AEROSPIKE_LIST_GET_APPEND_REC(key, value, array, static_pool)      ADD_APPEND_REC(key, value, array, static_pool) 
#define AEROSPIKE_LIST_GET_APPEND_PAIR(key, value, array, static_pool)     ADD_APPEND_PAIR(key, value, array, static_pool) 
#define AEROSPIKE_LIST_GET_APPEND_BYTES(key, value, array, static_pool)    ADD_APPEND_BYTES(key, value, array, static_pool) 

#define AEROSPIKE_DEFAULT_GET_ASSOC_UNDEF(key, value, array, static_pool)   ADD_ASSOC_NULL(key, value, array, static_pool) 
#define AEROSPIKE_DEFAULT_GET_ASSOC_UNKNOWN(key, value, array, static_pool) ADD_ASSOC_NULL(key, value, array, static_pool) 
#define AEROSPIKE_DEFAULT_GET_ASSOC_NIL(key, value, array, static_pool)     ADD_ASSOC_NULL(key, value, array, static_pool) 
#define AEROSPIKE_DEFAULT_GET_ASSOC_BOOLEAN(key, value, array, static_pool) ADD_ASSOC_BOOL(key, value, array, static_pool) 
#define AEROSPIKE_DEFAULT_GET_ASSOC_INTEGER(key, value, array, static_pool) ADD_ASSOC_LONG(key, value, array, static_pool) 
#define AEROSPIKE_DEFAULT_GET_ASSOC_STRING(key, value, array, static_pool)  ADD_ASSOC_STRING(key, value, array, static_pool) 
#define AEROSPIKE_DEFAULT_GET_ASSOC_LIST(key, value, array, static_pool)    ADD_ASSOC_LIST(key, value, array, static_pool) 
#define AEROSPIKE_DEFAULT_GET_ASSOC_MAP(key, value, array, static_pool)     ADD_ASSOC_MAP(key, value, array, static_pool) 
#define AEROSPIKE_DEFAULT_GET_ASSOC_REC(key, value, array, static_pool)     ADD_ASSOC_REC(key, value, array, static_pool) 
#define AEROSPIKE_DEFAULT_GET_ASSOC_PAIR(key, value, array, static_pool)    ADD_ASSOC_PAIR(key, value, array, static_pool) 
#define AEROSPIKE_DEFAULT_GET_ASSOC_BYTES(key, value, array, static_pool)   ADD_ASSOC_BYTES(key, value, array, static_pool) 

static inline as_status AS_DEFAULT_GET(void *key, void *value, *array)
{
    as_status status = AEROSPIKE_OK;
    AEROSPIKE_WALKER_SWITCH_CASE(GET, DEFAULT, ASSOC, status, static_pool, key, value, array, exit);
exit:
    return (status);
}
static inline as_status AS_LIST_GET(void *key, void *value, *array)
{
    as_status status = AEROSPIKE_OK;
    AEROSPIKE_WALKER_SWITCH_CASE(GET, LIST, APPEND, status, static_pool, key, value, array, exit);
exit:
    return (status);
}
static inline as_status AS_MAP_GET(void *key, void *value, *array)
{
    as_status status = AEROSPIKE_OK;
    AEROSPIKE_WALKER_SWITCH_CASE(GET, MAP, ASSOC, status, static_pool, key, value, array, exit);
exit:
    return (status);
}

#define AS_MAP_FOREACH as_map_foreach
#define AS_LIST_FOREACH as_list_foreach

#define AS_MAP_FOREACH_CALLBACK as_map_foreach_callback
#define AS_LIST_FOREACH_CALLBACK as_list_foreach_callback

#define ADD_ASSOC_ZVAL(array, key, store) add_assoc_zval(array, as_string_get((as_string *) key), store)
#define ADD_APPEND_ZVAL(array, key, store) add_next_index_zval(array, store)


#define AS_STORE_ITERATE(method, level, action, datatype, key, value, array)                                \
    zval *store;                                                                                            \
    MAKE_STD_ZVAL(store);                                                                                   \
    array_init(store);                                                                                      \
    AS_##datatype_FOREACH((AS_##method*) value,                                                             \
            (AS_##datatype_FOREACH_CALLBACK) AS_##level_##method, &store);                                  \
    ADD_##action_ZVAL(array, key, store);

static inline as_status ADD_APPEND_NULL(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_next_index_null(((zval*)array));
    return (status);
}

static inline as_status ADD_APPEND_BOOL(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_next_index_bool(((zval*)array),
            (int) as_boolean_get((as_boolean *) value));
    return (status);
}

static inline as_status ADD_APPEND_LONG(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_next_index_bool(((zval*)array),
            (long) as_integer_get((as_integer *) value));
    return (status);
}

static inline as_status ADD_APPEND_STRING(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_next_index_stringl(((zval*)array),
            as_string_get((as_string *) value),
            strlen(as_string_get((as_string *) value)), 1);
    return (status);
}

static inline as_status ADD_APPEND_MAP(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    AS_STORE_ITERATE(GET, LIST, APPEND, MAP, key, value, array);
    return (status);
}

static inline as_status ADD_APPEND_LIST(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    AS_STORE_ITERATE(GET, LIST, APPEND, LIST, key, value, array);
    return (status);
}

static inline as_status ADD_APPEND_REC(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;

    return (status);
}

static inline as_status ADD_APPEND_PAIR(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;

    return (status);
}

static inline as_status ADD_APPEND_BYTES(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;

    return (status);
}

static inline as_status ADD_ASSOC_NULL(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_assoc_null(((zval*)array), as_string_get((as_string *) key));
    return (status);
}

static inline as_status ADD_ASSOC_BOOL(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_assoc_bool(((zval*)array), as_string_get((as_string *) key),
            (int) as_boolean_get((as_boolean *) value));
    return (status);
}

static inline as_status ADD_ASSOC_LONG(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_assoc_long(((zval*)array), as_string_get((as_string *) key),
            (long) as_integer_get((as_integer *) value));
    return (status);
}

static inline as_status ADD_ASSOC_STRING(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    add_assoc_stringl(((zval*)array), as_string_get((as_string *) key),
            as_string_get((as_string *) value),
            strlen(as_string_get((as_string *) value)), 1);
    return (status);
}

static inline as_status ADD_ASSOC_MAP(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    AS_STORE_ITERATE(GET, DEFAULT, ASSOC, MAP, key, value, array);
    return (status);
}

static inline as_status ADD_ASSOC_LIST(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;
    AS_STORE_ITERATE(GET, DEFAULT, ASSOC, LIST, key, value, array);
    return (status);
}

static inline as_status ADD_ASSOC_REC(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;

    return (status);
}

static inline as_status ADD_ASSOC_PAIR(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;

    return (status);
}

static inline as_status ADD_ASSOC_BYTES(void *key, void *value, void *array)
{
    as_status status = AEROSPIKE_OK;

    return (status);
}


#define AEROSPIKE_MAP_GET_ASSOC_UNDEF(key, value, array, static_pool)      AEROSPIKE_DEFAULT_GET_ASSOC_UNDEF(key, value, array, static_pool)   
#define AEROSPIKE_MAP_GET_ASSOC_UNKNOWN(key, value, array, static_pool)    AEROSPIKE_DEFAULT_GET_ASSOC_UNKNOWN(key, value, array, static_pool)   
#define AEROSPIKE_MAP_GET_ASSOC_NIL(key, value, array, static_pool)        AEROSPIKE_DEFAULT_GET_ASSOC_NIL(key, value, array, static_pool)  
#define AEROSPIKE_MAP_GET_ASSOC_BOOLEAN(key, value, array, static_pool)    AEROSPIKE_DEFAULT_GET_ASSOC_BOOLEAN(key, value, array, static_pool)
#define AEROSPIKE_MAP_GET_ASSOC_INTEGER(key, value, array, static_pool)    AEROSPIKE_DEFAULT_GET_ASSOC_INTEGER(key, value, array, static_pool)
#define AEROSPIKE_MAP_GET_ASSOC_STRING(key, value, array, static_pool)     AEROSPIKE_DEFAULT_GET_ASSOC_STRING(key, value, array, static_pool) 
#define AEROSPIKE_MAP_GET_ASSOC_LIST(key, value, array, static_pool)       AEROSPIKE_DEFAULT_GET_ASSOC_LIST(key, value, array, static_pool)  
#define AEROSPIKE_MAP_GET_ASSOC_MAP(key, value, array, static_pool)        AEROSPIKE_DEFAULT_GET_ASSOC_MAP(key, value, array, static_pool)  
#define AEROSPIKE_MAP_GET_ASSOC_REC(key, value, array, static_pool)        AEROSPIKE_DEFAULT_GET_ASSOC_REC(key, value, array, static_pool)  
#define AEROSPIKE_MAP_GET_ASSOC_PAIR(key, value, array, static_pool)       AEROSPIKE_DEFAULT_GET_ASSOC_PAIR(key, value, array, static_pool)  
#define AEROSPIKE_MAP_GET_ASSOC_BYTES(key, value, array, static_pool)      AEROSPIKE_DEFAULT_GET_ASSOC_BYTES(key, value, array, static_pool)

static inline as_status AS_SET_ERROR_CASE(void* key_p, void* value_p, void* array_p, void* static_pool)
{
}

static inline as_status AS_ARRAYLIST_APPEND_INT64(void* key_p, value, array, static_pool)
{
    /*
     * key_p - ignore 
     * value - value to be stored in list
     * array - as_val to be typecased into list  
     */
     as_arraylist_append_int64((as_arraylist *)list, (int_64)Z_LVAL_P(data));
     return AEROSPIKE_OK;
}
 
static inline as_status AS_ARRAYLIST_APPEND_STR(key, value, array, static_pool)
{
    /*
     * key - ignore
     * value - value to be stored in list
     * array - as_val to be typecasted into list
     */
     as_arraylist_append_str((as_arraylist *)list, Z_STRVAL_P(data));
}

static inline void AS_ARRAYLIST_SET_APPEND_LIST(void* store_p, void* data_to_be_added_p, void* bin_name)
{
    as_arraylist_append_list((as_arraylist *)store_p, (as_list*) data_to_be_added_p);
}


static inline void AS_ARRAYLIST_SET_APPEND_MAP(void* store_p, void* data_to_be_added_p, void* bin_name)
{
    as_arraylist_append_map((as_arraylist *)store_p, (as_map*) data_to_be_added_p);
}

static inline as_status AS_LIST_PUT_APPEND_ARRAY(key, value, array, static_pool)
{
    /*
     * key_p   - holds the name of the bin
     * value_p - holds the data 
     * array_p - holds as_list  
     */
    as_status     status = AEROSPIKE_OK;
    AEROSPIKE_RESET_AND_PROCESS_ISARRAY(key, value, Z_ARRVAL_PP(value), array, static_pool_p, LIST, APPEND);
exit:
    return status;

}

static inline as_status AS_ARRAYLIST_APPEND_LIST(key, value, array, static_pool)
{
     /*
      * key -> key not to be looked in this case
      * value-> holds the data to be parsed for list
      * array->as_list which needs to be populated
      */
     as_status    status = AEROSPIKE_OK;
     AEROSPIKE_ITERATE_RECORDS_WITHIN(value, array, LIST, APPEND, status, static_pool)
exit:
    return status;
}
 
static inline as_status AS_ARRAYLIST_APPEND_MAP(key, value, array, static_pool) 
{
     as_status    status = AEROSPIKE_OK;
     AEROSPIKE_ITERATE_RECORDS_WITHIN(value, array, MAP, APPEND, status, static_pool)
exit:
     return status;
}

static inline as_status AS_RECORD_SET_NIL(void* key_p, void* value_p, void* array_p, void* static_pool_p)
{
    /* value holds the name of the bin*/
    as_record_set_nil((as_record *)(key_p), (int8_t *)value_p);
    return AEROSPIKE_OK;
}

static inline as_status AS_RECORD_SET_INT64(void* key_p, void* value_p, void* array_p, void* static_pool_p)
{
    /*
     * key_p - holds the name of the bin
     * value_p - holds the data associated with bin
     * array_p - holds as_record pointer 
    */
    as_record_set_int64((as_record *)array_p, (in8_t *)key_p, (int64_t) Z_LVAL_P(value_p)); //changed from Z_VAL_PP to Z_VAL_P
    return AEROSPIKE_OK;
}

static inline as_status AS_RECORD_SET_STR(key, value, array, static_pool)
{
    /*
     * key_p - holds the name of the bin
     * value_p - holds the data associated with bin
     * array_p - holds as_record pointer 
    */
    as_record_set_str((as_record *)array_p, (int_8 *)key_p, (char *) Z_STRVAL_P(value_p));
    return AEROSPIKE_OK;;
}

   
static inline as_status AS_DEFAULT_PUT_ASSOC_ARRAY(void* key_p, void* value_p, void* array_p, void* static_pool_p)
{
    /*
     * key_p - holds the name of the bin
     * value_p - holds the data associated with bin
     * array_p - holds as_record pointer 
    */
    as_status     status = AEROSPIKE_OK;
/* note: is this right ??*/
    AEROSPIKE_RESET_AND_PROCESS_ISARRAY(key_p, value_p, Z_ARRVAL_PP(value_p), Z_VAL_PP(array_p), static_pool_p, DEFAULT, ASSOC)
exit:
    return status;
}


static inline void AS_DEFAULT_SET_ASSOC_LIST(void* store_p, void* data_to_be_added_p, void* bin_name)
{
    as_record_set_list((as_record *)store_p, (int8_t*)bin_name, (as_list *) data_to_be_added);
}

static inline void AS_DEFAULT_SET_ASSOC_MAP(void* store_p, void* data_to_be_added_p, void* bin_name)
{
    as_record_set_map((as_record *)store_p, (int8_t*)bin_name, (as_map *) data_to_be_added);
}


static inline as_status AS_DEFAULT_PUT_ASSOC_LIST(key, value, array, static_pool)
{
    /*
     * key -> key not to be looked in this case
     * value-> holds the data to be parsed for list
     * array->as_list which needs to be populated
     */
     as_status    status = AEROSPIKE_OK;
     AEROSPIKE_ITERATE_RECORDS_WITHIN(value, array, LIST, APPEND, status, static_pool)
exit:
    return status; 
}

static inline void AS_STRINGMAP_SET_ASSOC_LIST(void* store_p, void* data_to_be_added_p, void* bin_name)
{
    as_stringmap_set_list((as_hashmap *)store_p, key, (as_list *) inner_list);
}

static inline void AS_STRINGMAP_SET_ASSOC_MAP(void* store_p, void* data_to_be_added_p, void* bin_name_p)
{
    as_stringmap_set_map((as_hashmap *)store, key, (as_map *)data_to_be_added);
}

static inline as_status AS_DEFAULT_PUT_ASSOC_MAP(key, value, array, static_pool)
{
    /*
     * key -> key not to be looked in this case
     * value-> holds the data to be parsed for map 
     * array->as_map which needs to be populated
     */

     as_status    status = AEROSPIKE_OK;
     /* need to check this*/
     AEROSPIKE_ITERATE_RECORDS_WITHIN(value, array, MAP, ASSOC, status, static_pool)
exit:
    return status;
}

static inline as_status AS_STRINGMAP_SET_INT64(key, value, array, static_pool)
{
/* need inputs */
    return AEROSPIKE_ERR;
}

static inline as_status AS_STRINGMAP_SET_STR(key, value, array, static_pool)
{
/* need inputs */
    return AEROSPIKE_ERR;
}

static inline as_status AS_MAP_PUT_ASSOC_ARRAY(key, value, array, static_pool)
{
 /*ZARRVAL_P*/
    /*
     * key_p   - holds the name of the bin
     * value_p - holds the data 
     * array_p - holds array  
     */
    as_status     status = AEROSPIKE_OK;
    AEROSPIKE_RESET_AND_PROCESS_ISARRAY(key, value, Z_ARRVAL_PP(value), array, static_pool_p, MAP, ASSOC)
exit:
    return status;


}

static inline as_status AS_STRINGMAP_SET_LIST(key, value, array, static_pool)
{
     /*
      * key -> key not to be looked in this case
      * value-> holds the data to be parsed for list
      * array->as_list which needs to be populated
      */
     as_status    status = AEROSPIKE_OK;
     AEROSPIKE_ITERATE_RECORDS_WITHIN(value, array, LIST, ASSOC, status, static_pool)
exit:
     return status;
}

static inline as_status AS_STRINGMAP_SET_MAP(key, value, array, static_pool)
{
     as_status    status = AEROSPIKE_OK;
     AEROSPIKE_ITERATE_RECORDS_WITHIN(value, array, MAP, ASSOC, status, static_pool)
exit:
     return status;
}
#endif /* end of __AERROSPIKE_TRANSFORM_H__ */
