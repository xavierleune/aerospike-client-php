/*
 *******************************************************************************************************
 * DESIGN CONSIDERATIONS:
 *******************************************************************************************************
 *      Looking at the earlier version of the code, we found similarities at
 *      various levels of code. Only changes were the naming terminologies for
 *      different methods, but the overall structuring and orchestration was
 *      pretty much similar at various levels.
 *
 *      With this in mind, the immediate thought process in mind was to work
 *      with macros and its concatenation with '##'. It will help us to generate
 *      similar code at various levels, but from the same location. Advantages
 *      of this approach are:
 *      1. Modularity of the code
 *      2. Code maintenance is easy as nomenclature and set of methods to write
 *          is unified and generic for all new enhancements.
 *      3. Reduces number of lines of visible code and easy to understand for
 *          developers once they get acquainted with the terminologies used.
 *
 *      Disadvantages:
 *      1. Macros are not debug friendly, but the function calls called within
 *          the macros can be debugged.
 *      2. Macro backbone should be stable enough. Then any issue occurred in
 *          the future can be debugged at the function level as we are trusting
 *          the backbone.
 *      3. Developers need to have clear understanding about the expansion of
 *          macros and location of their function calls which will be generated
 *          during the expansion with '##'.
 *
 *          (In case, if developer wants to see the expanded code of macro,
 *          gcc provides an option flag '-E'.
 *          This can be updated before the ".c" file name in Makefile.
 *          The ".c" file should have the Macro call which developer expects to
 *          expand.)
 *
 *******************************************************************************************************
 *
 *  Here is the overall view of architecture terminologies used to write macros:
 *  level => It represents the processing level in the code flow and can take
 *          following values. e.g. DEFAULT, MAP, LIST.
 *      DEFAULT represents that we are processing Record.
 *      MAP represents that we are processing Map.
 *      LIST represents that we are processing List.
 *
 *  method => It represents the method we are processing. viz. GET OR PUT.
 *           It can take any other method values in future sharing similar
 *           infrastructure.
 *
 *  action => It represents the action to be taken on that 'level' for that
 *           'method'.
 *           e.g. ASSOC, APPEND, INDEX
 *           ASSOC => All set(PUT) and/or assoc(GET) operations in Record and
 *              Map(with string keys for GET) are represented by action 'ASSOC'.
 *           INDEX => All assoc(GET) operations in Map(with integer keys for GET)
 *              are represented by action 'INDEX'.
 *           APPEND => All append(PUT) and/or get next (GET) operations in List
 *              are represented by action 'APPEND'.
 *
 *  datatype => It represents the actual data type which needs to be processed
 *  at particular 'level' for particular 'method' with particular 'action'.
 *  The datatypes will vary for GET as well as PUT method.
 *  The data types in GET method are inherited from the data types of CSDK.
 *  The data types in PUT method corresponds to the PHP data types.
 *
 *  Method name call will look like following as per the specification::
 *  "AEROSPIKE_##level_##method_##action_##datatype"
 *
 *  It can be derived to following names,
 *  for example,
 *  
 *  AEROSPIKE_MAP_GET_ASSOC_STRING
 *  AEROSPIKE_LIST_PUT_APPEND_INTEGER
 *  AEROSPIKE_DEFAULT_GET_ASSOC_MAP
 *
 *  Special Case:
 *  AEROSPIKE_##level##_PUT_##action##_ARRAY
 *
 *  In the PHP code, Map and List are interpreted as an Array.
 *  In order to differentiate the incoming Array, and categorize them into
 *  List and Map, we have special implementation inside the switch case of Array
 *  to handle this case for method 'PUT'.
 *
 *******************************************************************************************************
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

/*
 * Macro Expansion for data type LONG for method PUT:
 * ************************************************************************
 * => For level = DEFAULT, action = ASSOC
 * ************************************************************************
 * case IS_LONG:
 *      AEROSPIKE_DEFAULT_PUT_ASSOC_LONG(
 *              key, value, array, static_pool, serializer_policy, err)));
 *      if (AEROSPIKE_OK != (err->code)) {
 *          goto label;
 *      }
 *      break;
 * ************************************************************************
 * => For level = MAP, action = ASSOC
 * ************************************************************************
 * case IS_LONG:
 *      AEROSPIKE_MAP_PUT_ASSOC_LONG(
 *              key, value, array, static_pool, serializer_policy, err)));
 *      if (AEROSPIKE_OK != (err->code)) {
 *          goto label;
 *      }
 *      break;
 * ************************************************************************
 * => For level = LIST, action = APPEND
 * ************************************************************************
 * case IS_LONG:
 *      AEROSPIKE_LIST_PUT_APPEND_LONG(
 *              key, value, array, static_pool, serializer_policy, err)));
 *      if (AEROSPIKE_OK != (err->code)) {
 *          goto label;
 *      }
 *      break;
 * ************************************************************************
 */
#define EXPAND_CASE_PUT(level, method, action, datatype, key, value,             \
        array, err, static_pool, label, serializer_policy)                       \
    case IS_##datatype:                                                          \
        AEROSPIKE_##level##_##method##_##action##_##datatype(                    \
                key, value, array, static_pool, serializer_policy,               \
                err);                                                            \
        if (AEROSPIKE_OK != (err->code)) {                                       \
            goto label;                                                          \
        }                                                                        \
        break;

