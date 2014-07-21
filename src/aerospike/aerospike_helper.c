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
uint32_t              is_callback_registered;

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

    char msg[1024] = {0};
    va_list ap;

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
    
    va_start(ap, fmt);
    vsnprintf(msg, 1024, fmt, ap);
    msg[1023] = '\0';
    va_end(ap);

    if (level & 0x08) {
        if (!is_callback_registered) { 
                fprintf(stderr, "Logging error: level %d func %s file %s line %d msg %s", level, func, file, line, fmt);
        } else {
            if (zend_call_function(&func_call_info, &func_call_info_cache TSRMLS_CC) == SUCCESS && func_call_info.retval_ptr_ptr && *func_call_info.retval_ptr_ptr) {
                //TODO: COPY_PZVAL_TO_ZVAL(*return_value, *func_call_info.retval_ptr_ptr);
            } else {
                // TODO: Handle failure in zend_call_function
            }
	}
    } 

    zval_ptr_dtor(&z_func);
    zval_ptr_dtor(&z_file);
    zval_ptr_dtor(&z_line);
    zval_ptr_dtor(&z_level);

    return true;
}

extern int parseLogParameters(as_log* as_log_p)
{
    /*if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f*",
                            &func_call_info, &func_call_info_cache,
                            &func_call_info.params, &func_call_info.param_count) == FAILURE) {    
	DEBUG_PHP_EXT_ERROR("invalid aerospike object");
        return 0;
    }*/
    if (as_log_set_callback(as_log_p, &aerospike_helper_log_callback)) {
	is_callback_registered = 1;
        Z_ADDREF_P(func_call_info.function_name);
        return 1;
    
    } else {	
        return 0;;
    }	
}

    
