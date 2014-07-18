/*
 * For GET and PUT method.  Both methods may have LIST and MAP datatypes.  To
 * iterate over it, we need to have callbacks.  To use below macros for code
 * generation, callbacks should have specific nomenclature.
 *
 * Following are the basic actions beased on caller.  callback for LIST =>
 * APPEND callback for MAP => ASSOC initial call for this macro => ASSOC
 *
 * Callback name has following specification:
 * "AEROSPIKE_##level_##method_##action_##datatype"
 *
 */

#ifndef __AEROSPIKE_TRANSFORM_H__
#define __AEROSPIKE_TRANSFORM_H__
#include "aerospike/as_boolean.h"

#define AS_LIST_DATATYPE as_list
#define AS_MAP_DATATYPE as_map

#define AS_MAP_FOREACH as_map_foreach
#define AS_LIST_FOREACH as_list_foreach

#define AS_MAP_FOREACH_CALLBACK as_map_foreach_callback
#define AS_LIST_FOREACH_CALLBACK as_list_foreach_callback

#define FETCH_VALUE_GET(val) as_val_type(val)
#define FETCH_VALUE_PUT(val) Z_TYPE_PP(val)
// do we want to keep it Z_TYPE_P

#define EXPAND_CASE_PUT(level, method, action, datatype, key, value,           \
        array, err, static_pool, label)                                        \
    case IS_##datatype:                                                        \
        if (AEROSPIKE_OK != (err =                                             \
                AEROSPIKE_##level##_##method##_##action##_##datatype(          \
                    key, value, array, static_pool))) {                        \
            goto label;                                                        \
        }                                                                      \
        break;

#define EXPAND_CASE_GET(level, method, action, datatype, key, value,           \
        array, err, static_pool, label)                                        \
            case AS_##datatype:                                                \
                if (AEROSPIKE_OK != (err =                                     \
                    AEROSPIKE_##level##_##method##_##action##_##datatype(      \
                        key, value, array, static_pool))) {                    \
                    goto label;                                                \
                }                                                              \
                break;



#define AEROSPIKE_WALKER_SWITCH_CASE(method, level, action,                    \
        err, static_pool, key, value, array, label)                            \
        AEROSPIKE_WALKER_SWITCH_CASE_##method(method, level, action,           \
                err, static_pool, key, value, array, label)

#define AEROSPIKE_HASHMAP_BUCKET_SIZE     32
#define AEROSPIKE_ASLIST_BLOCK_SIZE       0

#define AERO_DEFAULT_KEY(htable, key, key_len, index, pointer)                 \
    zend_hash_get_current_key_ex(htable, &key, &key_len, &index, 0, &pointer);

#define AERO_LIST_KEY(htable, key, key_len, index, pointer)

#define AERO_MAP_KEY(htable, key, key_len, index, pointer)

#define CURRENT_LIST_SIZE(static_pool)                                         \
    ((as_static_pool *)static_pool)->current_list_id
#define CURRNET_MAP_SIZE(static_pool)                                          \
    ((as_static_pool *)static_pool)->current_map_id

#define CURRENT_LIST_POOL(static_pool)                                         \
    ((as_static_pool *)static_pool)->alloc_list

#define CURRENT_MAP_POOL(static_pool)                                          \
    ((as_static_pool *)static_pool)->alloc_map

#define INIT_LIST_IN_POOL(store, htable)                                       \
    store = as_arraylist_init((as_arraylist *)store,                           \
            zend_hash_num_elements(htable), AEROSPIKE_ASLIST_BLOCK_SIZE);

#define INIT_MAP_IN_POOL(store, htable)                                        \
    store = as_hashmap_init((as_hashmap *)store, AEROSPIKE_HASHMAP_BUCKET_SIZE);

