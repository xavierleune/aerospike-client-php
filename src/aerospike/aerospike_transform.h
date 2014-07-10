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

#define PREFIX_GET AS
#define PREFIX_PUT IS

#define FETCH_VALUE_GET(val) as_val_type(val)
#define FETCH_VALUE_PUT(val) Z_TYPE_PP(val)

#define EXPAND_CASE(level, method, action, datatype, key, value,        \
                            array, err, static_pool, label)             \
        case PREFIX_##method_##datatype:                                \
            if (AEROSPIKE_OK != (err.code =                             \
                        AEROSPIKE_##level_##method_##action_##datatype( \
                    key, value, array, static_pool))) {                 \
                goto label;                                             \
            }                                                           \
            break;



#define AEROSPIKE_WALKER_SWITCH_CASE(value, method, level,              \
        action, err, static_pool, key, value, array, label)             \
            AEROSPIKE_WALKER_SWITCH_CASE_##method(value, method,        \
                level, action, err, static_pool, key, value, array,     \
                    label)

#define AEROSPIKE_WALKER_SWITCH_CASE_GET(value, method, level, action,  \
        err, static_pool, key, value, label)                            \
    switch (FETCH_VALUE_##method(value)) {                              \
        EXPAND_CASE(level, method, action, UNDEF, key, value,  \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, NIL, key, value,    \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, BOOLEAN, key, value,\
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, INTEGER, key, value,\
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, STRING, key, value, \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, LIST, key, value,   \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, MAP, key, value,    \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, REC, key, value,    \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, PAIR, key, value,   \
                array, err, static_pool, label)                         \
        EXPAND_CASE(level, method, action, BYTES, key, value,  \
                array, err, static_pool, label)                         \
        default:                                                        \
            err.code = AEROSPIKE_ERR_PARAM;                             \
            goto label;                                                 \
    }

#define AEROSPIKE_DEFAULT_PUT_APPEND_NULL(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_PUT_APPEND_LONG(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_PUT_APPEND_STRING(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_PUT_APPEND_ARRAY(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_PUT_APPEND_LIST(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_PUT_APPEND_MAP(key, value, array, static_pool)  

#define AEROSPIKE_LIST_PUT_APPEND_NULL(key, value, array, static_pool)  
#define AEROSPIKE_LIST_PUT_APPEND_LONG(key, value, array, static_pool)  
#define AEROSPIKE_LIST_PUT_APPEND_STRING(key, value, array, static_pool)  
#define AEROSPIKE_LIST_PUT_APPEND_ARRAY(key, value, array, static_pool)  
#define AEROSPIKE_LIST_PUT_APPEND_LIST(key, value, array, static_pool)  
#define AEROSPIKE_LIST_PUT_APPEND_MAP(key, value, array, static_pool)  

#define AEROSPIKE_DEFAULT_PUT_ASSOC_NULL(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_PUT_ASSOC_LONG(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_PUT_ASSOC_STRING(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_PUT_ASSOC_ARRAY(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_PUT_ASSOC_LIST(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_PUT_ASSOC_MAP(key, value, array, static_pool)  

#define AEROSPIKE_MAP_PUT_ASSOC_NULL(key, value, array, static_pool)  
#define AEROSPIKE_MAP_PUT_ASSOC_LONG(key, value, array, static_pool)  
#define AEROSPIKE_MAP_PUT_ASSOC_STRING(key, value, array, static_pool)  
#define AEROSPIKE_MAP_PUT_ASSOC_ARRAY(key, value, array, static_pool)  
#define AEROSPIKE_MAP_PUT_ASSOC_LIST(key, value, array, static_pool)  
#define AEROSPIKE_MAP_PUT_ASSOC_MAP(key, value, array, static_pool)  

#define AEROSPIKE_DEFAULT_GET_APPEND_UNDEF(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_APPEND_NIL(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_APPEND_BOOLEAN(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_APPEND_INTEGER(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_APPEND_STRING(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_APPEND_LIST(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_APPEND_MAP(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_APPEND_REC(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_APPEND_PAIR(key, value, array, static_pool)  
#define AEROSPIKE_DEFAULT_GET_APPEND_BYTES(key, value, array, static_pool) 

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

