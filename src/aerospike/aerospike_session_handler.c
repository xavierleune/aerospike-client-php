#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "php_ini.h"
#include "php_variables.h"
#include "php_aerospike.h"
#include "ext/session/php_session.h"
#include "aerospike/aerospike.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_config.h"
#include "aerospike_common.h"

#define AEROSPIKE_SESSION_BIN "PHP_SESSION"

extern int persist;

/*
 *******************************************************************************************************
 * Sesion handler structure instance for Aerospike.
 *******************************************************************************************************
 */
PS_FUNCS(aerospike);
ps_module ps_mod_aerospike = {
    PS_MOD(aerospike)
};

/*
 *******************************************************************************************************
 * Function to check validity of session object.
 *
 * @param session_p         The aerospike_session object whose validity is to
 *                          be checked.
 * @param error_p           The C SDK's as_error object to be populated by this
 *                          method in case of any errors if encountered.
 *
 * @return AEROSPIKE::OK if success. Otherwise AEROSPIKE_x.
 *******************************************************************************************************
 */
static as_status
validate_session(aerospike_session *session_p, as_error *error_p TSRMLS_DC)
{
    as_error_init(error_p);

    if (!session_p) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Invalid session object");
        DEBUG_PHP_EXT_WARNING("Invalid session object");
        goto exit;
    }

    if (!session_p->ns_p || !session_p->set_p) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Invalid session object");
        DEBUG_PHP_EXT_WARNING("Invalid session object");
        goto exit;
    }

    if (!session_p->aerospike_obj_p) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }
exit:
    return error_p->code;
}

/*
 *******************************************************************************************************
 * Function to initialize session object.
 * Allocates memory for outer session object only and initializes all fields.
 * Does not allocate memory for internal fields.
 *
 * @param session_p         The aerospike_session object to be initialized.
 * @param error_p           The C SDK's as_error object to be populated by this
 *                          method in case of any errors if encountered.
 *
 * @return AEROSPIKE::OK if success. Otherwise AEROSPIKE_x.
 *******************************************************************************************************
 */
static as_status
init_session(aerospike_session **session_pp, as_error *error_p TSRMLS_DC)
{
    as_error_init(error_p);

    if (NULL == (*session_pp = emalloc(sizeof(aerospike_session)))) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
                "Could not allocate memory for session object");
        DEBUG_PHP_EXT_ERROR("Could not allocate memory for session object");
        goto exit;
    }

    if (NULL != ((*session_pp)->aerospike_obj_p = ecalloc(1, sizeof(Aerospike_object)))) {
        (*session_pp)->aerospike_obj_p->as_ref_p = NULL;
        (*session_pp)->aerospike_obj_p->is_conn_16 = AEROSPIKE_CONN_STATE_FALSE;
        (*session_pp)->aerospike_obj_p->is_persistent = true;
    } else {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
                "Could not allocate memory for aerospike object");
        DEBUG_PHP_EXT_ERROR("Could not allocate memory for aerospike object");
        goto exit;
    }
exit:
    return error_p->code;
}

/*
 *******************************************************************************************************
 * Function to destroy session object.
 * De-allocates memory for outer session object as well as internal fields
 * except ns_p and set_p.
 *
 * @param session_p         The aerospike_session object to be destroyed.
 *
 * @return AEROSPIKE::OK if success. Otherwise AEROSPIKE_x.
 *******************************************************************************************************
 */
static void
destroy_session(aerospike_session *session_p TSRMLS_DC)
{
    if (session_p && session_p->aerospike_obj_p) {
        session_p->aerospike_obj_p->as_ref_p = NULL;
        efree(session_p->aerospike_obj_p);
        session_p->aerospike_obj_p = NULL;
        efree(session_p);
        DEBUG_PHP_EXT_INFO("aerospike session object destroyed");
    } else {
        DEBUG_PHP_EXT_ERROR("invalid aerospike object");
    }
}

/*
 *******************************************************************************************************
 * PHP Exposed-Function to open an Aerospike PHP Session.
 * Opens an aerospike session by making a server connection using the hosts
 * specified in session.save_path only if session.save_handler is set to
 * "aerospike". The internal C SDK aerospike object is fetched/hashed into the
 * Aerospike extension's Persistent List.
 *
 * Invoked on calling session_start() from PHP userland.
 * @return SUCCESS or FAILURE.
 *******************************************************************************************************
 */