#define INIT_STORE(store, static_pool, htable, level)                          \
    if (AS_MAX_STORE_SIZE > CURRENT_##level##_SIZE(static_pool)) {             \
        store = (void *) CURRENT_##level##_POOL(static_pool)                   \
                [CURRENT_##level##_SIZE(static_pool)++];                       \
        INIT_##level##_IN_POOL(store, htable);                                 \
    }

#define AS_DEFAULT_INIT_STORE(store, htable, static_pool)                      \
    as_record_inita((as_record*) store, zend_hash_num_elements(htable))

#define AS_LIST_INIT_STORE(store, htable, static_pool)                         \
    INIT_STORE(store, static_pool, htable, LIST)

#define AS_MAP_INIT_STORE(store, htable, static_pool)                          \
    INIT_STORE(store, htable, static_pool, MAP)

#define AS_DEFAULT_STORE record
#define AS_LIST_STORE NULL
#define AS_MAP_STORE NULL

#define AS_DEFAULT_KEY key
#define AS_LIST_KEY record
#define AS_MAP_KEY record

#define AEROSPIKE_WALKER_SWITCH_CASE_PUT(method, level, action, err,           \
        static_pool, key, value, store, label)                                 \
do {                                                                           \
    HashTable *htable;                                                         \
    int htable_count;                                                          \
    HashPosition pointer;                                                      \
    zval **dataval;                                                            \
    uint key_len;                                                              \
    ulong index;                                                               \
    htable = Z_ARRVAL_PP((zval**) value);                                      \
    foreach_hashtable(htable, pointer, dataval) {                              \
        AERO_##level##_KEY(htable, key, key_len, index, pointer)               \
        switch (FETCH_VALUE_##method(dataval)) {                               \
            EXPAND_CASE_PUT(level, method, action, ARRAY, key,                 \
                    dataval, store, err, static_pool, label);                  \
            EXPAND_CASE_PUT(level, method, action, STRING, key,                \
                    dataval, store, err, static_pool, label);                  \
            EXPAND_CASE_PUT(level, method, action, LONG, key,                  \
                    dataval, store, err, static_pool, label);                  \
            EXPAND_CASE_PUT(level, method, action, NULL, key,                  \
                    dataval, store, err, static_pool, label);                  \
            default:                                                           \
                err = AEROSPIKE_ERR_PARAM;                                     \
                goto label;                                                    \
        }                                                                      \
    }                                                                          \
} while(0)

#define AEROSPIKE_WALKER_SWITCH_CASE_PUT_DEFAULT_ASSOC(err, static_pool, key,  \
        value, store, label)                                                   \
            AEROSPIKE_WALKER_SWITCH_CASE(PUT, DEFAULT, ASSOC, err,             \
                    static_pool, key, value, store, label)

#define AEROSPIKE_WALKER_SWITCH_CASE_PUT_LIST_APPEND(err, static_pool, key,    \
        value, store, label)                                                   \
            AEROSPIKE_WALKER_SWITCH_CASE(PUT, LIST, APPEND, err,               \
                    static_pool, key, value, store, label)

#define AEROSPIKE_WALKER_SWITCH_CASE_PUT_MAP_ASSOC(err, static_pool, key,      \
        value, store, label)                                                   \
            AEROSPIKE_WALKER_SWITCH_CASE(PUT, MAP, ASSOC, err,                 \
                    static_pool, key, value, store, label)

/*note : INTEGER, it does not translate to IS_LONG ??? */
#define AEROSPIKE_WALKER_SWITCH_CASE_GET(method, level, action, err,           \
        static_pool, key, value, array, label)                                 \
    switch (FETCH_VALUE_##method(value)) {                                     \
        EXPAND_CASE_GET(level, method, action, UNDEF, key, value,              \
                array, err, static_pool, label)                                \
        EXPAND_CASE_GET(level, method, action, NIL, key, value,                \
                array, err, static_pool, label)                                \
        EXPAND_CASE_GET(level, method, action, BOOLEAN, key, value,            \
                array, err, static_pool, label)                                \
        EXPAND_CASE_GET(level, method, action, INTEGER, key, value,            \
                array, err, static_pool, label)                                \
        EXPAND_CASE_GET(level, method, action, STRING, key, value,             \
                array, err, static_pool, label)                                \
        EXPAND_CASE_GET(level, method, action, LIST, key, value,               \
                array, err, static_pool, label)                                \
        EXPAND_CASE_GET(level, method, action, MAP, key, value,                \
                array, err, static_pool, label)                                \
        EXPAND_CASE_GET(level, method, action, REC, key, value,                \
                array, err, static_pool, label)                                \
        EXPAND_CASE_GET(level, method, action, PAIR, key, value,               \
                array, err, static_pool, label)                                \
        EXPAND_CASE_GET(level, method, action, BYTES, key, value,              \
                array, err, static_pool, label)                                \
        default:                                                               \
            err = AEROSPIKE_ERR_PARAM;                                         \
            goto label;                                                        \
    }

