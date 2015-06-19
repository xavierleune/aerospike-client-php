#include "php.h"
#include "aerospike/as_log.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_config.h"
#include "aerospike/as_error.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike.h"
#include "aerospike_common.h"
#include "aerospike/as_udf.h"
#include "aerospike/aerospike_udf.h"
#include "aerospike_policy.h"

/*
 ******************************************************************************************************
 Registers a UDF module.
 *
 * @param aerospike_obj_p           The C client's aerospike object.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param path_p                    The path to the module on the client side
 * @param language                  The Aerospike::UDF_TYPE_* constant.
 * @param options_p                 The optional policy.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status
aerospike_udf_register(Aerospike_object* aerospike_obj_p, as_error* error_p,
        char* path_p, char* module_p, long language, zval* options_p)
{
    FILE*                   file_p = NULL;
    uint32_t                size = 0;
    uint32_t                content_size;
    uint8_t*                bytes_p = NULL;
    uint8_t*                buff_p = NULL;
    uint32_t                read;
    as_bytes                udf_content;
    as_bytes*               udf_content_p = NULL;
    as_policy_info          info_policy;
    TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);

    set_policy(&aerospike_obj_p->as_ref_p->as_p->config, NULL,
            NULL, NULL, NULL, &info_policy, NULL, NULL, NULL,
            options_p, error_p TSRMLS_CC);
    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    file_p = fopen(path_p, "r");

    if (!file_p) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_UDF_NOT_FOUND,
                "Cannot open script file");
        DEBUG_PHP_EXT_DEBUG("Cannot open script file");
        goto exit;
    }

    fseek(file_p, 0L, SEEK_END);
    content_size = ftell(file_p);
    fseek(file_p, 0L, SEEK_SET);

    /*
     * Using emalloc here to maintain consistency of PHP extension.
     * malloc can also be used for C-SDK.
     * if emalloc is used, pass parameter 4 of function as_bytes_init_wrap as
     * "false" and handle the freeing up of the same here.
     */
    if (NULL == (bytes_p = (uint8_t *) emalloc(content_size + 1))) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
                "Memory allocation failed for contents of UDF");
        DEBUG_PHP_EXT_DEBUG("Memory allocation failed for contents of UDF");
        goto exit;
    }

    buff_p = bytes_p;
    read = (int) fread(buff_p, 1, LUA_FILE_BUFFER_FRAME, file_p);
    while (read) {
        size += read;
        buff_p += read;
        read = (int) fread(buff_p, 1, LUA_FILE_BUFFER_FRAME, file_p);
    }

    char *udf_path = LUA_USER_PATH_PHP_INI;
    char copy_filepath[AS_CONFIG_PATH_MAX_LEN] = {0};
    uint32_t user_path_len = strlen(udf_path);

    memcpy(copy_filepath, udf_path, user_path_len);

    if(udf_path[user_path_len - 1] != '/') {
        memcpy(copy_filepath + user_path_len, "/", 1);
    }

    char *filename = strrchr(path_p, '/');
    if(!filename) {
        filename = path_p;
    } else if(filename[0] == '/') {
        filename = filename + 1;
    }

    memcpy(copy_filepath + user_path_len + 1, filename, strlen(filename));

    FILE *fileW_p = NULL;
    fileW_p = fopen(copy_filepath, "r");
    if(!fileW_p && errno == ENOENT) {  
        fileW_p = fopen(copy_filepath, "w");
        if (!fileW_p) {
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_UDF_NOT_FOUND,
                    "Cannot create script file");
            DEBUG_PHP_EXT_DEBUG("Cannot create script file");
            goto exit;
        }

        if (0 >= (int) fwrite(bytes_p, size, 1, fileW_p)) {
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_UDF_NOT_FOUND,
                    "unable to write to script file");
            DEBUG_PHP_EXT_DEBUG("unable to write to script file");
            goto exit;
        }
    }

    as_bytes_init_wrap(&udf_content, bytes_p, size, false);
    udf_content_p = &udf_content;
    /*
     * Register the UDF file in the database cluster.
     */
    if (AEROSPIKE_OK != aerospike_udf_put(aerospike_obj_p->as_ref_p->as_p,
                error_p, &info_policy, module_p, language,
                      udf_content_p)) {
        DEBUG_PHP_EXT_DEBUG("%s", error_p->message);
        goto exit;
    } else if (AEROSPIKE_OK !=
            aerospike_udf_put_wait(aerospike_obj_p->as_ref_p->as_p,
                error_p, &info_policy, module_p, 0)) {
        DEBUG_PHP_EXT_DEBUG("%s", error_p->message);
        goto exit;
    }

