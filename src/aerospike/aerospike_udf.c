#include "php.h"
#include "aerospike/as_log.h"
#include "aerospike/as_key.h"
#include "aerospike/as_config.h"
#include "aerospike/as_error.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike.h"
#include "aerospike_common.h"
#include "aerospike/as_udf.h"
/*
 ******************************************************************************************************
 Registers a UDF module.
 *
 * @param aerospike_obj_p           The C client's aerospike object.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param path                      Path to the module on the client side
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status aerospike_udf_register(Aerospike_object* aerospike_obj_p,
        as_error* error_p, char* path)
{
    as_status               status = AEROSPIKE_OK;
    FILE*                   file;
    long                    size = 0;
    long                    content_size;
    uint8_t*                bytes;
    uint8_t*                buff;
    int                     read;
    as_bytes                udf_content;

    file = fopen(path, "r");

    if (!file) {
        PHP_EXT_SET_AS_ERROR(error_p, AEROSPIKE_ERR_PARAM, "Cannot open script file %s", path);
        goto exit;
    }

    fseek(file, 0L, SEEK_END);
    content_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    bytes = (uint8_t *) malloc(content_size+1);
    if (bytes == NULL) {
        PHP_EXT_SET_AS_ERROR(error_p, AEROSPIKE_ERR, "Memory allocation failed for contents of UDF");
        goto exit;
    }

    buff = bytes;
    read = (int) fread(buff, 1, 512, file);
    while (read) {
        size += read;
        buff += read;
        read = (int)fread(buff, 1, 512, file);
    }

    as_bytes_init_wrap(&udf_content, bytes, size, true);
    /*
     * Register the UDF file in the database cluster.
     */
    if (aerospike_udf_put(aerospike_obj_p->as_ref_p->as_p, error_p, NULL, path, AS_UDF_TYPE_LUA,
                &udf_content) != AEROSPIKE_OK) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Unable to register UDF module");
        goto exit;
    }
exit:
    as_bytes_destroy(&udf_content);
    return status;
}

/*
 ******************************************************************************************************
 Remove a UDF module from the aerospike DB.
 *
 * @param aerospike_obj_p           The C client's aerospike object.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param module                    The name of the UDF module to remove from the Aerospike DB.
 * @module_len                      The module name length.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status aerospike_udf_deregister(Aerospike_object* aerospike_obj_p,
        as_error* error_p, char* module, long module_len)
{
    as_status               status = AEROSPIKE_OK;
    const char*             lua_suffix = ".lua";
    char*                   file_name;
    long                    file_name_len;

    file_name_len = module_len + strlen(lua_suffix)+1;
    file_name = ecalloc(1, file_name_len);
    strncpy(file_name, module, module_len);
    strcat(file_name, lua_suffix);
    if (aerospike_udf_remove(aerospike_obj_p->as_ref_p->as_p, error_p, NULL, file_name) != AEROSPIKE_OK) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Unable to remove UDF module");
        goto exit;
    }
exit:
    if (file_name) {
        efree(file_name);
    }
    return status;
}

/*
 ******************************************************************************************************
 Applies a UDF to a record at the Aerospike DB.
 *
 * @param aerospike_obj_p           The C client's aerospike object.
 * @param as_key_p                  The C client's as_key that identifies the
 *                                  record on which UDF will be applied.
 * @param error_p                   The C client's as_error to be set to the encountered error.
 * @param module                    The name of the UDF module registered
 *                                  against the Aerospike DB.
 * @param function                  the name of the function to be applied to
 *                                  the record.
 * @param args                      An array of arguments for the UDF.
 * @param return_value              It will contain result value of calling the
 *                                  UDF.
 *
 * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_x.
 ******************************************************************************************************
 */
extern as_status aerospike_udf_apply(Aerospike_object* aerospike_obj_p, as_key* as_key_p,
        as_error* error_p, char* module, char* function, zval** args, zval** return_value)
{
    as_status              status = AEROSPIKE_OK;
    as_arraylist           args_list;
    as_static_pool         udf_pool = {0};

    as_arraylist_inita(&args_list, zend_hash_num_elements(Z_ARRVAL_PP(args)));
    AS_LIST_PUT(NULL, args, &args_list, &udf_pool, NULL, error_p);

    if (aerospike_key_apply(aerospike_obj_p->as_ref_p->as_p, error_p, NULL,
                as_key_p, module, function, &args_list, return_value) != AEROSPIKE_OK) {
        status = AEROSPIKE_ERR_PARAM;
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Unable to remove UDF module\n");
        goto exit;
    }
    printf("\n\n%d", Z_ARRVAL_PP(return_value));
exit:
    return status;
}
