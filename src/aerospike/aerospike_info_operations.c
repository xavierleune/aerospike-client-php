/*
 *
 * Copyright (C) 2014-2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#include "php.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_error.h"
#include "aerospike/as_record.h"
#include "aerospike/aerospike_info.h"
#include <arpa/inet.h>

#include "aerospike_common.h"
#include "aerospike_policy.h"

#define MAX_HOST_COUNT 128
#define INFO_REQUEST_RESPONSE_DELIMITER "\t"
#define INFO_RESPONSE_END "\n"
#define HOST_DELIMITER ";"
#define IP_PORT_DELIMITER ":"

/*
 *******************************************************************************************************
 * Wrapper function to perform an aerospike_info_host within the C client.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param request_str_p         The request string for info.
 * @param response_str_p        The response zval to be initialized as a string and populated with the
 *                              actual info response by this method.
 * @param host                  The optional host array containing a single addr and port.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_info_specific_host(aerospike* as_object_p,
		as_error* error_p, char* request_str_p,
		zval* response_str_p, zval* host, zval* options_p TSRMLS_DC)
{
	as_status                   status = AEROSPIKE_OK;
	as_policy_info              info_policy;
#if PHP_VERSION_ID < 70000
	zval**                      host_name = NULL;
	zval**                      port = NULL;
#else
	zval*                       host_name = NULL;
	zval*                       port = NULL;
#endif
	char*                       address = (char *) as_object_p->config.hosts[0].addr;
	long                        port_no = as_object_p->config.hosts[0].port;
	char*                       response_p = NULL;

	set_policy(&as_object_p->config, NULL, NULL, NULL, NULL, &info_policy, NULL, NULL, NULL,
			options_p, error_p TSRMLS_CC);

	if (AEROSPIKE_OK != (error_p->code)) {
		DEBUG_PHP_EXT_DEBUG("Unable to set policy");
		goto exit;
	}
	
	if (host) {
#if PHP_VERSION_ID < 70000
		if (FAILURE == AEROSPIKE_ZEND_HASH_FIND(Z_ARRVAL_P(host),
					PHP_AS_KEY_DEFINE_FOR_ADDR,
				PHP_AS_KEY_DEFINE_FOR_ADDR_LEN + 1, (void**)&host_name))
#else
		if (NULL == (host_name = AEROSPIKE_ZEND_HASH_FIND(Z_ARRVAL_P(host),
					PHP_AS_KEY_DEFINE_FOR_ADDR,
				PHP_AS_KEY_DEFINE_FOR_ADDR_LEN + 1, (void**)&host_name)))
#endif
		{
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
					"Unable to find addr");
			DEBUG_PHP_EXT_DEBUG("Unable to find addr");
			goto exit;
		}
#if PHP_VERSION_ID < 70000
		if (FAILURE == AEROSPIKE_ZEND_HASH_FIND(Z_ARRVAL_P(host),
					PHP_AS_KEY_DEFINE_FOR_PORT,
					PHP_AS_KEY_DEFINE_FOR_PORT_LEN + 1,  (void**)&port))
#else
		if (NULL == (port = AEROSPIKE_ZEND_HASH_FIND(Z_ARRVAL_P(host),
					PHP_AS_KEY_DEFINE_FOR_PORT,
					PHP_AS_KEY_DEFINE_FOR_PORT_LEN + 1,  (void**)&port)))
#endif
		{
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
					"Unable to find port");
			DEBUG_PHP_EXT_DEBUG("Unable to find port");
			goto exit;
		}
#if PHP_VERSION_ID < 70000
		if ((Z_TYPE_PP(host_name) != IS_STRING) || (Z_TYPE_PP(port) != IS_LONG))
#else
		if ((Z_TYPE_P(host_name) != IS_STRING) || (Z_TYPE_P(port) != IS_LONG))
#endif
		{
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM,
					"Host parameters are not correct");
			DEBUG_PHP_EXT_DEBUG("Host parameters are not correct");
			goto exit;
		}

#if PHP_VERSION_ID < 70000
		address = Z_STRVAL_PP(host_name);
		port_no = Z_LVAL_PP(port);
#else
		address = Z_STRVAL_P(host_name);
		port_no = Z_LVAL_P(port);
#endif
	}

	if (AEROSPIKE_OK !=
			(status = aerospike_info_host(as_object_p, error_p, &info_policy,
				(const char *) address, (uint16_t) port_no, request_str_p,
				&response_p))) {
			DEBUG_PHP_EXT_DEBUG("%s", error_p->message);
			goto exit;
	}

	if (response_p != NULL) {
		AEROSPIKE_ZVAL_STRINGL(response_str_p, response_p, strlen(response_p), 1);
		free(response_p);
		response_p = NULL;
	} else {
		AEROSPIKE_ZVAL_STRINGL(response_str_p, "", 0, 1);
	}

exit:
	return status;
}

/*
 *******************************************************************************************************
 * Helper function to get all nodes within the cluster.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param return_p              The return zval to be populated with the
 *                              node ip:port pairs by this method.
 * @param host                  The optional host array containing a single addr and port.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_info_get_cluster_nodes(aerospike* as_object_p,
		as_error* error_p, zval* return_p, zval* host, zval* options_p TSRMLS_DC)
{
	as_policy_info              info_policy;
#if PHP_VERSION_ID < 70000
	zval*                       response_services_p = NULL;
	zval*                       response_service_p = NULL;
	zval*                       current_host_p[MAX_HOST_COUNT] = {0};
#else
	zval                        response_services_p;
	zval                        response_service_p;
	zval                        current_host_p[MAX_HOST_COUNT];
#endif
	char*                       tok = NULL;
	char*                       saved = NULL;
	uint32_t                    host_index = 0;
	uint32_t                    host_index_iter = 0;
	bool                        iter_first = true;
	bool                        break_flag = false;

#if PHP_VERSION_ID < 70000
	MAKE_STD_ZVAL(response_services_p);
	MAKE_STD_ZVAL(response_service_p);
#else
	array_init(&response_services_p);
	array_init(&response_service_p);
#endif

#if PHP_VERSION_ID < 70000
	if (AEROSPIKE_OK !=
			aerospike_info_specific_host(as_object_p, error_p,
					"services", response_services_p, host, options_p TSRMLS_CC))
#else
	if (AEROSPIKE_OK !=
			aerospike_info_specific_host(as_object_p, error_p,
					"services", &response_services_p, host, options_p TSRMLS_CC))
#endif
	{
		DEBUG_PHP_EXT_ERROR("getNodes: services call returned an error");
		goto exit;
	}

#if PHP_VERSION_ID < 70000
	if (AEROSPIKE_OK !=
			aerospike_info_specific_host(as_object_p, error_p,
					"service", response_service_p, host, options_p TSRMLS_CC))
#else
	if (AEROSPIKE_OK !=
			aerospike_info_specific_host(as_object_p, error_p,
					"service", &response_service_p, host, options_p TSRMLS_CC))
#endif
	{
		DEBUG_PHP_EXT_ERROR("getNodes: service call returned an error");
		goto exit;
	}

	host_index = 0;

#if PHP_VERSION_ID < 70000
	MAKE_STD_ZVAL(current_host_p[host_index]);
	array_init(current_host_p[host_index]);
#else
	array_init(&current_host_p[host_index]);
#endif

#if PHP_VERSION_ID < 70000
	if (0 != add_next_index_zval(return_p, current_host_p[host_index]))
#else
	if (0 != add_next_index_zval(return_p, &current_host_p[host_index]))
#endif
	{
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
				"Unable to get addr-port");
		DEBUG_PHP_EXT_DEBUG("Unable to get addr-port");
		goto exit;
	}

#if PHP_VERSION_ID < 70000
	tok = strtok_r(Z_STRVAL_P(response_service_p), INFO_REQUEST_RESPONSE_DELIMITER, &saved);
#else
	tok = strtok_r(Z_STRVAL_P(&response_service_p), INFO_REQUEST_RESPONSE_DELIMITER, &saved);
#endif
	if (tok == NULL) {
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
			"Unable to get addr in service");
		DEBUG_PHP_EXT_DEBUG("Unable to get addr in service");
		goto exit;
	}

	tok = strtok_r(NULL, IP_PORT_DELIMITER, &saved);
	if (tok == NULL) {
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
			"Unable to get addr in service");
		DEBUG_PHP_EXT_DEBUG("Unable to get addr in service");
		goto exit;
	}

#if PHP_VERSION_ID < 70000
	if (0 != AEROSPIKE_ADD_ASSOC_STRINGL(current_host_p[host_index], "addr", tok, strlen(tok), 1))
#else
	if (0 != AEROSPIKE_ADD_ASSOC_STRINGL(&current_host_p[host_index], "addr", tok, strlen(tok), 1))
#endif
	{
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
			"Unable to get addr");
		DEBUG_PHP_EXT_DEBUG("Unable to get addr");
		goto exit;
	}

	tok = strtok_r(NULL, INFO_RESPONSE_END, &saved);
	if (tok == NULL) {
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
			"Unable to get port in service");
		DEBUG_PHP_EXT_DEBUG("Unable to get port in service");
		goto exit;
	}

#if PHP_VERSION_ID < 70000
	if (0 != AEROSPIKE_ADD_ASSOC_STRINGL(current_host_p[host_index], "port", tok, strlen(tok), 1))
#else
	if (0 != AEROSPIKE_ADD_ASSOC_STRINGL(&current_host_p[host_index], "port", tok, strlen(tok), 1))
#endif
	{
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
				"Unable to get port");
		DEBUG_PHP_EXT_DEBUG("Unable to get port");
		goto exit;
	}

	host_index++;
#if PHP_VERSION_ID < 70000
	tok = strtok_r(Z_STRVAL_P(response_services_p), INFO_REQUEST_RESPONSE_DELIMITER, &saved);
#else
	tok = strtok_r(Z_STRVAL_P(&response_services_p), INFO_REQUEST_RESPONSE_DELIMITER, &saved);
#endif

	while (tok != NULL) {
		tok = strtok_r(NULL, IP_PORT_DELIMITER, &saved);
		if (tok == NULL) {
			goto exit;
		}

#if PHP_VERSION_ID < 70000
		MAKE_STD_ZVAL(current_host_p[host_index]);
		array_init(current_host_p[host_index]);
#else
		array_init(&current_host_p[host_index]);
#endif

#if PHP_VERSION_ID < 70000
		if (0 != add_next_index_zval(return_p, current_host_p[host_index]))
#else
		if (0 != add_next_index_zval(return_p, &current_host_p[host_index]))
#endif
		{
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
					"Unable to get addr-port");
			DEBUG_PHP_EXT_DEBUG("Unable to get addr-port");
			goto exit;
		}

#if PHP_VERSION_ID < 70000
		if (0 != AEROSPIKE_ADD_ASSOC_STRINGL(current_host_p[host_index], "addr", tok, strlen(tok), 1))
#else
		if (0 != AEROSPIKE_ADD_ASSOC_STRINGL(&current_host_p[host_index], "addr", tok, strlen(tok), 1))
#endif
		{
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
					"Unable to get addr");
			DEBUG_PHP_EXT_DEBUG("Unable to get addr");
			goto exit;
		}

		tok = strtok_r(NULL, HOST_DELIMITER, &saved);
		if (tok == NULL) {
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
					"Unable to get port");
			DEBUG_PHP_EXT_DEBUG("Unable to get port");
			goto exit;
		}

		if (strstr(tok, INFO_RESPONSE_END)) {
			tok = strtok_r(tok, INFO_RESPONSE_END, &saved);
			break_flag = true;
		}

#if PHP_VERSION_ID < 70000
		if (0 != AEROSPIKE_ADD_ASSOC_STRINGL(current_host_p[host_index], "port", tok, strlen(tok), 1))
#else
		if (0 != AEROSPIKE_ADD_ASSOC_STRINGL(&current_host_p[host_index], "port", tok, strlen(tok), 1))
#endif
		{
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
					"Unable to get port");
			DEBUG_PHP_EXT_DEBUG("Unable to get port");
			goto exit;
		}

		host_index++;

		if (break_flag == true) {
			goto exit;
		}
	}

exit:
#if PHP_VERSION_ID < 70000
	if (response_services_p) {
		zval_ptr_dtor(&response_services_p);
	}
#else
	zval_ptr_dtor(&response_services_p);
#endif

#if PHP_VERSION_ID < 70000
	if (response_service_p) {
		zval_ptr_dtor(&response_service_p);
	}
#else
	zval_ptr_dtor(&response_service_p);
#endif
	return error_p->code;
}

/*
 *******************************************************************************************************
 * Helper function to convert struct sockaddr address to a string, IPv4 and IPv6.
 * Currently unused. Can use if as_address supports IPv6.
 *
 * @param sa                  The sockaddr address to be converted into a string.
 *******************************************************************************************************
 */