PS_OPEN_FUNC(aerospike)
{
    as_error                error;
    as_config               config;
    aerospike_session*      session_p = NULL;
    HashTable*              persistent_list = AEROSPIKE_G(persistent_list_g);
    int                     iter_host = 0;

    DEBUG_PHP_EXT_INFO("In PS_OPEN_FUNC");

    as_error_init(&error);

    if (AEROSPIKE_OK != init_session(&session_p, &error TSRMLS_CC)) {
        goto exit;
    }

    if (!session_p->aerospike_obj_p) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLIENT, "Invalid aerospike object");
        DEBUG_PHP_EXT_ERROR("Invalid aerospike object");
        goto exit;
    }

    if (AEROSPIKE_OK !=
            aerospike_helper_check_and_set_config_for_session(&config,
                (char *) save_path, session_p, &error TSRMLS_CC)) {
        DEBUG_PHP_EXT_ERROR("Unable to check and set config for session");
        goto exit;
    }

    if (AEROSPIKE_OK !=
            aerospike_helper_object_from_alias_hash(session_p->aerospike_obj_p,
                                                              true, &config,
                                                              persistent_list,
                                                              persist TSRMLS_CC)) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLIENT, "Unable to find object from alias");
        DEBUG_PHP_EXT_ERROR("Unable to find object from alias");
        goto exit;
    }

    /* Connect to the cluster */
    if (session_p->aerospike_obj_p->as_ref_p &&
            session_p->aerospike_obj_p->is_conn_16 == AEROSPIKE_CONN_STATE_FALSE &&
            (AEROSPIKE_OK != aerospike_connect(session_p->aerospike_obj_p->as_ref_p->as_p, &error))) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLUSTER, "Unable to connect to server");
        DEBUG_PHP_EXT_WARNING("Unable to connect to server");
        goto exit;
    }

    /* Addr of hosts has been malloced within parse_save_path() function of
     * aerospike_helper.c
     * We free the addr's here.
     */

    for (iter_host = 0; iter_host < config.hosts_size; iter_host++) {
        efree((char *) config.hosts[iter_host].addr);
    }

    /* connection is established, set the connection flag now */
    session_p->aerospike_obj_p->is_conn_16 = AEROSPIKE_CONN_STATE_TRUE;

    DEBUG_PHP_EXT_INFO("Success in creating php-aerospike object");
exit:
    if (error.code == AEROSPIKE_OK) {
        PS_SET_MOD_DATA(session_p);
        return SUCCESS;
    } else {
        destroy_session(session_p TSRMLS_CC);
        PS_SET_MOD_DATA(NULL);
        return FAILURE;
    }
}

/*
 *******************************************************************************************************
 * PHP Exposed-Function to close an Aerospike PHP Session.
 * Closes an aerospike session.
 * The internal C SDK aerospike object is not destroyed at this point for
 * further reuse. Only ref count is decremented.
 *
 * Invoked on calling session_write_close() from PHP userland or by default upon
 * script termination.
 * @return SUCCESS or FAILURE.
 *******************************************************************************************************
 */
PS_CLOSE_FUNC(aerospike)
{
    as_error            error;
    aerospike_session*  session_p = PS_GET_MOD_DATA();

    DEBUG_PHP_EXT_INFO("In PS_CLOSE_FUNC");
    as_error_init(&error);

    if (AEROSPIKE_OK != aerospike_helper_close_php_connection(session_p->aerospike_obj_p,
                    &error TSRMLS_CC)) {
        DEBUG_PHP_EXT_ERROR("Aerospike close returned error");
    }

    destroy_session(session_p TSRMLS_CC);
    PS_SET_MOD_DATA(NULL);

exit:
    return (error.code == AEROSPIKE_OK) ? SUCCESS : FAILURE;
}

/*
 *******************************************************************************************************
 * PHP Exposed-Function to read and populate contents of an Aerospike PHP Session.
 * Fetches record in the aerospike server with PK==session_id.
 * Populates the session object with contents of a bin named "PHP_SESSION".
 *
 * Invoked on calling session_start() from PHP userland.
 * @return SUCCESS or FAILURE.
 *******************************************************************************************************
 */
PS_READ_FUNC(aerospike)
{
    as_error            error;
    aerospike_session*  session_p = PS_GET_MOD_DATA();
    as_record*          record_p = NULL;
    as_key              key_get;
    int16_t             init_key = 0;
    char*               session_data_p = NULL;

    DEBUG_PHP_EXT_INFO("In PS_READ_FUNC");

    if (AEROSPIKE_OK != validate_session(session_p, &error TSRMLS_CC)) {
       goto exit; 
    }

    as_key_init_str(&key_get, session_p->ns_p, session_p->set_p, key);
    init_key = 1;

    if (AEROSPIKE_OK != aerospike_key_get(session_p->aerospike_obj_p->as_ref_p->as_p,
                &error, NULL, &key_get, &record_p)) {
        DEBUG_PHP_EXT_ERROR("Unable to retrieve session data");
        goto exit;
    }

    if (NULL == (session_data_p = as_record_get_str(record_p, AEROSPIKE_SESSION_BIN))) {
         PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLIENT,
                    "Unable to get session bin of the record");
         DEBUG_PHP_EXT_DEBUG("Unable to get session bin of the record");
         goto exit;
    }

    *val = estrndup(session_data_p, strlen(session_data_p));
    *vallen = strlen(session_data_p);

