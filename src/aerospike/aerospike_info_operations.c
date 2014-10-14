#include "php.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_error.h"
#include "aerospike/as_record.h"

#include "aerospike_common.h"
#include "aerospike_policy.h"

extern as_status
aerospike_info_specific_host(aerospike* as_object_p,
        as_error* error_p, char* request_str_p,
        zval* response_str_p, zval* host, zval* options_p TSRMLS_DC)
{
    as_policy_info              info_policy;
    zval**                      host_name = NULL;
    zval**                      port = NULL;
    char*                       address = (char *) as_object_p->config.hosts[0].addr;
    long                        port_no = as_object_p->config.hosts[0].port;
    char*                       response_p = NULL;

    set_policy(NULL, NULL, NULL, NULL, &info_policy, NULL, NULL, NULL,
            options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }
    
    if (host) {
        if (FAILURE == zend_hash_find(Z_ARRVAL_P(host), PHP_AS_KEY_DEFINE_FOR_ADDR,
                PHP_AS_KEY_DEFINE_FOR_ADDR_LEN + 1, (void**)&host_name)) {
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
                    "Unable to find addr");
            DEBUG_PHP_EXT_DEBUG("Unable to find addr");
            goto exit;
        }
        if (FAILURE == zend_hash_find(Z_ARRVAL_P(host), PHP_AS_KEY_DEFINE_FOR_PORT,
               PHP_AS_KEY_DEFINE_FOR_PORT_LEN + 1,  (void**)&port)) {
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
                    "Unable to find port");
            DEBUG_PHP_EXT_DEBUG("Unable to find port");
            goto exit;
        }
        if ((Z_TYPE_PP(host_name) != IS_STRING) || (Z_TYPE_PP(port) != IS_LONG)) {
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
                    "Host parameters are not correct");
            DEBUG_PHP_EXT_DEBUG("Host parameters are not correct");
            goto exit;
        }
        address = Z_STRVAL_PP(host_name);
        port_no = Z_LVAL_PP(port);
    }/* else {
        address = (char *) as_object_p->config.hosts[0].addr;
    }*/

    if (AEROSPIKE_OK != aerospike_info_host(as_object_p, error_p, &info_policy,
                (const char *) address, (uint16_t) port_no, request_str_p, &response_p)) {
            DEBUG_PHP_EXT_DEBUG(error_p->message);
            goto exit;
    }

    if (response_p != NULL) {
        ZVAL_STRINGL(response_str_p, response_p, strlen(response_p), 1);
    } else {
        ZVAL_STRINGL(response_str_p, "", 0, 1);
        //ZVAL_EMPTY_STRING(response_str_p);
    }

exit:
    return error_p->code;
}

/*extern as_status
aerospke_info_request_multiple_nodes(aerospike* as_onbject_p,
        as_error* error_p, char* request_str_p, zval* config_p,
        zval* return_value_p)
{

}*/
