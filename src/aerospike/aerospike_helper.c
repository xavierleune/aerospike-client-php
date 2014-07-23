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
    if (level & 0x08) {
        char msg[1024] = {0};
        va_list ap;

        va_start(ap, fmt);
        vsnprintf(msg, 1024, fmt, ap);
        msg[1023] = '\0';
        va_end(ap);
        if (!is_callback_registered) { 
            fprintf(stderr, "PHP EXTn: level %d func %s file %s line %d msg %s \n", level, func, file, line, msg);
	}
    }

    if (is_callback_registered) { 
        int16_t   iter = 0;
        zval**    params[4];
        zval*     z_func = NULL;
        zval*     z_file = NULL;
        zval*     z_line = NULL; 
        zval*     z_level = NULL;
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
   
        if (zend_call_function(&func_call_info, &func_call_info_cache TSRMLS_CC) == SUCCESS && 
            func_call_info.retval_ptr_ptr && *func_call_info.retval_ptr_ptr) {
                //TODO: COPY_PZVAL_TO_ZVAL(*return_value, *func_call_info.retval_ptr_ptr);
        } else {
                // TODO: Handle failure in zend_call_function
        }

        for (iter = 0; iter < 4; iter++) {
            zval_ptr_dtor(params[iter]);
        }
        //zval_ptr_dtor(&z_file);
        //zval_ptr_dtor(&z_line);
        //zval_ptr_dtor(&z_level);
    }

    return true;
}

extern int parseLogParameters(as_log* as_log_p)
{
    if (as_log_set_callback(as_log_p, &aerospike_helper_log_callback)) {
	is_callback_registered = 1;
        Z_ADDREF_P(func_call_info.function_name);
        return 1;
    } else {	
        return 0;;
    }	
}

extern void 
aerospike_helper_set_error(zend_class_entry *ce_p, zval *object_p, as_error *error_p, bool reset_flag TSRMLS_DC)
{
    zval*    err_code_p = NULL;
    zval*    err_msg_p = NULL;

    MAKE_STD_ZVAL(err_code_p);
    MAKE_STD_ZVAL(err_msg_p);

    if (reset_flag) {
        ZVAL_STRINGL(err_msg_p, DEFAULT_ERROR, strlen(DEFAULT_ERROR), 1);
        ZVAL_LONG(err_code_p, DEFAULT_ERRORNO);
    } else {
        ZVAL_STRINGL(err_msg_p, error_p->message, strlen(error_p->message), 1);
        ZVAL_LONG(err_code_p, error_p->code);
    }

    zend_update_property(ce_p, object_p, "error", strlen("error"), err_msg_p TSRMLS_DC);
    zend_update_property(ce_p, object_p, "errorno", strlen("errorno"), err_code_p TSRMLS_DC);

    zval_ptr_dtor(&err_code_p);
    zval_ptr_dtor(&err_msg_p);
}
 
