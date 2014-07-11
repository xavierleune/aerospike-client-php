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

#define AEROSPIKE_WALKER_SWITCH_CASE_GET(value, method, level, action,  \
        err, static_pool, key, value, label)                            \
    switch (FETCH_VALUE_##method(value)) {                              \
        EXPAND_CASE(level, method, action, UNDEF, key, value,           \
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


#define IS_MAP(hash, key, key_len, index, pointer)                      \
    (zend_hash_get_current_key_ex(hash, &(key), &(key_len), &(index),   \
            0, &(pointer)) == HASH_KEY_IS_STRING)

#define IS_LIST(hash, ket, key_len, index, pointer)                     \
    (zend_hash_get_current_key_ex(hash, &(key), &(key_len), &(index),   \
            0, &(pointer)) != HASH_KEY_IS_STRING)

/*level => where are you at present */
#define AEROSPIKE_PROCESS_ARRAY(datatype, level, action, label, hash, key, key_len, index, pointer, value, store, static_pool)                      \
    if (IS_##datatype(hash, key, key_len, index, pointer)) {                   \
        as_val *inner_store = INIT_##datatype(); \
        if (AEROSPIKE_OK != (error_code =  \
                    AEROSPIKE_##level_PUT_##action_##datatype(key, value, inner_store, static_pool))) { \
            goto label;\
        }\
    }

static inline AS_LIST_PUT_APPEND_ARRAY(void *key_p, void *value_p, void *container_p, void *static_pool_p)
{
    HashTable*             inner_ht_p = (HashTable *)(key_p);
    HashPosition           hashPosition_p;
    int8_t*                inner_key_p;
    int32_t                inner_key_len_32;
    u_int64_t              inner_idx_u64;
    as_status              error_code = AEROSPIKE_OK;

    zend_hash_internal_pointer_reset_ex(inner_ht_p, &hashPosition_p);
    AEROSPIKE_PROCESS_ARRAY(MAP, LIST, APPEND, exit, inner_ht_p, inner_key_p, inner_key_len_32, inner_idx_u64, hashPosition_p, value_p, static_pool)
    AEROSPIKE_PROCESS_ARRAY(LIST, LIST, APPEND, exit, inner_ht_p, inner_key_p, inner_key_len_32, inner_idx_u64, hashPosition_p, value_p, static_pool)
#if 0
    if (IS_MAP(inner_ht_p, inner_key_p, inner_key_len_32, inner_idx_u34, hashPosition_p)) {
        as_map *inner_map = INIT_MAP();
        if (AEROSPIKE_OK != (error_code =
                    AEROSPIKE_LIST_PUT_APPEND_MAP(inner_key_p, value, inner_map, static_pool_p))) {
            goto failure;
        }
    }
    if (IS_LIST(inner_hash, inner_key, inner_key_len, inner_index,
                inner_pointer)) {
        as_arraylist *inner_list = INIT_LIST();
        if (AEROSPIKE_OK != (error_code =
                    AEROSPIKE_LIST_PUT_APPEND_LIST(key, value,
                        inner_list, static_pool))) {
            goto failure;
        }
    }
#endif
failure:
    return (error_code);
}

    


#define AEROSPIKE_LIST_PUT_APPEND_NULL(key, value, array, static_pool)       AS_SET_ERROR_CASE(key, value, array, static_pool) 
#define AEROSPIKE_LIST_PUT_APPEND_LONG(key, value, array, static_pool)       AS_ARRAYLIST_APPEND_INT64(key, value, array, static_pool) 
#define AEROSPIKE_LIST_PUT_APPEND_STRING(key, value, array, static_pool)     AS_ARRAYLIST_APPEND_STR(key, value, array, static_pool)
#define AEROSPIKE_LIST_PUT_APPEND_ARRAY(key, value, array, static_pool)      AS_LIST_PUT_APPEND_ARRAY(key, value, array, static_pool) 
#define AEROSPIKE_LIST_PUT_APPEND_LIST(key, value, array, static_pool)       AS_ARRAYLIST_APPEND_LIST(key, value, array, static_pool) 
#define AEROSPIKE_LIST_PUT_APPEND_MAP(key, value, array, static_pool)        AS_ARRAYLIST_APPEND_MAP(key, value, array, static_pool) 

#define AEROSPIKE_DEFAULT_PUT_ASSOC_NULL(key, value, array, static_pool)     AS_RECORD_SET_NIL(key, value, array, static_pool) 
#define AEROSPIKE_DEFAULT_PUT_ASSOC_LONG(key, value, array, static_pool)     AS_RECORD_SET_INT64(key, value, array, static_pool)
#define AEROSPIKE_DEFAULT_PUT_ASSOC_STRING(key, value, array, static_pool)   AS_RECORD_SET_STR(key, value, array, static_pool)
#define AEROSPIKE_DEFAULT_PUT_ASSOC_ARRAY(key, value, array, static_pool)    AS_DEFAULT_PUT_ASSOC_ARRAY(key, value, array, static_pool)
#define AEROSPIKE_DEFAULT_PUT_ASSOC_LIST(key, value, array, static_pool)     AS_DEFAULT_PUT_ASSOC_LIST(key, value, array, static_pool)
#define AEROSPIKE_DEFAULT_PUT_ASSOC_MAP(key, value, array, static_pool)      AS_DEFAULT_PUT_ASSOC_MAP(key, value, array, static_pool)

#define AEROSPIKE_MAP_PUT_ASSOC_NULL(key, value, array, static_pool)         AS_SET_ERROR_CASE() 
#define AEROSPIKE_MAP_PUT_ASSOC_LONG(key, value, array, static_pool)         AS_STRINGMAP_SET_INT64(key, value, array, static_pool)
#define AEROSPIKE_MAP_PUT_ASSOC_STRING(key, value, array, static_pool)       AS_STRINGMAP_SET_STR(key, value, array, static_pool)
#define AEROSPIKE_MAP_PUT_ASSOC_ARRAY(key, value, array, static_pool)        AS_MAP_PUT_ASSOC_ARRAY(key, value, array, static_pool)
#define AEROSPIKE_MAP_PUT_ASSOC_LIST(key, value, array, static_pool)         AS_STRINGMAP_SET_LIST(key, value, array, static_pool) 
#define AEROSPIKE_MAP_PUT_ASSOC_MAP(key, value, array, static_pool)          AS_STRINGMAP_SET_MAP(key, value, array, static_pool) 

#define AEROSPIKE_LIST_GET_APPEND_UNDEF(key, value, array, static_pool)  
#define AEROSPIKE_LIST_GET_APPEND_NIL(key, value, array, static_pool)  
#define AEROSPIKE_LIST_GET_APPEND_BOOLEAN(key, value, array, static_pool)  
#define AEROSPIKE_LIST_GET_APPEND_INTEGER(key, value, array, static_pool)  
#define AEROSPIKE_LIST_GET_APPEND_STRING(key, value, array, static_pool)  
#define AEROSPIKE_LIST_GET_APPEND_LIST(key, value, array, static_pool)  
#define AEROSPIKE_LIST_GET_APPEND_MAP(key, value, array, static_pool)  
#define AEROSPIKE_LIST_GET_APPEND_REC(key, value, array, static_pool)  
#define AEROSPIKE_LIST_GET_APPEND_PAIR(key, value, array, static_pool)  
#define AEROSPIKE_LIST_GET_APPEND_BYTES(key, value, array, static_pool) 

#define AEROSPIKE_DEFAULT_GET_ASSOC_UNDEF(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_ASSOC_NIL(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_ASSOC_BOOLEAN(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_ASSOC_INTEGER(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_ASSOC_STRING(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_ASSOC_LIST(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_ASSOC_MAP(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_ASSOC_REC(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_ASSOC_PAIR(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_ASSOC_BYTES(key, value, array, static_pool) 

#define AEROSPIKE_MAP_GET_ASSOC_UNDEF(key, value, array, static_pool)      AEROSPIKE_DEFAULT_GET_ASSOC_UNDEF(key, value, array, static_pool)   
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
}
 
static inline as_status AS_ARRAYLIST_APPEND_STR(key, value, array, static_pool)
{
}

static inline as_status AS_LIST_PUT_APPEND_ARRAY(key, value, array, static_pool)
{
}

static inline as_status AS_ARRAYLIST_APPEND_LIST(key, value, array, static_pool)
{
}
 
static inline as_status AS_ARRAYLIST_APPEND_MAP(key, value, array, static_pool) 
{
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

#define AEROSPIKE_RESET_KEY_PROCESS_ISARRAY(key, value_p, array_p, static_pool_p, level, action)\
{\
    HashTable*             inner_ht_p = (HashTable *)(array_p);\
    HashPosition           hashPosition_p;\
    int8_t*                inner_key_p;\
    int32_t                inner_key_len_32;\
    u_int64_t              inner_idx_u64;\
\
    zend_hash_internal_pointer_reset_ex(inner_ht_p, &hashPosition_p); \
    AEROSPIKE_PROCESS_ARRAY(MAP, level, action, exit, inner_ht_p, inner_key_p, inner_key_len_32, inner_idx_u64, hashPosition_p, value_p, static_pool)\
    AEROSPIKE_PROCESS_ARRAY(LIST, level, action, exit, inner_ht_p, inner_key_p, inner_key_len_32, inner_idx_u64, hashPosition_p, value_p, static_pool)\
}
   
static inline as_status AS_DEFAULT_PUT_ASSOC_ARRAY(void* key_p, void* value_p, void* array_p, void* static_pool_p)
{
    /*
     * key_p - holds the name of the bin
     * value_p - holds the data associated with bin
     * array_p - holds as_record pointer 
    */
    as_status     status = AEROSPIKE_OK;
    AEROSPIKE_RESET_KEY_PROCESS_ISARRAY(key_p, value_p, array_p, static_pool_p, DEFAULT, ASSOC);
exit:
    return status;
}


static inline as_status AS_DEFAULT_PUT_ASSOC_LIST(key, value, array, static_pool)
{
}

static inline as_status AS_DEFAULT_PUT_ASSOC_MAP(key, value, array, static_pool)
{
}

static inline as_status AS_STRINGMAP_SET_INT64(key, value, array, static_pool)
{
}

static inline as_status AS_STRINGMAP_SET_STR(key, value, array, static_pool)
{
}

static inline as_status AS_MAP_PUT_ASSOC_ARRAY(key, value, array, static_pool)
{
}

static inline as_status AS_STRINGMAP_SET_LIST(key, value, array, static_pool)
{
}

static inline as_status AS_STRINGMAP_SET_MAP(key, value, array, static_pool)
{
}
#endif /* end of __AERROSPIKE_TRANSFORM_H__ */
