#include "php.h"

#include "aerospike_common.h"

extern bool
aerospike_helper_log_callback(as_log_level level, const char * func, const char * file, uint32_t line, const char * fmt, ...)
{
    zval **params[4];
    zval *z_func, *z_file, *z_line, *z_level;

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

    if (zend_call_function(&func_call_info, &func_call_info_cache TSRMLS_CC) == SUCCESS && func_call_info.retval_ptr_ptr && *func_call_info.retval_ptr_ptr) {
        //COPY_PZVAL_TO_ZVAL(*return_value, *func_call_info.retval_ptr_ptr);
    } else {
        //fprintf(stderr, "%s");
    }

    zval_ptr_dtor(&z_func);
    zval_ptr_dtor(&z_file);
    zval_ptr_dtor(&z_line);
    zval_ptr_dtor(&z_level);

    return true;
}