exit:
    if (file_p) {
        fclose(file_p);
    }

    if (bytes_p) {
        efree(bytes_p);
    }

    if (fileW_p) {
        fclose(fileW_p);
    }

    if (udf_content_p) {
        as_bytes_destroy(udf_content_p);
    }

    return error_p->code;
}

/*
 ******************************************************************************************************
 Remove a UDF module from the aerospike DB.
 *
 * @param aerospike_obj_p           The C client's aerospike object.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param module_p                  The name of the UDF module to remove from the Aerospike DB.
 * @param module_len                The module name length.
 * @param options_p                 The optional policy.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status
aerospike_udf_deregister(Aerospike_object* aerospike_obj_p,
        as_error* error_p, char* module_p, zval* options_p)
{
    as_policy_info          info_policy;
    TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);

    set_policy(&aerospike_obj_p->as_ref_p->as_p->config, NULL,
            NULL, NULL, NULL, &info_policy,NULL, NULL, NULL,
            options_p, error_p TSRMLS_CC);
    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    if (AEROSPIKE_OK != aerospike_udf_remove(aerospike_obj_p->as_ref_p->as_p,
                error_p, NULL, module_p)) {
        DEBUG_PHP_EXT_ERROR("%s", error_p->message);
        goto exit;
    }
exit:
    return error_p->code;
}

/*
 ******************************************************************************************************
 Applies a UDF to a record at the Aerospike DB.
 *
 * @param aerospike_obj_p           The C client's aerospike object.
 * @param as_key_p                  The C client's as_key that identifies the
 *                                  record on which UDF will be applied.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param module_p                  The name of the UDF module registered
 *                                  against the Aerospike DB.
 * @param function_p                The name of the function to be applied to
 *                                  the record.
 * @param args_pp                   An array of arguments for the UDF.
 * @param return_value_p            It will contain result value of calling the
 *                                  UDF.
 * @param options_p                 The optional policy.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status
aerospike_udf_apply(Aerospike_object* aerospike_obj_p,
        as_key* as_key_p, as_error* error_p, char* module_p, char* function_p,
        zval** args_pp, zval* return_value_p, zval* options_p, int8_t* serializer_policy_p)
{
    as_arraylist                args_list;
    as_arraylist*               args_list_p = NULL;
    as_static_pool              udf_pool = {0};
    as_val*                     udf_result_p = NULL;
    foreach_callback_udata      udf_result_callback_udata;
    int8_t                      serializer_policy = (serializer_policy_p) ? *serializer_policy_p : SERIALIZER_NONE;
    as_policy_apply             apply_policy;
    TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);

    set_policy_udf_apply(&aerospike_obj_p->as_ref_p->as_p->config, &apply_policy, &serializer_policy,
            options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    if ((*args_pp)) {
        as_arraylist_inita(&args_list,
                zend_hash_num_elements(Z_ARRVAL_PP(args_pp)));
        args_list_p = &args_list;
        AS_LIST_PUT(NULL, args_pp, args_list_p, &udf_pool, serializer_policy, error_p TSRMLS_CC);
    }

    if (AEROSPIKE_OK != (aerospike_key_apply(aerospike_obj_p->as_ref_p->as_p,
                    error_p, &apply_policy, as_key_p, module_p, function_p,
                    (as_list *) args_list_p, &udf_result_p))) {
        DEBUG_PHP_EXT_DEBUG("%s", error_p->message);
        goto exit;
    }

    if (return_value_p) {
        udf_result_callback_udata.udata_p = return_value_p;
        udf_result_callback_udata.error_p = error_p;
        AS_DEFAULT_GET(NULL, udf_result_p, &udf_result_callback_udata);
    }
     
exit:
    if (udf_result_p) {
        as_val_destroy(udf_result_p);
    }
    
    if (args_list_p) {
        as_arraylist_destroy(args_list_p);
    }

    /* clean up the as_* objects that were initialised */
    aerospike_helper_free_static_pool(&udf_pool);

    return error_p->code;
}

