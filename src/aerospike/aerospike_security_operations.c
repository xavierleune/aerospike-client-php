#include "php.h"

#include "aerospike/aerospike.h"
#include "aerospike/as_error.h"
#include "aerospike/as_admin.h"
#include "aerospike_common.h"

#define MAX_ROLES 4

/*
 *******************************************************************************************************
 * Internal function to convert roles from zval array to a char ** roles array.
 *
 * @param roles_ht_p            The HashTable for the roles array.
 * @param roles_array_p         The conversion reult to be populated by this
 *                              method.
 * @param roles_count           The count of roles to be populated by this
 *                              method.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 *******************************************************************************************************
 */
static as_status
aerospike_security_operations_convert_roles_from_zval(HashTable *roles_ht_p,
        char **roles_array_p, int *roles_count, as_error *error_p)
{
    HashPosition                roles_position;
    zval**                      roles_entry = NULL;
    int                         roles_index = 0;

    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, "");
    if ((*roles_count = zend_hash_num_elements(roles_ht_p)) == 0) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "No roles specified");
        DEBUG_PHP_EXT_DEBUG("No roles specified");
        goto exit;
    }

    foreach_hashtable(roles_ht_p, roles_position, roles_entry) {
        if (Z_TYPE_PP(roles_entry) != IS_STRING) {
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Expected role of type string");
            DEBUG_PHP_EXT_DEBUG("Expected role of type string");
            goto exit;
        }
        roles_array_p[roles_index++] = Z_STRVAL_PP(roles_entry);
    }
exit:
    return error_p->code;
}

/*
 *******************************************************************************************************
 * Internal function to convert roles into zval array from as_user_roles.
 *
 * @param roles_p               The zval array to be populated by this method with the roles.
 * @param user_roles_p          The as_user_roles to be converted into a zval
 *                              array.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 *******************************************************************************************************
 */
static as_status
aerospike_security_operations_convert_roles_to_zval(zval *roles_p,
        as_user_roles *user_roles_p, as_error *error_p)
{
    int                         roles_index = 0;

    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, "");

    if (!user_roles_p) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Invalid as_user_roles");
        DEBUG_PHP_EXT_DEBUG("Invalid as_user_roles");
        goto exit;
    }

    for (roles_index = 0; roles_index < user_roles_p->roles_size; roles_index++) {
        if (user_roles_p->roles[roles_index] && add_next_index_stringl(roles_p,
                        user_roles_p->roles[roles_index],
                        strlen(user_roles_p->roles[roles_index]), 1)) {
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to add a role into the zval array");
            DEBUG_PHP_EXT_DEBUG("Unable to add a role into the zval array");
            goto exit; 
        }
    }

exit:
    return error_p->code;
}