exit:
    if (init_key) {
        as_key_destroy(&key_get);
    }

    if (record_p) {
        as_record_destroy(record_p);
    }
    return (error.code == AEROSPIKE_OK) ? SUCCESS : FAILURE;
}

/*
 *******************************************************************************************************
 * PHP Exposed-Function to write off contents of an Aerospike PHP Session to the
 * server.
 * Writes a record in the aerospike server with PK==session_id with a bin named "PHP_SESSION"
 * containing all the session object contents.
 *
 * Invoked on calling session_write_close() from PHP userland.
 * @return SUCCESS or FAILURE.
 *******************************************************************************************************
 */
PS_WRITE_FUNC(aerospike)
{
    as_error            error;
    aerospike_session*  session_p = PS_GET_MOD_DATA();
    as_key              key_put;
    as_record           record;
    int16_t             init_key = 0;
    int16_t             init_record = 0;

    DEBUG_PHP_EXT_INFO("In PS_WRITE_FUNC");

    if (AEROSPIKE_OK != validate_session(session_p, &error TSRMLS_CC)) {
       goto exit; 
    }

    if (key == NULL || !strcmp(key, "")) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLIENT, "Invalid Session ID");
        DEBUG_PHP_EXT_ERROR("Invalid Session ID");
        goto exit;
    }

    if (val == NULL || !strcmp(val, "")) {
        DEBUG_PHP_EXT_DEBUG("Empty session data");
        goto exit;
    }

    as_key_init_str(&key_put, session_p->ns_p, session_p->set_p, key);
    init_key = 1;

    as_record_inita(&record, 1);
    init_record = 1;
    if (!as_record_set_str(&record, AEROSPIKE_SESSION_BIN, val)) {
        PHP_EXT_SET_AS_ERR(&error, AEROSPIKE_ERR_CLIENT, "Unable to set record");
        DEBUG_PHP_EXT_ERROR("Unable to set record");
        goto exit;
    }

    record.ttl = CACHE_EXPIRE_PHP_INI;
    if (AEROSPIKE_OK != aerospike_key_put(session_p->aerospike_obj_p->as_ref_p->as_p,
                                        &error, NULL, &key_put, &record)) {
        DEBUG_PHP_EXT_ERROR("Unable to save session data");
    }

exit:
    if (init_record) {
        as_record_destroy(&record);
    }
    if (init_key) {
        as_key_destroy(&key_put);
    }
    return (error.code == AEROSPIKE_OK) ? SUCCESS : FAILURE;
}

/*
 *******************************************************************************************************
 * PHP Exposed-Function to destroy an Aerospike PHP Session deleting the
 * contents from the server.
 * Deletes a record in the aerospike server with PK==session_id
 * that contains all the session object contents.
 *
 * Invoked on calling session_destroy() from PHP userland.
 * @return SUCCESS or FAILURE.
 *******************************************************************************************************
 */
PS_DESTROY_FUNC(aerospike)
{
    as_error            error;
    aerospike_session*  session_p = PS_GET_MOD_DATA();
    as_key              key_remove;
    int16_t             init_key = 0;

    DEBUG_PHP_EXT_INFO("In PS_DESTROY_FUNC");

    if (AEROSPIKE_OK != validate_session(session_p, &error TSRMLS_CC)) {
       goto exit; 
    }

    as_key_init_str(&key_remove, session_p->ns_p, session_p->set_p, key);
    init_key = 1;

    if (AEROSPIKE_OK !=
            aerospike_key_remove(session_p->aerospike_obj_p->as_ref_p->as_p,
                &error, NULL, &key_remove)) {
        goto exit;
    }

exit:
    if (init_key) {
        as_key_destroy(&key_remove);
    }
    return (error.code == AEROSPIKE_OK) ? SUCCESS : FAILURE;
}

/*
 *******************************************************************************************************
 * PHP Exposed-Function to handle gc of Aerospike PHP Session
 *
 * @return SUCCESS or FAILURE.
 *******************************************************************************************************
 */
PS_GC_FUNC(aerospike)
{
    DEBUG_PHP_EXT_INFO("In PS_GC_FUNC");
    return SUCCESS;
}