#define AEROSPIKE_WALKER_SWITCH_CASE_GET_DEFAULT_ASSOC(err, static_pool, key,  \
        value, array, label)                                                   \
            AEROSPIKE_WALKER_SWITCH_CASE_GET(GET, DEFAULT,                     \
                    ASSOC, err, static_pool, key, value, array, label)

#define AEROSPIKE_WALKER_SWITCH_CASE_GET_MAP_ASSOC(err, static_pool, key,      \
        value, array, label)                                                   \
            AEROSPIKE_WALKER_SWITCH_CASE_GET(GET, MAP,                         \
                    ASSOC, err, static_pool, key, value, array, label)

#define AEROSPIKE_WALKER_SWITCH_CASE_GET_LIST_APPEND(err, static_pool, key,    \
        value, array, label)                                                   \
            AEROSPIKE_WALKER_SWITCH_CASE_GET(GET, LIST,                        \
                    APPEND, err, static_pool, key, value, array, label) 

#define AS_APPEND_LIST_TO_LIST(key, value, array)                              \
    AS_STORE_ITERATE(GET, LIST, APPEND, LIST, key, value, *(zval **)array)

#define AS_APPEND_MAP_TO_LIST(key, value, array)                               \
    AS_STORE_ITERATE(GET, LIST, APPEND, MAP, key, value, *(zval **)array)

#define AS_ASSOC_LIST_TO_MAP(key, value, array)                                \
    AS_STORE_ITERATE(GET, MAP, ASSOC, LIST, key, value, *(zval **)array)

#define AS_ASSOC_MAP_TO_MAP(key, value, array)                                 \
    AS_STORE_ITERATE(GET, MAP, ASSOC, MAP, key, value, *(zval **)array)

#define AS_ASSOC_LIST_TO_DEFAULT(key, value, array)                            \
    AS_STORE_ITERATE(GET, DEFAULT, ASSOC, LIST, key, value, array)

#define AS_ASSOC_MAP_TO_DEFAULT(key, value, array)                             \
    AS_STORE_ITERATE(GET, DEFAULT, ASSOC, MAP, key, value, array)

#define AS_MAX_STORE_SIZE 1024
#define AS_MAX_LIST_SIZE AS_MAX_STORE_SIZE
#define AS_MAX_MAP_SIZE AS_MAX_STORE_SIZE

typedef struct list_map_static_pool {
    u_int32_t        current_list_id;
    as_arraylist     alloc_list[AS_MAX_LIST_SIZE];
    u_int32_t        current_map_id;
    as_hashmap       alloc_map[AS_MAX_MAP_SIZE];
} as_static_pool;

#define IS_MAP_TYPE(htable, key, key_len, index, pointer)                      \
    (zend_hash_get_current_key_ex(htable, &key, &key_len, &index, 0,           \
                                  &pointer) == HASH_KEY_IS_STRING)

#define IS_LIST_TYPE(htable, key, key_len, index, pointer)                     \
    (zend_hash_get_current_key_ex(htable, &key, &key_len, &index, 0,           \
                                  &pointer) != HASH_KEY_IS_STRING)


