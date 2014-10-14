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
        zval* response_str_p, zval* host, zval* options_p)
{
    as_policy_info              info_policy;
    zval**                      host_name = NULL;
    zval**                      port = NULL;
    char*                       address = as_object_p->config.hosts[0].addr;
    long                        port_no = as_object_p->config.hosts[0].port;
    char*                       response_p = NULL;
    printf("addr and port is %s and  %ld\n", address, port_no);

    set_policy(NULL, NULL, NULL, NULL, &info_policy, NULL, NULL, NULL,
            options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }
    
    if (host) {
        if (FAILURE == zend_hash_find(Z_ARRVAL_P(host), PHP_AS_KEY_DEFINE_FOR_ADDR,
                PHP_AS_KEY_DEFINE_FOR_ADDR_LEN + 1, (void**)&host_name)) {
            error_p->code = AEROSPIKE_ERR_PARAM;
            DEBUG_PHP_EXT_DEBUG("Unable to find addr");
            goto exit;
        }
        if (FAILURE == zend_hash_find(Z_ARRVAL_P(host), PHP_AS_KEY_DEFINE_FOR_PORT,
               PHP_AS_KEY_DEFINE_FOR_PORT_LEN + 1,  (void**)&port)) {
            error_p->code = AEROSPIKE_ERR_PARAM;
            DEBUG_PHP_EXT_DEBUG("Unable to find port");
            goto exit;
        }
        if ((Z_TYPE_PP(host_name) != IS_STRING) || (Z_TYPE_PP(port) != IS_LONG)) {
            error_p->code = AEROSPIKE_ERR_PARAM;
            DEBUG_PHP_EXT_DEBUG("Host parameters are not correct");
            goto exit;
        }
        address = Z_STRVAL_PP(host_name);
        port_no = Z_LVAL_PP(port);
    }

    if (AEROSPIKE_OK != aerospike_info_host(as_object_p, error_p, &info_policy,
                address, port_no, request_str_p, &response_str_p)) {
            DEBUG_PHP_EXT_DEBUG(error_p->message);
            goto exit;
    }
    ZVAL_STRING(response_str_p, response_p, 1);
exit:
    return error_p->code;
}

/*extern as_status
aerospke_info_request_multiple_nodes(aerospike* as_onbject_p,
        as_error* error_p, char* request_str_p, zval* config_p,
        zval* return_value_p)
{

}*/