/*
 * Macro Expansion for data type STRING for method GET:
 * ************************************************************************
 * => For level = DEFAULT, action = ASSOC
 * ************************************************************************
 * case AS_STRING:
 *      AEROSPIKE_DEFAULT_GET_ASSOC_STRING(
 *              key, value, array, static_pool, err);
 *      if (AEROSPIKE_OK != ((as_error *)err)->code) {
 *          goto label;
 *      }
 *      break;
 * ************************************************************************
 * => For level = MAP, action = ASSOC
 * ************************************************************************
 * case AS_STRING:
 *      AEROSPIKE_MAP_GET_ASSOC_STRING(
 *              key, value, array, static_pool, err);
 *      if (AEROSPIKE_OK != ((as_error *)err)->code) {
 *          goto label;
 *      }
 *      break;
 * ************************************************************************
 * => For level = LIST, action = APPEND
 * ************************************************************************
 * case AS_STRING:
 *      AEROSPIKE_LIST_GET_APPEND_STRING(
 *              key, value, array, static_pool, err);
 *      if (AEROSPIKE_OK != ((as_error *)err)->code) {
 *          goto label;
 *      }
 *      break;
 * ************************************************************************
 */
#define EXPAND_CASE_GET(level, method, action, datatype, key, value,           \
        array, err, static_pool, label)                                        \
    case AS_##datatype:                                                        \
        AEROSPIKE_##level##_##method##_##action##_##datatype(                  \
                key, value, array, static_pool, err);                          \
        if (AEROSPIKE_OK != ((as_error *)err)->code) {                         \
            goto label;                                                        \
        }                                                                      \
        break;

/*
 *******************************************************************************************************
 * This is the main walker which will walk over all datatypes for all actions in
 * all methods at all levels.
 * You will find the wrapper macros over this one for particular tasks.
 *******************************************************************************************************
 */
#define AEROSPIKE_WALKER_SWITCH_CASE(method, level, action,                       \
        err, static_pool, key, value, array, label, serializer_policy)            \
        AEROSPIKE_WALKER_SWITCH_CASE_##method(method, level, action,              \
                err, static_pool, key, value, array, label, serializer_policy)

#define AEROSPIKE_HASHMAP_BUCKET_SIZE     32
#define AEROSPIKE_ASLIST_BLOCK_SIZE       0

/*
 *******************************************************************************************************
 * For the case of method PUT, we need to deduce the key for Record as well as
 * Map when we iterate over array sent by PHP.
 *
 * These methods will generalise the key generation.
 *******************************************************************************************************
 */
#define AS_DEFAULT_KEY(hashtable, key, key_len, index, pointer,                \
        static_pool, err, label)                                               \
            zend_hash_get_current_key_ex(hashtable, (char **)&key, &key_len,   \
                    &index, 0, &pointer);                                      \
            if ((char*)key == NULL) {                                          \
                err->code = AEROSPIKE_ERR_CLIENT;                              \
                goto label;                                                    \
            }                                                                  \
            if (key_len > (AS_BIN_NAME_MAX_LEN + 1)) {                         \
                PHP_EXT_SET_AS_ERR(err, AEROSPIKE_ERR_BIN_NAME, "Bin name longer than 14 chars");   \
                goto label;                                                    \
            }

#define AS_LIST_KEY(hashtable, key, key_len, index, pointer, static_pool,      \
        err, label)                                                            \

#define AS_MAP_KEY(hashtable, key, key_len, index, pointer, static_pool,       \
        err, label)                                                            \
do {                                                                           \
    char *local_key;                                                           \
    uint key_type = zend_hash_get_current_key_ex(hashtable,                    \
            (char **)&local_key, &key_len, &index, 0, &pointer);               \
    if (key_type == HASH_KEY_IS_STRING) {                                      \
        as_string *map_str;                                                    \
        GET_STR_POOL(map_str, static_pool, err, label);                        \
        as_string_init(map_str, local_key, false);                             \
        key = (as_val*) (map_str);                                             \
    } else if (key_type == HASH_KEY_IS_LONG) {                                 \
        as_integer *map_int;                                                   \
        GET_INT_POOL(map_int, static_pool, err, label);                        \
        as_integer_init(map_int, index);                                       \
        key = (as_val*) map_int;                                               \
    } else {                                                                   \
        PHP_EXT_SET_AS_ERR(err, AEROSPIKE_ERR_CLIENT, "Invalid Key type for Map");    \
        goto label;                                                            \
    }                                                                          \
} while(0);

/* 
 *******************************************************************************************************
 * End of key deduction methods for Record, List and Map. 
 *******************************************************************************************************
 */

/* 
 *******************************************************************************************************
 * Macros to access Static Pool 
 *******************************************************************************************************
 */
#define CURRENT_LIST_SIZE(static_pool)                                         \
    ((as_static_pool *)static_pool)->current_list_id

#define CURRENT_MAP_SIZE(static_pool)                                          \
    ((as_static_pool *)static_pool)->current_map_id

#define STR_CNT(static_pool)                                                   \
    (((as_static_pool *)static_pool)->current_str_id)

#define INT_CNT(static_pool)                                                   \
    (((as_static_pool *)static_pool)->current_int_id)