#define AEROSPIKE_PROCESS_ARRAY(datatype, level, action, label, key,           \
        value, store, status, static_pool) {                                   \
    HashTable *htable;                                                         \
    HashPosition pointer;                                                      \
    char *key = NULL;                                                          \
    void *inner_store;                                                         \
    AS_##level##_INIT_STORE(inner_store, htable, static_pool);                 \
    uint key_len;                                                              \
    ulong index;                                                               \
    htable = Z_ARRVAL_PP((zval**)value);                                       \
    zend_hash_internal_pointer_reset_ex(htable, &pointer);                     \
    if (IS_##datatype##_TYPE(htable, key, key_len, index, pointer)) {          \
        if ((AEROSPIKE_OK != (status =                                         \
                    AEROSPIKE_##level##_PUT_##action##_##datatype(key,         \
                        value, inner_store, static_pool)))) {                  \
            goto label;                                                        \
        }                                                                      \
        AEROSPIKE_##level##_SET_##action##_##datatype(store, inner_store,      \
                key);                                                          \
    }                                                                          \
}

#define AEROSPIKE_PROCESS_ARRAY_DEFAULT_ASSOC_MAP(key, value, store,           \
        status, static_pool, label)                                            \
            AEROSPIKE_PROCESS_ARRAY(MAP, DEFAULT, ASSOC, label, key, value,    \
                    store, status, static_pool)

#define AEROSPIKE_PROCESS_ARRAY_DEFAULT_ASSOC_LIST(key, value, store,          \
        status, static_pool, label)                                            \
            AEROSPIKE_PROCESS_ARRAY(LIST, DEFAULT, ASSOC, label, key, value,   \
                    store, status, static_pool)

static inline as_status AS_DEFAULT_PUT_ASSOC_ARRAY(void *key,
        void *value, void *store, void *static_pool)
{
    as_status status;
    AEROSPIKE_PROCESS_ARRAY_DEFAULT_ASSOC_MAP(key, value, store, status,
            static_pool, exit);
    AEROSPIKE_PROCESS_ARRAY_DEFAULT_ASSOC_LIST(key, value, store, status,
            static_pool, exit);
exit:
    return (status);
}

#define AEROSPIKE_PROCESS_ARRAY_MAP_ASSOC_MAP(key, value, store,               \
        status, static_pool, label)                                            \
            AEROSPIKE_PROCESS_ARRAY(MAP, MAP, ASSOC, label, key, value,        \
                    store, status, static_pool)

#define AEROSPIKE_PROCESS_ARRAY_MAP_ASSOC_LIST(key, value, store,              \
        status, static_pool, label)                                            \
            AEROSPIKE_PROCESS_ARRAY(LIST, MAP, ASSOC, label, key, value,       \
                    store, status, static_pool)

static inline as_status AS_MAP_PUT_ASSOC_ARRAY(void *key,
        void *value, void *store, void *static_pool)
{
    as_status status;
    AEROSPIKE_PROCESS_ARRAY_MAP_ASSOC_MAP(key, value, store, status,
            static_pool, exit);
    AEROSPIKE_PROCESS_ARRAY_MAP_ASSOC_LIST(key, value, store, status,
            static_pool, exit);
exit:
    return (status);
}

#define AEROSPIKE_PROCESS_ARRAY_LIST_APPEND_MAP(key, value, store,             \
        status, static_pool, label)                                            \
            AEROSPIKE_PROCESS_ARRAY(MAP, LIST, APPEND, label, key, value,      \
                    store, status, static_pool)

#define AEROSPIKE_PROCESS_ARRAY_LIST_APPEND_LIST(key, value, store,            \
        status, static_pool, label)                                            \
            AEROSPIKE_PROCESS_ARRAY(LIST, LIST, APPEND, label, key, value,     \
                    store, status, static_pool)

static inline as_status AS_LIST_PUT_APPEND_ARRAY(void *key,
        void *value, void *store, void *static_pool)
{
    as_status status;
    AEROSPIKE_PROCESS_ARRAY_MAP_ASSOC_MAP(key, value, store, status,
            static_pool, exit);
    AEROSPIKE_PROCESS_ARRAY_MAP_ASSOC_LIST(key, value, store, status,
            static_pool, exit);
exit:
    return (status);
}
/* Misc function calls to set inner store  */

