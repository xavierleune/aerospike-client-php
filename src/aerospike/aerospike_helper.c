#include "php.h"
#include "aerospike/as_log.h"
#include "aerospike/as_key.h"
#include "aerospike/as_config.h"
#include "aerospike/as_error.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike.h"

#include "aerospike_common.h"

zend_fcall_info       func_call_info;
zend_fcall_info_cache func_call_info_cache;
zval                  *func_callback_retval_p;

as_log_level   php_log_level_set = AS_LOG_LEVEL_DEBUG;

extern bool 
aerospike_helper_log_callback(as_log_level level, const char * func, const char * file, uint32_t line, const char * fmt, ...)
{
    zval **params[4];
    zval *z_func;
    zval *z_file;
    zval *z_line; 
    zval *z_level;
    func_callback_retval_p = NULL;

    ALLOC_INIT_ZVAL(z_level);
    ZVAL_LONG(z_level, level);
    params[0] = &z_level;

    ALLOC_INIT_ZVAL(z_func);
    ZVAL_STRING(z_func, func, 1);
    params[1] = &z_func;

    ALLOC_INIT_ZVAL(z_file);
    ZVAL_STRING(z_file, file, 1);
    params[2] = &z_file;

    ALLOC_INIT_ZVAL(z_line);
    ZVAL_LONG(z_line, line);
    params[3] = &z_line;

    func_call_info.param_count = 4;
    func_call_info.params = params;
    func_call_info.retval_ptr_ptr = &func_callback_retval_p;

#if 0
    if (zend_call_function(&func_call_info, &func_call_info_cache TSRMLS_CC) == SUCCESS && func_call_info.retval_ptr_ptr && *func_call_info.retval_ptr_ptr) {
        //COPY_PZVAL_TO_ZVAL(*return_value, *func_call_info.retval_ptr_ptr);
    } else {
#else
        fprintf(stderr, "Logging error: level %d func %s file %s line %d msg %s", level, func, file, line, fmt);
#endif
//    }

    zval_ptr_dtor(&z_func);
    zval_ptr_dtor(&z_file);
    zval_ptr_dtor(&z_line);
    zval_ptr_dtor(&z_level);

    return true;
}