static char *get_ip_str(const struct sockaddr *sa)
{
	int         maxlen = INET6_ADDRSTRLEN;
	char*       s = NULL;

	if (!sa) {
		strncpy(s, "Invalid sockaddr_in", maxlen);
		return NULL;
	}

	switch(sa->sa_family) {
		case AF_INET:
			if (NULL == (s = (char *) malloc(INET_ADDRSTRLEN))) {
				return NULL;
			}
			inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
					s, INET_ADDRSTRLEN);
			break;

		case AF_INET6:
			if (NULL == (s = (char *) malloc(INET6_ADDRSTRLEN))) {
				return NULL;
			}
			inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
					s, INET6_ADDRSTRLEN);
			break;

		default:
			if (NULL == (s = (char *) malloc(maxlen))) {
				return NULL;
			}
			strncpy(s, "Unknown AF", maxlen);
			return NULL;
	}

	return s;
}

/*
 *******************************************************************************************************
 * Callback for as_info_foreach.
 * It processes the as_val and translates it into an equivalent zval array.
 * It then calls the user registered callback passing the zval array as an
 * argument.
 *       
 * @param err               The as_error object sent by C client to this
 *                          callback.
 * @param node              The current as_node object for which the callback is
 *                          fired by C client.
 * @param request           The info request string.
 * @param response          The info response string for current node.
 * @param udata             The callback udata containing the host_lookup array
 *                          and the return zval to be populated with an entry
 *                          for current node's info response with the node's ID
 *                          as the key.
 *
 * @return true if callback is successful; else false.
 *******************************************************************************************************
 */