#define AEROSPIKE_LIST_SET_APPEND_LIST(outer_store, inner_store, bin_name)     \
    AS_LIST_SET_APPEND_LIST(outer_store, inner_store, bin_name)

#define AEROSPIKE_LIST_SET_APPEND_MAP(outer_store, inner_store, bin_name)      \
    AS_LIST_SET_APPEND_MAP(outer_store, inner_store, bin_name)

#define AEROSPIKE_DEFAULT_SET_ASSOC_LIST(outer_store, inner_store, bin_name)   \
    AS_DEFAULT_SET_ASSOC_LIST(outer_store, inner_store, bin_name)

#define AEROSPIKE_DEFAULT_SET_ASSOC_MAP(outer_store, inner_store, bin_name)    \
    AS_DEFAULT_SET_ASSOC_MAP(outer_store, inner_store, bin_name)

#define AEROSPIKE_MAP_SET_ASSOC_LIST(outer_store, inner_store, bin_name)       \
    AS_MAP_SET_ASSOC_LIST(outer_store, inner_store, bin_name)

#define AEROSPIKE_MAP_SET_ASSOC_MAP(outer_store, inner_store, bin_name)        \
    AS_MAP_SET_ASSOC_MAP(outer_store, inner_store, bin_name)

/* PUT function calls for level = LIST */

#define AEROSPIKE_LIST_PUT_APPEND_NULL(key, value, array, static_pool)         \
    AS_SET_ERROR_CASE(key, value, array, static_pool)

#define AEROSPIKE_LIST_PUT_APPEND_LONG(key, value, array, static_pool)         \
    AS_LIST_PUT_APPEND_INT64(key, value, array, static_pool)

#define AEROSPIKE_LIST_PUT_APPEND_STRING(key, value, array, static_pool)       \
    AS_LIST_PUT_APPEND_STR(key, value, array, static_pool)

#define AEROSPIKE_LIST_PUT_APPEND_ARRAY(key, value, array, static_pool)        \
    AS_LIST_PUT_APPEND_ARRAY(key, value, array, static_pool)

#define AEROSPIKE_LIST_PUT_APPEND_LIST(key, value, array, static_pool)         \
    AS_LIST_PUT_APPEND_LIST(key, value, array, static_pool)

#define AEROSPIKE_LIST_PUT_APPEND_MAP(key, value, array, static_pool)          \
    AS_LIST_PUT_APPEND_MAP(key, value, array, static_pool)

/* PUT function calls for level = DEFAULT */

#define AEROSPIKE_DEFAULT_PUT_ASSOC_NULL(key, value, array, static_pool)       \
    AS_DEFAULT_PUT_ASSOC_NIL(key, value, array, static_pool)

#define AEROSPIKE_DEFAULT_PUT_ASSOC_LONG(key, value, array, static_pool)       \
    AS_DEFAULT_PUT_ASSOCINT64(key, value, array, static_pool)

#define AEROSPIKE_DEFAULT_PUT_ASSOC_STRING(key, value, array, static_pool)     \
    AS_DEFAULT_PUT_ASSOC_STR(key, value, array, static_pool)

#define AEROSPIKE_DEFAULT_PUT_ASSOC_ARRAY(key, value, array, static_pool)      \
    AS_DEFAULT_PUT_ASSOC_ARRAY(key, value, array, static_pool)

#define AEROSPIKE_DEFAULT_PUT_ASSOC_LIST(key, value, array, static_pool)       \
    AS_DEFAULT_PUT_ASSOC_LIST(key, value, array, static_pool)

#define AEROSPIKE_DEFAULT_PUT_ASSOC_MAP(key, value, array, static_pool)        \
    AS_DEFAULT_PUT_ASSOC_MAP(key, value, array, static_pool)

/* PUT function calls for level = MAP */

#define AEROSPIKE_MAP_PUT_ASSOC_NULL(key, value, array, static_pool)           \
    AS_SET_ERROR_CASE(key, value, array, static_pool)