/*
 *******************************************************************************************************
 * Wrapper function to create a new user on aerospike server.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param user_p                The user to be created.
 * @param password_p            The password of the user to be created.
 * @param roles_ht_p            The HashTable for the roles array.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_security_operations_create_user(aerospike* as_object_p, as_error *error_p,
        char* user_p, char* password_p, HashTable* roles_ht_p, zval* options_p TSRMLS_DC)
{
    as_policy_admin             admin_policy;
    char*                       roles_array_p[MAX_ROLES] = {0};
    int                         roles_count = 0;

    if ((!error_p) || (!as_object_p) || (!user_p) || (!password_p)) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to create user");
        DEBUG_PHP_EXT_DEBUG("Unable to create user");
        goto exit;
    }

    set_policy_admin(&admin_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }
    
    if (AEROSPIKE_OK !=
            aerospike_security_operations_convert_roles_from_zval(roles_ht_p,
                roles_array_p, &roles_count, error_p)) {
        DEBUG_PHP_EXT_DEBUG("Unable to parse roles");
        goto exit;
    }

    if (AEROSPIKE_OK != (error_p->code = aerospike_create_user(as_object_p,
                    &admin_policy, user_p, password_p,
                    (const char **) roles_array_p, roles_count))) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to create user");
        DEBUG_PHP_EXT_DEBUG("Unable to create user");
        goto exit;
    }

exit:
    return(error_p->code);
}

/*
 *******************************************************************************************************
 * Wrapper function to drop an existing user on aerospike server.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param user_p                The user to be created.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_security_operations_drop_user(aerospike* as_object_p,
        as_error *error_p, char* user_p, zval* options_p TSRMLS_DC)
{
    as_policy_admin             admin_policy;

    if ((!error_p) || (!as_object_p) || (!user_p)) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to drop user");
        DEBUG_PHP_EXT_DEBUG("Unable to drop user");
        goto exit;
    }

    set_policy_admin(&admin_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }
    
    if (AEROSPIKE_OK != (error_p->code = aerospike_drop_user(as_object_p,
                    &admin_policy, user_p))) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to drop user");
        DEBUG_PHP_EXT_DEBUG("Unable to drop user");
        goto exit;
    }

exit:
    return(error_p->code);
}

/*
 *******************************************************************************************************
 * Wrapper function to change password of an existing user on aerospike server.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param user_p                The user whose password is to be changed.
 * @param password_p            The new password of the user.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_security_operations_change_password(aerospike* as_object_p,
        as_error *error_p, char* user_p, char* password_p,
        zval* options_p TSRMLS_DC)
{
    as_policy_admin             admin_policy;

    if ((!error_p) || (!as_object_p) || (!user_p) || (!password_p)) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to create user");
        DEBUG_PHP_EXT_DEBUG("Unable to create user");
        goto exit;
    }

    set_policy_admin(&admin_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }
    
    if (AEROSPIKE_OK != (error_p->code = aerospike_set_password(as_object_p,
                    &admin_policy, user_p, password_p))) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to change password");
        DEBUG_PHP_EXT_DEBUG("Unable to change password");
        goto exit;
    }

exit:
    return(error_p->code);
}

/*
 *******************************************************************************************************
 * Wrapper function to grant roles to an existing user on aerospike server.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param user_p                The user to which roles are to be granted.
 * @param roles_ht_p            The HashTable for the roles array.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_security_operations_grant_roles(aerospike* as_object_p, as_error *error_p,
        char* user_p, HashTable* roles_ht_p, zval* options_p TSRMLS_DC)
{
    as_policy_admin             admin_policy;
    char*                       roles_array_p[MAX_ROLES] = {0};
    int                         roles_count = 0;

    if ((!error_p) || (!as_object_p) || (!user_p))  {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to grant roles");
        DEBUG_PHP_EXT_DEBUG("Unable to grant roles");
        goto exit;
    }

    set_policy_admin(&admin_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }
    
    if (AEROSPIKE_OK !=
            aerospike_security_operations_convert_roles_from_zval(roles_ht_p,
                roles_array_p, &roles_count, error_p)) {
        DEBUG_PHP_EXT_DEBUG("Unable to parse roles");
        goto exit;
    }

    if (AEROSPIKE_OK != (error_p->code = aerospike_grant_roles(as_object_p,
                    &admin_policy, user_p, (const char **) roles_array_p,
                    roles_count))) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to grant roles");
        DEBUG_PHP_EXT_DEBUG("Unable to grant roles");
        goto exit;
    }

exit:
    return(error_p->code);
}

/*
 *******************************************************************************************************
 * Wrapper function to revoke roles from an existing user on aerospike server.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param user_p                The user from which roles are to be revoked.
 * @param roles_ht_p            The HashTable for the roles array.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_security_operations_revoke_roles(aerospike* as_object_p, as_error *error_p,
        char* user_p, HashTable* roles_ht_p, zval* options_p TSRMLS_DC)
{
    as_policy_admin             admin_policy;
    char*                       roles_array_p[MAX_ROLES] = {0};
    int                         roles_count = 0;

    if ((!error_p) || (!as_object_p) || (!user_p))  {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to revoke roles");
        DEBUG_PHP_EXT_DEBUG("Unable to revoke roles");
        goto exit;
    }

    set_policy_admin(&admin_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }
    
    if (AEROSPIKE_OK !=
            aerospike_security_operations_convert_roles_from_zval(roles_ht_p,
                roles_array_p, &roles_count, error_p)) {
        DEBUG_PHP_EXT_DEBUG("Unable to parse roles");
        goto exit;
    }

    if (AEROSPIKE_OK != (error_p->code = aerospike_revoke_roles(as_object_p,
                    &admin_policy, user_p, (const char **) roles_array_p,
                    roles_count))) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to revoke roles");
        DEBUG_PHP_EXT_DEBUG("Unable to revoke roles");
        goto exit;
    }

exit:
    return(error_p->code);
}

/*
 *******************************************************************************************************
 * Wrapper function to replace roles from an existing user on aerospike server.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param user_p                The user from which roles are to be replaced.
 * @param roles_ht_p            The HashTable for the roles array.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_security_operations_replace_roles(aerospike* as_object_p, as_error *error_p,
        char* user_p, HashTable* roles_ht_p, zval* options_p TSRMLS_DC)
{
    as_policy_admin             admin_policy;
    char*                       roles_array_p[MAX_ROLES] = {0};
    int                         roles_count = 0;

    if ((!error_p) || (!as_object_p) || (!user_p))  {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to replace roles");
        DEBUG_PHP_EXT_DEBUG("Unable to replace roles");
        goto exit;
    }

    set_policy_admin(&admin_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }
    
    if (AEROSPIKE_OK !=
            aerospike_security_operations_convert_roles_from_zval(roles_ht_p,
                roles_array_p, &roles_count, error_p)) {
        DEBUG_PHP_EXT_DEBUG("Unable to parse roles");
        goto exit;
    }

    if (AEROSPIKE_OK != (error_p->code = aerospike_replace_roles(as_object_p,
                    &admin_policy, user_p, (const char **) roles_array_p,
                    roles_count))) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to replace roles");
        DEBUG_PHP_EXT_DEBUG("Unable to replace roles");
        goto exit;
    }

exit:
    return(error_p->code);
}

/*
 *******************************************************************************************************
 * Wrapper function to query a user on aerospike server to retrieve the user's roles.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param user_p                The user from which roles are to be retrieved.
 * @param roles_p               The zval roles array to be populated by this method.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_security_operations_query_user(aerospike* as_object_p, as_error *error_p,
        char* user_p, zval* roles_p, zval* options_p TSRMLS_DC)
{
    as_policy_admin             admin_policy;
    as_user_roles*              user_roles_p = NULL;

    if ((!error_p) || (!as_object_p) || (!user_p))  {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to query user");
        DEBUG_PHP_EXT_DEBUG("Unable to query user");
        goto exit;
    }

    set_policy_admin(&admin_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }
    
    if (AEROSPIKE_OK != (error_p->code = aerospike_query_user(as_object_p,
                    &admin_policy, user_p, &user_roles_p))) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to query user");
        DEBUG_PHP_EXT_DEBUG("Unable to query user");
        goto exit;
    }

    if (AEROSPIKE_OK !=
            aerospike_security_operations_convert_roles_to_zval(roles_p,
                user_roles_p, error_p)) {
        DEBUG_PHP_EXT_DEBUG("Unable to parse as_user_roles");
        goto exit;
    }

exit:
    if (user_roles_p) {
        as_user_roles_destroy(user_roles_p);
    }
    return(error_p->code);
}

/*
 *******************************************************************************************************
 * Wrapper function to query all users on aerospike server to retrieve their roles.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param roles_p               The zval roles array to be populated by this method.
 *                              (Associative array with keys as users)
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_security_operations_query_users(aerospike* as_object_p, as_error *error_p,
        zval* roles_p, zval* options_p TSRMLS_DC)
{
    as_policy_admin             admin_policy;
    as_user_roles**             all_roles_pp = NULL;
    int                         user_count = 0;
    int                         user_index = 0;

    if ((!error_p) || (!as_object_p))  {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to query users");
        DEBUG_PHP_EXT_DEBUG("Unable to query users");
        goto exit;
    }

    set_policy_admin(&admin_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }
    
    if (AEROSPIKE_OK != (error_p->code = aerospike_query_users(as_object_p,
                    &admin_policy, &all_roles_pp, &user_count))) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to query users");
        DEBUG_PHP_EXT_DEBUG("Unable to query users");
        goto exit;
    }

    for (user_index = 0; user_index < user_count; user_index++) {
        zval *user_p = NULL;
        MAKE_STD_ZVAL(user_p);
        array_init(user_p);

        if (AEROSPIKE_OK !=
                aerospike_security_operations_convert_roles_to_zval(user_p,
                    all_roles_pp[user_index], error_p)) {
            zval_ptr_dtor(&user_p);
            DEBUG_PHP_EXT_DEBUG("Unable to parse as_user_roles");
            goto exit;
        }

        if (all_roles_pp[user_index]->user &&
                (0 != add_assoc_zval(roles_p, all_roles_pp[user_index]->user,
                                     user_p))) {
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
                    "Unable to add user to roles array");
            DEBUG_PHP_EXT_DEBUG("Unable to add user to roles array");
            goto exit;
        }
    }

exit:
    return(error_p->code);
}