#define BYTES_CNT(static_pool)                                                 \
    (((as_static_pool *)static_pool)->current_bytes_id)

#define STR_POOL(static_pool)                                                  \
    ((as_static_pool *)static_pool)->string_pool

#define INT_POOL(static_pool)                                                  \
    ((as_static_pool *)static_pool)->integer_pool

#define BYTES_POOL(static_pool)                                                \
    ((as_static_pool *)static_pool)->bytes_pool

#define CURRENT_LIST_POOL(static_pool)                                         \
    ((as_static_pool *)static_pool)->alloc_list

#define CURRENT_MAP_POOL(static_pool)                                          \
    ((as_static_pool *)static_pool)->alloc_map

#define GET_STR_POOL(map_str, static_pool, err, label)                         \
    if (AS_MAX_STORE_SIZE > STR_CNT(static_pool)) {                            \
        map_str = &(STR_POOL(static_pool)[STR_CNT(static_pool)++]);            \
    } else {                                                                   \
        PHP_EXT_SET_AS_ERR(err, AEROSPIKE_ERR_CLIENT, "Cannot allocate as_string");   \
        goto label;                                                            \
    }

#define GET_INT_POOL(map_int, static_pool, err, label)                         \
    if (AS_MAX_STORE_SIZE > INT_CNT(static_pool)) {                            \
        map_int = &(INT_POOL(static_pool)[INT_CNT(static_pool)++]);            \
    } else {                                                                   \
        PHP_EXT_SET_AS_ERR(err, AEROSPIKE_ERR_CLIENT, "Cannot allocate as_integer");  \
        goto label;                                                            \
    }

#define GET_BYTES_POOL(map_bytes, static_pool, err, label)                     \
    if (AS_MAX_STORE_SIZE > BYTES_CNT(static_pool)) {                          \
        map_bytes = &(BYTES_POOL(static_pool)[BYTES_CNT(static_pool)++]);      \
    } else {                                                                   \
        PHP_EXT_SET_AS_ERR(err, AEROSPIKE_ERR_CLIENT, "Cannot allocate as_bytes");    \
        goto label;                                                            \
    }

#define INIT_LIST_IN_POOL(store, hashtable)                                    \
    store = as_arraylist_init((as_arraylist *)store,                           \
            zend_hash_num_elements(hashtable), AEROSPIKE_ASLIST_BLOCK_SIZE);

#define INIT_MAP_IN_POOL(store, hashtable_)                                    \
    store = (as_hashmap *) as_hashmap_init((as_hashmap*)store,                 \
            AEROSPIKE_HASHMAP_BUCKET_SIZE);       

#define INIT_STORE(store, static_pool, hashtable, level, err, label)           \
    if (AS_MAX_STORE_SIZE > CURRENT_##level##_SIZE(static_pool)) {             \
        store = (void *)                                                       \
        &CURRENT_##level##_POOL(static_pool)[                                  \
        (CURRENT_##level##_SIZE(static_pool))++];                              \
        INIT_##level##_IN_POOL(store, hashtable);                              \
    } else {                                                                   \
        PHP_EXT_SET_AS_ERR(err, AEROSPIKE_ERR_CLIENT, "Cannot allocate list/map");    \
        goto label;                                                            \
    }

#define AS_DEFAULT_INIT_STORE(store, hashtable, static_pool, err, label)                      

#define AS_LIST_INIT_STORE(store, hashtable, static_pool, err, label)          \
    INIT_STORE(store, static_pool, hashtable, LIST, err, label)

#define AS_MAP_INIT_STORE(store, hashtable, static_pool, err, label)           \
    INIT_STORE(store, static_pool, hashtable, MAP, err, label)

/*
 *******************************************************************************************************
 * End of macros for accessing Static Pools.
 *******************************************************************************************************
 */

/*
 *******************************************************************************************************
 * Walker for PUT:
 * It will loop over all the complex datatypes(record, map, list) and generate
 * the code for various levels. It will populate switch case to classify the
 * datatypes and deduce respective methods from each case (expanded above).
 *******************************************************************************************************
 */
#define AEROSPIKE_WALKER_SWITCH_CASE_PUT(method, level, action, err,           \
        static_pool, key, value, store, label, serializer_policy)              \
do {                                                                           \
    HashTable *hashtable;                                                      \
    int htable_count;                                                          \
    HashPosition pointer;                                                      \
    zval **dataval;                                                            \
    uint key_len;                                                              \
    ulong index;                                                               \
    hashtable = Z_ARRVAL_PP((zval**) value);                                   \
    foreach_hashtable(hashtable, pointer, dataval) {                           \
        AS_##level##_KEY(hashtable, key, key_len, index, pointer,              \
                static_pool, err, label)                                       \
        switch (FETCH_VALUE_##method(dataval)) {                               \
            EXPAND_CASE_PUT(level, method, action, ARRAY, key,                 \
                    dataval, store, err, static_pool, label,                   \
                    serializer_policy);                                        \
            EXPAND_CASE_PUT(level, method, action, STRING, key,                \
                    dataval, store, err, static_pool, label, -1);              \
            EXPAND_CASE_PUT(level, method, action, LONG, key,                  \
                    dataval, store, err, static_pool, label, -1);              \
            EXPAND_CASE_PUT(level, method, action, DOUBLE, key,                \
                    dataval, store, err, static_pool, label,                   \
                    serializer_policy);                                        \
            EXPAND_CASE_PUT(level, method, action, NULL, key,                  \
                    dataval, store, err, static_pool, label,                   \
                    serializer_policy);                                        \
            EXPAND_CASE_PUT(level, method, action, OBJECT, key,                \
                    dataval, store, err, static_pool, label,                   \
                    serializer_policy);                                        \
            EXPAND_CASE_PUT(level, method, action, BOOL, key,                  \
                    dataval, store, err, static_pool, label,                   \
                    serializer_policy);                                        \
            default:                                                           \
                PHP_EXT_SET_AS_ERR(err, AEROSPIKE_ERR_PARAM,                   \
                        "Invalid Datatype");                                   \
                goto label;                                                    \
        }                                                                      \
    }                                                                          \
} while(0)