extern bool
aerospike_info_callback(const as_error* err, const as_node* node,
		char* request, char* response, void* udata) 
{
	TSRMLS_FETCH();
	foreach_callback_info_udata*        udata_ptr = (foreach_callback_info_udata *) udata;
	struct sockaddr_in*                 addr = NULL;

	if (!response) {
		goto exit;
	}

	if (node && response) {
		if (udata_ptr->host_lookup_p) {
			addr = as_node_get_address((as_node *)node);
			char ip_port[IP_PORT_MAX_LEN];
			snprintf(ip_port, IP_PORT_MAX_LEN, "%s:%d", inet_ntop(addr->sin_family,
						&(addr->sin_addr), ip_port, INET_ADDRSTRLEN),
					ntohs(addr->sin_port));

#if PHP_VERSION_ID < 70000
			zval **tmp;
			if (SUCCESS == AEROSPIKE_ZEND_HASH_FIND(udata_ptr->host_lookup_p, ip_port,
					strlen(ip_port), (void**)&tmp))
#else
			zval *tmp;
			if (NULL == (tmp = AEROSPIKE_ZEND_HASH_FIND(udata_ptr->host_lookup_p, ip_port,
					strlen(ip_port), (void**)&tmp)))
#endif
			{
				if (0 != AEROSPIKE_ADD_ASSOC_STRINGL(udata_ptr->udata_p, node->name,
							response, strlen(response), 1)) {
					DEBUG_PHP_EXT_DEBUG("Unable to get node info");
					goto exit;
				}
			}
		} else {
			if (0 != AEROSPIKE_ADD_ASSOC_STRINGL(udata_ptr->udata_p, node->name,
						response, strlen(response), 1)) {
				DEBUG_PHP_EXT_DEBUG("Unable to get node info");
				goto exit;
			}	
		}
	} else {
		return false;
	}
exit:
	return true;
}