#define AEROSPIKE_MAP_PUT_ASSOC_LONG(key, value, array, static_pool)           \
    AS_MAP_PUT_ASSOC_INT64(key, value, array, static_pool)

#define AEROSPIKE_MAP_PUT_ASSOC_STRING(key, value, array, static_pool)         \
    AS_MAP_PUT_ASSOC_STR(key, value, array, static_pool)

#define AEROSPIKE_MAP_PUT_ASSOC_ARRAY(key, value, array, static_pool)          \
    AS_MAP_PUT_ASSOC_ARRAY(key, value, array, static_pool)

#define AEROSPIKE_MAP_PUT_ASSOC_LIST(key, value, array, static_pool)           \
    AS_MAP_PUT_ASSOC_LIST(key, value, array, static_pool)

#define AEROSPIKE_MAP_PUT_ASSOC_MAP(key, value, array, static_pool)            \
    AS_MAP_PUT_ASSOC_MAP(key, value, array, static_pool)

/* GET function calls for level = LIST */

#define AEROSPIKE_LIST_GET_APPEND_UNDEF(key, value, array, static_pool)        \
    ADD_LIST_APPEND_NULL(key, value, array)

#define AEROSPIKE_LIST_GET_APPEND_UNKNOWN(key, value, array, static_pool)      \
    ADD_LIST_APPEND_NULL(key, value, array)

#define AEROSPIKE_LIST_GET_APPEND_NIL(key, value, array, static_pool)          \
    ADD_LIST_APPEND_NULL(key, value, array)

#define AEROSPIKE_LIST_GET_APPEND_BOOLEAN(key, value, array, static_pool)      \
    ADD_LIST_APPEND_BOOL(key, value, array)

#define AEROSPIKE_LIST_GET_APPEND_INTEGER(key, value, array, static_pool)      \
    ADD_LIST_APPEND_LONG(key, value, array)

#define AEROSPIKE_LIST_GET_APPEND_STRING(key, value, array, static_pool)       \
    ADD_LIST_APPEND_STRING(key, value, array)

#define AEROSPIKE_LIST_GET_APPEND_LIST(key, value, array, static_pool)         \
    ADD_LIST_APPEND_LIST(key, value, array)

#define AEROSPIKE_LIST_GET_APPEND_MAP(key, value, array, static_pool)          \
    ADD_LIST_APPEND_MAP(key, value, array)

#define AEROSPIKE_LIST_GET_APPEND_REC(key, value, array, static_pool)          \
    ADD_LIST_APPEND_REC(key, value, array)

#define AEROSPIKE_LIST_GET_APPEND_PAIR(key, value, array, static_pool)         \
    ADD_LIST_APPEND_PAIR(key, value, array)

#define AEROSPIKE_LIST_GET_APPEND_BYTES(key, value, array, static_pool)        \
    ADD_LIST_APPEND_BYTES(key, value, array) 

/* GET function calls for level = DEFAULT */

#define AEROSPIKE_DEFAULT_GET_ASSOC_UNDEF(key, value, array, static_pool)      \
    ADD_DEFAULT_ASSOC_NULL(key, value, array)

#define AEROSPIKE_DEFAULT_GET_ASSOC_UNKNOWN(key, value, array, static_pool)    \
    ADD_DEFAULT_ASSOC_NULL(key, value, array)

#define AEROSPIKE_DEFAULT_GET_ASSOC_NIL(key, value, array, static_pool)        \
    ADD_DEFAULT_ASSOC_NULL(key, value, array)

#define AEROSPIKE_DEFAULT_GET_ASSOC_BOOLEAN(key, value, array, static_pool)    \
    ADD_DEFAULT_ASSOC_BOOL(key, value, array)

#define AEROSPIKE_DEFAULT_GET_ASSOC_INTEGER(key, value, array, static_pool)    \
    ADD_DEFAULT_ASSOC_LONG(key, value, array)