/*
 *******************************************************************************************************
 * Wrappers over the walker of PUT for all levels with all actions.
 *******************************************************************************************************
 */
#define AEROSPIKE_WALKER_SWITCH_CASE_PUT_DEFAULT_ASSOC(err, static_pool, key,  \
        value, store, label, serializer_policy)                                \
            AEROSPIKE_WALKER_SWITCH_CASE(PUT, DEFAULT, ASSOC, err,             \
                    static_pool, key, value, store, label, serializer_policy)

#define AEROSPIKE_WALKER_SWITCH_CASE_PUT_LIST_APPEND(err, static_pool, key,    \
        value, store, label, serializer_policy)                                \
            AEROSPIKE_WALKER_SWITCH_CASE(PUT, LIST, APPEND, err,               \
                    static_pool, key, value, store, label, serializer_policy)

#define AEROSPIKE_WALKER_SWITCH_CASE_PUT_MAP_ASSOC(err, static_pool, key,      \
        value, store, label, serializer_policy)                                \
            AEROSPIKE_WALKER_SWITCH_CASE(PUT, MAP, ASSOC, err,                 \
                    static_pool, key, value, store, label, serializer_policy)
/* 
 *******************************************************************************************************
 * End of Wrappers over the walker of PUT.
 *******************************************************************************************************
 */


/*
 *******************************************************************************************************
 * Walker for GET:
 * It will provide callbacks for complex datatypes(map, list) and generate
 * the code for various levels. It will populate switch case to classify the
 * datatypes and deduce respective methods from each case (expanded above).
 *******************************************************************************************************
 */
#define AEROSPIKE_WALKER_SWITCH_CASE_GET(method, level, action, err,           \
        static_pool, key, value, array, label, serializer_policy)              \
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
            ((as_error *) err)->code = AEROSPIKE_ERR_PARAM;                    \
            goto label;                                                        \
    }

/*
 *******************************************************************************************************
 * Wrappers over the walker of GET for all levels with all actions
 *******************************************************************************************************
 */
#define AEROSPIKE_WALKER_SWITCH_CASE_GET_DEFAULT_ASSOC(err, static_pool, key,  \
        value, array, label)                                                   \
            AEROSPIKE_WALKER_SWITCH_CASE(GET, DEFAULT, ASSOC, err,             \
                    static_pool, key, value, array, label, -1)

#define AEROSPIKE_WALKER_SWITCH_CASE_GET_MAP_ASSOC(err, static_pool, key,      \
        value, array, label)                                                   \
            AEROSPIKE_WALKER_SWITCH_CASE(GET, MAP, ASSOC, err,                 \
                    static_pool, key, value, array, label, -1)

#define AEROSPIKE_WALKER_SWITCH_CASE_GET_MAP_INDEX(err, static_pool, key,      \
        value, array, label)                                                   \
            AEROSPIKE_WALKER_SWITCH_CASE(GET, MAP, INDEX, err,                 \
                    static_pool, key, value, array, label, -1)

#define AEROSPIKE_WALKER_SWITCH_CASE_GET_LIST_APPEND(err, static_pool, key,    \
        value, array, label)                                                   \
            AEROSPIKE_WALKER_SWITCH_CASE(GET, LIST, APPEND, err,               \
                    static_pool, key, value, array, label, -1) 

/* 
 *******************************************************************************************************
 * End of Wrappers over the walker of PUT.
 *******************************************************************************************************
 */

/* 
 *******************************************************************************************************
 * Macros for GET to iterate over complex datatypes and handle internal complex
 * datatypes with callbacks.
 *******************************************************************************************************
 */
#define AS_APPEND_LIST_TO_LIST(key, value, array, err)                         \
    AS_STORE_ITERATE(GET, LIST, APPEND, LIST, key, value, *(zval **)array, err)

#define AS_APPEND_MAP_TO_LIST(key, value, array, err)                          \
    AS_STORE_ITERATE(GET, LIST, APPEND, MAP, key, value, *(zval **)array, err)

#define AS_ASSOC_LIST_TO_MAP(key, value, array, err)                           \
    AS_STORE_ITERATE(GET, MAP, ASSOC, LIST, key, value, *(zval **)array, err)

#define AS_ASSOC_MAP_TO_MAP(key, value, array, err)                            \
    AS_STORE_ITERATE(GET, MAP, ASSOC, MAP, key, value, *(zval **)array, err)