/*
 ******************************************************************************************************
 Lists the UDF modules registered with the server.
 *
 * @param aerospike_obj_p           The C client's aerospike object.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param array_of_modules_p        An array of registered UDF modules
 *                                  against the Aerospike DB to be populated by
 *                                  this function.
 * @param language                  Optionally filters a subset of modules
 *                                  matching the given type.
 * @options_p                       The optional parameters to the
 *                                  Aerospike::listRegistered().
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status
aerospike_list_registered_udf_modules(Aerospike_object* aerospike_obj_p,
        as_error* error_p, zval* array_of_modules_p, long language,
        zval* options_p)
{
    as_udf_files            udf_files;
    uint32_t                init_udf_files = 0;
    uint32_t                i = 0;
    as_policy_info          info_policy;
    TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);

    set_policy(&aerospike_obj_p->as_ref_p->as_p->config, NULL,
            NULL, NULL, NULL, &info_policy, NULL, NULL, NULL,
            options_p, error_p TSRMLS_CC);
    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    as_udf_files_init(&udf_files, 0);
    init_udf_files = 1;
    if (AEROSPIKE_OK != aerospike_udf_list(aerospike_obj_p->as_ref_p->as_p,
                error_p, &info_policy, &udf_files)) {
        DEBUG_PHP_EXT_ERROR("%s", error_p->message);
        goto exit;
    }

    for (i = 0; i < udf_files.size; i++) {
        if ((language != -1) && (udf_files.entries[i].type != language)) {
                continue;
        }
        zval* module_p = NULL;
        MAKE_STD_ZVAL(module_p);
        array_init(module_p);
        add_assoc_stringl(module_p, UDF_MODULE_NAME, udf_files.entries[i].name,
                strlen(udf_files.entries[i].name), 1);
        add_assoc_long(module_p, UDF_MODULE_TYPE,
                (udf_files.entries[i].type));
        add_next_index_zval(array_of_modules_p, module_p);
    }

exit:
    if (init_udf_files) {
        as_udf_files_destroy(&udf_files);
    }
    return error_p->code;
}

/*
 ******************************************************************************************************
 Get the code of registered UDF module.
 *
 * @param aerospike_obj_p           The C client's aerospike object.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param module_p                  The name of the UDF module to get from the
 *                                  server.
 * @param module_len                Length of UDF module name.
 * @param udf_code_p                Code of the UDF to be populated by this
 *                                  function.
 * @param language                  The Aerospike::UDF_TYPE_* constant.
 * @param options_p                 The optional policy.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status
aerospike_get_registered_udf_module_code(Aerospike_object* aerospike_obj_p,
        as_error* error_p, char* module_p, zval* udf_code_p,
        long language, zval* options_p)
{
    uint32_t                init_udf_file = 0;
    as_udf_file             udf_file;
    as_policy_info          info_policy;
    TSRMLS_FETCH_FROM_CTX(aerospike_obj_p->ts);

    set_policy(&aerospike_obj_p->as_ref_p->as_p->config, NULL, NULL,
            NULL, NULL, &info_policy, NULL, NULL, NULL, options_p,
            error_p TSRMLS_CC);
    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

    as_udf_file_init(&udf_file);
    init_udf_file = 1;
    
    if (AEROSPIKE_OK != aerospike_udf_get(aerospike_obj_p->as_ref_p->as_p,
                error_p, &info_policy, module_p, language, &udf_file)) {
        DEBUG_PHP_EXT_ERROR("%s", error_p->message);
        goto exit;
    }

    ZVAL_STRINGL(udf_code_p, (char*) udf_file.content.bytes, udf_file.content.size, 1);
exit:
    if (init_udf_file) {
        as_udf_file_destroy(&udf_file);
    }
    return error_p->code;
}