#define AEROSPIKE_DEFAULT_GET_ASSOC_STRING(key, value, array, static_pool)     \
    ADD_DEFAULT_ASSOC_STRING(key, value, array)

#define AEROSPIKE_DEFAULT_GET_ASSOC_LIST(key, value, array, static_pool)       \
    ADD_DEFAULT_ASSOC_LIST(key, value, array)

#define AEROSPIKE_DEFAULT_GET_ASSOC_MAP(key, value, array, static_pool)        \
    ADD_DEFAULT_ASSOC_MAP(key, value, array)

#define AEROSPIKE_DEFAULT_GET_ASSOC_REC(key, value, array, static_pool)        \
    ADD_DEFAULT_ASSOC_REC(key, value, array)

#define AEROSPIKE_DEFAULT_GET_ASSOC_PAIR(key, value, array, static_pool)       \
    ADD_DEFAULT_ASSOC_PAIR(key, value, array)

#define AEROSPIKE_DEFAULT_GET_ASSOC_BYTES(key, value, array, static_pool)      \
    ADD_DEFAULT_ASSOC_BYTES(key, value, array) 

/* GET function calls for level = MAP */

#define AEROSPIKE_MAP_GET_ASSOC_UNDEF(key, value, array, static_pool)          \
    ADD_MAP_ASSOC_NULL(key, value, array)

#define AEROSPIKE_MAP_GET_ASSOC_UNKNOWN(key, value, array, static_pool)        \
    ADD_MAP_ASSOC_NULL(key, value, array)

#define AEROSPIKE_MAP_GET_ASSOC_NIL(key, value, array, static_pool)            \
    ADD_MAP_ASSOC_NULL(key, value, array)

#define AEROSPIKE_MAP_GET_ASSOC_BOOLEAN(key, value, array, static_pool)        \
    ADD_MAP_ASSOC_BOOL(key, value, array)

#define AEROSPIKE_MAP_GET_ASSOC_INTEGER(key, value, array, static_pool)        \
    ADD_MAP_ASSOC_LONG(key, value, array)

#define AEROSPIKE_MAP_GET_ASSOC_STRING(key, value, array, static_pool)         \
    ADD_MAP_ASSOC_STRING(key, value, array)

#define AEROSPIKE_MAP_GET_ASSOC_LIST(key, value, array, static_pool)           \
    ADD_MAP_ASSOC_LIST(key, value, array)

#define AEROSPIKE_MAP_GET_ASSOC_MAP(key, value, array, static_pool)            \
    ADD_MAP_ASSOC(key, value, array)

#define AEROSPIKE_MAP_GET_ASSOC_REC(key, value, array, static_pool)            \
    ADD_MAP_ASSOC_REC(key, value, array)

#define AEROSPIKE_MAP_GET_ASSOC_PAIR(key, value, array, static_pool)           \
    ADD_MAP_ASSOC_PAIR(key, value, array)

#define AEROSPIKE_MAP_GET_ASSOC_BYTES(key, value, array, static_pool)          \
    ADD_MAP_ASSOC_BYTES(key, value, array) 

#define ADD_MAP_ZVAL(array, key, store)                                        \
    add_assoc_zval(array, as_string_get((as_string *) key), store)

#define ADD_DEFAULT_ZVAL(array, key, store) add_assoc_zval(array, key, store)

#define ADD_LIST_ZVAL(array, key, store) add_next_index_zval(array, store)

#define AS_STORE_ITERATE(method, level, action, datatype, key, value, array)   \
do {                                                                           \
    zval *store;                                                               \
    MAKE_STD_ZVAL(store);                                                      \
    array_init(store);                                                         \
    AS_##datatype##_FOREACH((AS_##datatype##_DATATYPE*) value,                 \
            (AS_##datatype##_FOREACH_CALLBACK)                                 \
            AS_##datatype##_##method##_CALLBACK, &store);                      \
    ADD_##action##_ZVAL(array, key, store);                                    \
} while(0);

#endif /* end of __AERROSPIKE_TRANSFORM_H__ */