#define AS_INDEX_LIST_TO_MAP(key, value, array, err)                           \
    AS_STORE_ITERATE(GET, MAP, INDEX, LIST, key, value, *(zval **)array, err)

#define AS_INDEX_MAP_TO_MAP(key, value, array, err)                            \
    AS_STORE_ITERATE(GET, MAP, INDEX, MAP, key, value, *(zval **)array, err)

#define AS_ASSOC_LIST_TO_DEFAULT(key, value, array, err)                       \
    AS_STORE_ITERATE(GET, DEFAULT, ASSOC, LIST, key, value, array, err)

#define AS_ASSOC_MAP_TO_DEFAULT(key, value, array, err)                        \
    AS_STORE_ITERATE(GET, DEFAULT, ASSOC, MAP, key, value, array, err)

#define AS_STORE_ITERATE(method, level, action, datatype, key, value, array,   \
        err)                                                                   \
do {                                                                           \
    zval *store;                                                               \
    MAKE_STD_ZVAL(store);                                                      \
    array_init(store);                                                         \
    foreach_callback_udata  foreach_##datatype##_callback_udata;               \
    foreach_##datatype##_callback_udata.udata_p = store;                       \
    foreach_##datatype##_callback_udata.error_p = (as_error *) err;            \
    AS_##datatype##_FOREACH((AS_##datatype##_DATATYPE*) value,                 \
            (AS_##datatype##_FOREACH_CALLBACK)                                 \
            AS_##datatype##_##method##_CALLBACK,                               \
            &foreach_##datatype##_callback_udata);                             \
    ADD_##level##_##action##_ZVAL(array, key,                                  \
            foreach_##datatype##_callback_udata.udata_p)                       \
} while(0);

/* 
 *******************************************************************************************************
 * End of Macros for GET iteration over complex datatypes.
 *******************************************************************************************************
 */

/* 
 *******************************************************************************************************
 * Macro to iterate over the keys of an array.
 * If number of iterations matches the length of array,
 * then we can say that it is a LIST.
 * Else, it is a MAP.
 *******************************************************************************************************
 */
#define TRAVERSE_KEYS(hashtable, key, key_len, index, pointer, key_iterator)   \
    while ((zend_hash_get_current_key_ex(hashtable, (char **)&key,             \
            &key_len, &index, 0, &pointer) == HASH_KEY_IS_LONG) &&             \
            index == key_iterator) {                                           \
        key_iterator++;                                                        \
        zend_hash_move_forward_ex(hashtable, &pointer);                        \
    }                                                                          \

/*
 *******************************************************************************************************
 * Special implementation for Array:
 * This macro will deduce whether given array is of type LIST OR MAP.
 * It will call respective functions for LIST and MAP to iterate over them.
 * After iteration, it will set those values to the parent store.
 *******************************************************************************************************
 */
#define AEROSPIKE_PROCESS_ARRAY(level, action, label, key, value, store,       \
                                err, static_pool, serializer_policy)           \
    HashTable *hashtable;                                                      \
    HashPosition pointer;                                                      \
    char *inner_key = NULL;                                                    \
    void *inner_store;                                                         \
    uint inner_key_len;                                                        \
    ulong index;                                                               \
    uint key_iterator = 0;                                                     \
    hashtable = Z_ARRVAL_PP((zval**)value);                                    \
    zend_hash_internal_pointer_reset_ex(hashtable, &pointer);                  \
    TRAVERSE_KEYS(hashtable, inner_key, inner_key_len, index, pointer,         \
            key_iterator)                                                      \
    if (key_iterator == zend_hash_num_elements(hashtable)) {                   \
        AS_LIST_INIT_STORE(inner_store, hashtable, static_pool,                \
                err, label);                                                   \
        AEROSPIKE_##level##_PUT_##action##_LIST(inner_key,                     \
                        value, inner_store, static_pool,                       \
                            serializer_policy, err);                           \
        if (AEROSPIKE_OK != (err->code)) {                                     \
            goto label;                                                        \
        }                                                                      \
        AEROSPIKE_##level##_SET_##action##_LIST(store,                         \
                       inner_store, key, err);                                 \
        if(AEROSPIKE_OK != (err->code)) {                                      \
            goto label;                                                        \
        }                                                                      \
    } else {                                                                   \
        AS_MAP_INIT_STORE(inner_store, hashtable, static_pool,                 \
                err, label);                                                   \
        AEROSPIKE_##level##_PUT_##action##_MAP(inner_key,                      \
                        value, inner_store, static_pool,                       \
                            serializer_policy, err);                           \
        if (AEROSPIKE_OK != (err->code)) {                                     \
            goto label;                                                        \
        }                                                                      \
        AEROSPIKE_##level##_SET_##action##_MAP(store,                          \
                       inner_store, key, err);                                 \
        if (AEROSPIKE_OK != (err->code)) {                                     \
            goto label;                                                        \
        }                                                                      \
    }                                                                          
/*
 *******************************************************************************************************
 * End of macro for special implementation of Array.
 *******************************************************************************************************
 */

/*
 *******************************************************************************************************
 * Miscellaneous function calls to set inner store
 *******************************************************************************************************
 */