#if PHP_VERSION_ID < 70000
static void my_hashtable_dtor(void *p) {
	/*
	 * Custom dtor for hashtable
	 */
	return;
}
#else
static void my_hashtable_dtor(zval *p) {
	/*
	 * Custom dtor for hashtable
	 */
	return;
}
#endif

/*
 *******************************************************************************************************
 * Send an info request to multiple cluster nodes.
 *
 * @param as_object_p               The C client's aerospike object.
 * @param error_p                   The C client's as_error to be set to the
 *                                  encountered error.
 * @param request_str_p             request string
 * @config_p                        Config filter array.
 * @return_value_p                  An array of response strings keyed by the
 *                                  cluster node ID.
 * @param options_p                 Options array with read timeout.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_info_request_multiple_nodes(aerospike* as_object_p,
		as_error* error_p, char* request_str_p, zval* config_p,
		zval* return_value_p, zval* options_p TSRMLS_DC)
{
	foreach_callback_info_udata     info_callback_udata;
	as_policy_info                  info_policy;
	HashTable*                      host_lookup_p = NULL;

	if ((!request_str_p)) {
		PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
				"Request string is empty");
		DEBUG_PHP_EXT_DEBUG("Request string is empty");
		goto exit;
	}

	set_policy(&as_object_p->config, NULL, NULL, NULL, NULL,
			&info_policy, NULL, NULL, NULL, options_p, error_p TSRMLS_CC);

	if (AEROSPIKE_OK != (error_p->code)) {
		DEBUG_PHP_EXT_DEBUG("Unable to set policy");
		goto exit;
	}

	if (config_p) {
		if (NULL == (host_lookup_p = emalloc(sizeof(HashTable)))) {
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
					"Unable to allocate memory for host lookup");
			DEBUG_PHP_EXT_DEBUG("Unable to allocate memory for host lookup");
			goto exit;
		}
		zend_hash_init(host_lookup_p, MAX_HOST_COUNT, NULL, &my_hashtable_dtor, 0);

		transform_zval_config_into transform_zval_config_into_zval;
		transform_zval_config_into_zval.transform_result.host_lookup_p = host_lookup_p;
		transform_zval_config_into_zval.transform_result_type = TRANSFORM_INTO_ZVAL;
		memset( transform_zval_config_into_zval.user, '\0', AS_USER_SIZE);
		memset( transform_zval_config_into_zval.pass, '\0', AS_PASSWORD_HASH_SIZE );

		if (AEROSPIKE_OK !=
				(error_p->code =
				 aerospike_transform_check_and_set_config(Z_ARRVAL_P(config_p),
					 NULL, &transform_zval_config_into_zval))) {
			DEBUG_PHP_EXT_DEBUG("Unable to create host lookup");
			goto exit;
		}
	}

	info_callback_udata.udata_p = return_value_p;
	info_callback_udata.host_lookup_p = host_lookup_p;

	if (AEROSPIKE_OK != aerospike_info_foreach(as_object_p, error_p,
				&info_policy, request_str_p,
				(aerospike_info_foreach_callback) aerospike_info_callback,
				&info_callback_udata)) {
		DEBUG_PHP_EXT_DEBUG("%s", error_p->message);
		goto exit;
	}

exit:
	if (host_lookup_p) {
		zend_hash_clean(host_lookup_p);
		zend_hash_destroy(host_lookup_p);
		efree(host_lookup_p);
	}
	return error_p->code;
}