#define AEROSPIKE_LIST_SET_APPEND_LIST(outer_store, inner_store, bin_name,     \
        err)                                                                   \
    AS_LIST_SET_APPEND_LIST(outer_store, inner_store, bin_name, err TSRMLS_CC)

#define AEROSPIKE_LIST_SET_APPEND_MAP(outer_store, inner_store, bin_name, err) \
    AS_LIST_SET_APPEND_MAP(outer_store, inner_store, bin_name, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_SET_ASSOC_LIST(outer_store, inner_store, bin_name,   \
        err)                                                                   \
    AS_DEFAULT_SET_ASSOC_LIST(outer_store, inner_store, bin_name, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_SET_ASSOC_MAP(outer_store, inner_store, bin_name,    \
        err)                                                                   \
    AS_DEFAULT_SET_ASSOC_MAP(outer_store, inner_store, bin_name, err TSRMLS_CC)

#define AEROSPIKE_MAP_SET_ASSOC_LIST(outer_store, inner_store, bin_name, err)  \
    AS_MAP_SET_ASSOC_LIST(outer_store, inner_store, bin_name, err TSRMLS_CC)

#define AEROSPIKE_MAP_SET_ASSOC_MAP(outer_store, inner_store, bin_name, err)   \
    AS_MAP_SET_ASSOC_MAP(outer_store, inner_store, bin_name, err TSRMLS_CC)

/*
 *******************************************************************************************************
 * PUT function calls for level = LIST
 *******************************************************************************************************
 */
#define AEROSPIKE_LIST_PUT_APPEND_NULL(key, value, array, static_pool,         \
           serializer_policy, err)                                             \
    AS_LIST_PUT_APPEND_BYTES(key, value, array, static_pool,                   \
            serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_LIST_PUT_APPEND_LONG(key, value, array, static_pool,         \
           serializer_policy, err)                                             \
    AS_LIST_PUT_APPEND_INT64(key, value, array, static_pool,                   \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_LIST_PUT_APPEND_STRING(key, value, array, static_pool,       \
           serializer_policy, err)                                             \
    AS_LIST_PUT_APPEND_STR(key, value, array, static_pool,                     \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_LIST_PUT_APPEND_ARRAY(key, value, array, static_pool,        \
           serializer_policy, err)                                             \
    AS_LIST_PUT_APPEND_ARRAY(key, value, array, static_pool,                   \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_LIST_PUT_APPEND_LIST(key, value, array, static_pool,         \
            serializer_policy, err)                                            \
    AS_LIST_PUT_APPEND_LIST(key, value, array, static_pool,                    \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_LIST_PUT_APPEND_MAP(key, value, array, static_pool,          \
           serializer_policy, err)                                             \
    AS_LIST_PUT_APPEND_MAP(key, value, array, static_pool,                     \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_LIST_PUT_APPEND_OBJECT(key, value, array, static_pool,       \
           serializer_policy, err)                                             \
    AS_LIST_PUT_APPEND_BYTES(key, value, array, static_pool,                   \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_LIST_PUT_APPEND_DOUBLE(key, value, array, static_pool,       \
           serializer_policy, err)                                             \
    AS_LIST_PUT_APPEND_BYTES(key, value, array, static_pool,                   \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_LIST_PUT_APPEND_BOOL(key, value, array, static_pool,         \
           serializer_policy, err)                                             \
    AS_LIST_PUT_APPEND_BYTES(key, value, array, static_pool,                   \
        serializer_policy, err TSRMLS_CC)

/*
 *******************************************************************************************************
 * PUT function calls for level = DEFAULT
 *******************************************************************************************************
 */
#define AEROSPIKE_DEFAULT_PUT_ASSOC_NULL(key, value, array, static_pool,       \
           serializer_policy, err)                                             \
    AS_DEFAULT_PUT_ASSOC_BYTES(key, value, array, static_pool,                 \
                    serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_PUT_ASSOC_LONG(key, value, array, static_pool,       \
            serializer_policy, err)                                            \
    AS_DEFAULT_PUT_ASSOC_INT64(key, value, array, static_pool,                 \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_PUT_ASSOC_STRING(key, value, array, static_pool,     \
           serializer_policy, err)                                             \
    AS_DEFAULT_PUT_ASSOC_STR(key, value, array, static_pool,                   \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_PUT_ASSOC_ARRAY(key, value, array, static_pool,      \
           serializer_policy, err)                                             \
    AS_DEFAULT_PUT_ASSOC_ARRAY(key, value, array, static_pool,                 \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_PUT_ASSOC_LIST(key, value, array, static_pool,       \
            serializer_policy, err)                                            \
    AS_DEFAULT_PUT_ASSOC_LIST(key, value, array, static_pool,                  \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_PUT_ASSOC_MAP(key, value, array, static_pool,        \
            serializer_policy, err)                                            \
    AS_DEFAULT_PUT_ASSOC_MAP(key, value, array, static_pool,                   \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_PUT_ASSOC_OBJECT(key, value, array, static_pool,     \
            serializer_policy, err)                                            \
    AS_DEFAULT_PUT_ASSOC_BYTES(key, value, array, static_pool,                 \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_PUT_ASSOC_DOUBLE(key, value, array, static_pool,     \
            serializer_policy, err)                                            \
    AS_DEFAULT_PUT_ASSOC_BYTES(key, value, array, static_pool,                 \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_PUT_ASSOC_BOOL(key, value, array, static_pool,       \
            serializer_policy, err)                                            \
    AS_DEFAULT_PUT_ASSOC_BYTES(key, value, array, static_pool,                 \
        serializer_policy, err TSRMLS_CC)

/*
 *******************************************************************************************************
 * PUT function calls for level = MAP
 *******************************************************************************************************
 */
#define AEROSPIKE_MAP_PUT_ASSOC_NULL(key, value, array, static_pool,           \
           serializer_policy, err)                                             \
    AS_MAP_PUT_ASSOC_BYTES(key, value, array, static_pool,                     \
            serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_MAP_PUT_ASSOC_LONG(key, value, array, static_pool,           \
           serializer_policy, err)                                             \
    AS_MAP_PUT_ASSOC_INT64(key, value, array, static_pool,                     \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_MAP_PUT_ASSOC_STRING(key, value, array, static_pool,         \
            erializer_policy, err)                                             \
    AS_MAP_PUT_ASSOC_STR(key, value, array, static_pool,                       \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_MAP_PUT_ASSOC_ARRAY(key, value, array, static_pool,          \
           serializer_policy, err)                                             \
    AS_MAP_PUT_ASSOC_ARRAY(key, value, array, static_pool,                     \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_MAP_PUT_ASSOC_LIST(key, value, array, static_pool,           \
           serializer_policy, err)                                             \
    AS_MAP_PUT_ASSOC_LIST(key, value, array, static_pool,                      \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_MAP_PUT_ASSOC_MAP(key, value, array, static_pool,            \
           serializer_policy, err)                                             \
    AS_MAP_PUT_ASSOC_MAP(key, value, array, static_pool,                       \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_MAP_PUT_ASSOC_OBJECT(key, value, array, static_pool,         \
           serializer_policy, err)                                             \
    AS_MAP_PUT_ASSOC_BYTES(key, value, array, static_pool,                     \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_MAP_PUT_ASSOC_DOUBLE(key, value, array, static_pool,         \
           serializer_policy, err)                                             \
    AS_MAP_PUT_ASSOC_BYTES(key, value, array, static_pool,                     \
        serializer_policy, err TSRMLS_CC)

#define AEROSPIKE_MAP_PUT_ASSOC_BOOL(key, value, array, static_pool,           \
           serializer_policy, err)                                             \
    AS_MAP_PUT_ASSOC_BYTES(key, value, array, static_pool,                     \
        serializer_policy, err TSRMLS_CC)

/*
 *******************************************************************************************************
 * GET function calls for level = LIST
 *******************************************************************************************************
 */
#define AEROSPIKE_LIST_GET_APPEND_UNDEF(key, value, array, static_pool, err)   \
    ADD_LIST_APPEND_NULL(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_LIST_GET_APPEND_UNKNOWN(key, value, array, static_pool, err) \
    ADD_LIST_APPEND_NULL(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_LIST_GET_APPEND_NIL(key, value, array, static_pool, err)     \
    ADD_LIST_APPEND_NULL(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_LIST_GET_APPEND_BOOLEAN(key, value, array, static_pool, err) \
    ADD_LIST_APPEND_BOOL(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_LIST_GET_APPEND_INTEGER(key, value, array, static_pool, err) \
    ADD_LIST_APPEND_LONG(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_LIST_GET_APPEND_STRING(key, value, array, static_pool, err)  \
    ADD_LIST_APPEND_STRING(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_LIST_GET_APPEND_LIST(key, value, array, static_pool, err)    \
    ADD_LIST_APPEND_LIST(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_LIST_GET_APPEND_MAP(key, value, array, static_pool, err)     \
    ADD_LIST_APPEND_MAP(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_LIST_GET_APPEND_REC(key, value, array, static_pool, err)     \
    ADD_LIST_APPEND_REC(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_LIST_GET_APPEND_PAIR(key, value, array, static_pool, err)    \
    ADD_LIST_APPEND_PAIR(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_LIST_GET_APPEND_BYTES(key, value, array, static_pool, err)   \
    ADD_LIST_APPEND_BYTES(key, value, &array, err TSRMLS_CC)

/*
 *******************************************************************************************************
 * GET function calls for level = DEFAULT
 *******************************************************************************************************
 */
#define AEROSPIKE_DEFAULT_GET_ASSOC_UNDEF(key, value, array, static_pool,      \
        err)                                                                   \
    ADD_DEFAULT_ASSOC_NULL(key, value, array, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_GET_ASSOC_UNKNOWN(key, value, array, static_pool,    \
        err)                                                                   \
    ADD_DEFAULT_ASSOC_NULL(key, value, array, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_GET_ASSOC_NIL(key, value, array, static_pool, err)   \
    ADD_DEFAULT_ASSOC_NULL(key, value, array, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_GET_ASSOC_BOOLEAN(key, value, array, static_pool,    \
        err)                                                                   \
    ADD_DEFAULT_ASSOC_BOOL(key, value, array, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_GET_ASSOC_INTEGER(key, value, array, static_pool,    \
        err)                                                                   \
    ADD_DEFAULT_ASSOC_LONG(key, value, array, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_GET_ASSOC_STRING(key, value, array, static_pool,     \
        err)                                                                   \
    ADD_DEFAULT_ASSOC_STRING(key, value, array, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_GET_ASSOC_LIST(key, value, array, static_pool, err)  \
    ADD_DEFAULT_ASSOC_LIST(key, value, array, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_GET_ASSOC_MAP(key, value, array, static_pool,        \
        err)                                                                   \
    ADD_DEFAULT_ASSOC_MAP(key, value, array, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_GET_ASSOC_REC(key, value, array, static_pool, err)   \
    ADD_DEFAULT_ASSOC_REC(key, value, array, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_GET_ASSOC_PAIR(key, value, array, static_pool, err)  \
    ADD_DEFAULT_ASSOC_PAIR(key, value, array, err TSRMLS_CC)

#define AEROSPIKE_DEFAULT_GET_ASSOC_BYTES(key, value, array, static_pool,      \
        err)                                                                   \
    ADD_DEFAULT_ASSOC_BYTES(key, value, array, err TSRMLS_CC)

/*
 *******************************************************************************************************
 * GET function calls for level = MAP with string key
 *******************************************************************************************************
 */
#define   AEROSPIKE_MAP_GET_ASSOC_UNDEF(key, value, array, static_pool, err)   \
    ADD_MAP_ASSOC_NULL(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_ASSOC_UNKNOWN(key, value, array, static_pool, err)   \
    ADD_MAP_ASSOC_NULL(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_ASSOC_NIL(key, value, array, static_pool, err)       \
    ADD_MAP_ASSOC_NULL(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_ASSOC_BOOLEAN(key, value, array, static_pool, err)   \
    ADD_MAP_ASSOC_BOOL(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_ASSOC_INTEGER(key, value, array, static_pool, err)   \
    ADD_MAP_ASSOC_LONG(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_ASSOC_STRING(key, value, array, static_pool, err)    \
    ADD_MAP_ASSOC_STRING(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_ASSOC_LIST(key, value, array, static_pool, err)      \
    ADD_MAP_ASSOC_LIST(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_ASSOC_MAP(key, value, array, static_pool, err)       \
    ADD_MAP_ASSOC_MAP(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_ASSOC_REC(key, value, array, static_pool, err)       \
    ADD_MAP_ASSOC_REC(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_ASSOC_PAIR(key, value, array, static_pool, err)      \
    ADD_MAP_ASSOC_PAIR(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_ASSOC_BYTES(key, value, array, static_pool, err)     \
    ADD_MAP_ASSOC_BYTES(key, value, &array, err TSRMLS_CC)

/*
 *******************************************************************************************************
 * GET function calls for level = MAP with integer key
 *******************************************************************************************************
 */
#define AEROSPIKE_MAP_GET_INDEX_UNDEF(key, value, array, static_pool,          \
        err)                                                                   \
    ADD_MAP_INDEX_NULL(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_INDEX_UNKNOWN(key, value, array, static_pool,        \
        err)                                                                   \
    ADD_MAP_INDEX_NULL(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_INDEX_NIL(key, value, array, static_pool,            \
        err)                                                                   \
    ADD_MAP_INDEX_NULL(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_INDEX_BOOLEAN(key, value, array, static_pool,        \
        err)                                                                   \
    ADD_MAP_INDEX_BOOL(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_INDEX_INTEGER(key, value, array, static_pool,        \
        err)                                                                   \
    ADD_MAP_INDEX_LONG(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_INDEX_STRING(key, value, array, static_pool,         \
        err)                                                                   \
    ADD_MAP_INDEX_STRING(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_INDEX_LIST(key, value, array, static_pool,           \
        err)                                                                   \
    ADD_MAP_INDEX_LIST(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_INDEX_MAP(key, value, array, static_pool,            \
        err)                                                                   \
    ADD_MAP_INDEX_MAP(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_INDEX_REC(key, value, array, static_pool,            \
        err)                                                                   \
    ADD_MAP_INDEX_REC(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_INDEX_PAIR(key, value, array, static_pool,           \
        err)                                                                   \
    ADD_MAP_INDEX_PAIR(key, value, &array, err TSRMLS_CC)

#define AEROSPIKE_MAP_GET_INDEX_BYTES(key, value, array, static_pool,          \
        err)                                                                   \
    ADD_MAP_INDEX_BYTES(key, value, &array, err TSRMLS_CC)

/*
 *******************************************************************************************************
 * Macros for ZVAL processing at different levels
 *******************************************************************************************************
 */
#define ADD_MAP_ASSOC_ZVAL(array, key, store)                                  \
    add_assoc_zval(array, as_string_get((as_string *) key), store);

#define ADD_MAP_INDEX_ZVAL(array, key, store)                                  \
    add_index_zval(array, as_integer_get((as_integer *) key), store);

/*
 * key will be NULL in case of UDF methods.
 * NULL will differentiate UDF from normal GET calls.
 */
#define ADD_DEFAULT_ASSOC_ZVAL(array, key, store)                              \
    if (key == NULL) {                                                         \
        ZVAL_ZVAL((zval *) array, store, 1, 1);                                \
    } else {                                                                   \
        add_assoc_zval((zval *) array, key, store);                            \
    }

#define ADD_LIST_APPEND_ZVAL(array, key, store)                                \
    add_next_index_zval(array, store);

#endif /* end of __AERROSPIKE_TRANSFORM_H__ */

