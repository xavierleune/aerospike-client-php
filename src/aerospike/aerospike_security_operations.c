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
        char **roles_array_p, int *roles_count, as_error *error_p TSRMLS_DC)
{
    HashPosition                roles_position;
#if (PHP_VERSION_ID < 70000)
    zval**                      roles_entry = NULL;
#else
    zval*                       roles_entry = NULL;
#endif
    int                         roles_index = 0;

    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, "");
    if ((*roles_count = zend_hash_num_elements(roles_ht_p)) == 0) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "No roles specified");
        DEBUG_PHP_EXT_DEBUG("No roles specified");
        goto exit;
    }

    AEROSPIKE_FOREACH_HASHTABLE(roles_ht_p, roles_position, roles_entry) {
#if (PHP_VERSION_ID < 70000)
        if (Z_TYPE_PP(roles_entry) != IS_STRING)
#else
        if (Z_TYPE_P(roles_entry) != IS_STRING)
#endif
        {
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Expected role of type string");
            DEBUG_PHP_EXT_DEBUG("Expected role of type string");
            goto exit;
        }
#if (PHP_VERSION_ID < 70000)
        roles_array_p[roles_index++] = Z_STRVAL_PP(roles_entry);
#else
        roles_array_p[roles_index++] = Z_STRVAL_P(roles_entry);
#endif
    }
exit:
    return error_p->code;
}

/*
 *******************************************************************************************************
 * Internal function to convert privileges from zval array to a char ** roles array.
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
aerospike_security_operations_convert_privileges_from_zval(HashTable *privileges_ht_p,
        as_privilege **privileges, int privileges_count, as_error *error_p TSRMLS_DC)
{
    HashPosition                privileges_position;
    HashPosition                privileges_individual_position;
#if (PHP_VERSION_ID < 70000)
    zval**                      privileges_entry = NULL;
#else
    zval *                      privileges_entry = NULL;
#endif
    int                         privileges_index = 0;

    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, "");

    AEROSPIKE_FOREACH_HASHTABLE(privileges_ht_p, privileges_position, privileges_entry) {
#if (PHP_VERSION_ID < 70000)
        zval**                      each_privilege_entry = NULL;
#else
        zval*                       each_privilege_entry = NULL;
#endif
        HashTable* each_privilege_p = NULL;
#if (PHP_VERSION_ID < 70000)
        if (Z_TYPE_PP(privileges_entry) != IS_ARRAY)
#else
        if (Z_TYPE_P(privileges_entry) != IS_ARRAY)
#endif
        {
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "Expected privilege of type array");
            DEBUG_PHP_EXT_DEBUG("Expected privilege of type array");
            goto exit;
        }
		privileges[privileges_index] = (as_privilege *)cf_malloc(sizeof(as_privilege));
		strcpy(privileges[privileges_index]->ns, ""); 
		strcpy(privileges[privileges_index]->set, ""); 
#if (PHP_VERSION_ID < 70000)
        each_privilege_p = Z_ARRVAL_P(*privileges_entry);
#else
        each_privilege_p = Z_ARRVAL_P(privileges_entry);
#endif
		AEROSPIKE_FOREACH_HASHTABLE(each_privilege_p, privileges_individual_position, each_privilege_entry)  {
			char * options_key = NULL;
			ulong options_index;
			uint options_key_len;

#if (PHP_VERSION_ID < 70000)
			if (AEROSPIKE_ZEND_HASH_GET_CURRENT_KEY_EX(Z_ARRVAL_P(*privileges_entry), (char **) &options_key,
                        &options_key_len, &options_index, 0, &privileges_individual_position)
                                != HASH_KEY_IS_STRING)
#else
			if (AEROSPIKE_ZEND_HASH_GET_CURRENT_KEY_EX(Z_ARRVAL_P(privileges_entry), (char **) &options_key,
                        &options_key_len, &options_index, 0, &privileges_individual_position)
                                != HASH_KEY_IS_STRING)
#endif
            {
                DEBUG_PHP_EXT_DEBUG("Privilege key should be a string");
                PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR,
                        "Privilege key should be a string");
                goto exit;
            }
			if(strcmp(options_key, "ns") == 0) {
#if (PHP_VERSION_ID < 70000)
				strcpy(privileges[privileges_index]->ns, Z_STRVAL_PP(each_privilege_entry)); 
#else
				strcpy(privileges[privileges_index]->ns, Z_STRVAL_P(each_privilege_entry)); 
#endif
			} else if(strcmp(options_key, "set") == 0) {
#if (PHP_VERSION_ID < 70000)
				strcpy(privileges[privileges_index]->set, Z_STRVAL_PP(each_privilege_entry));
#else
				strcpy(privileges[privileges_index]->set, Z_STRVAL_P(each_privilege_entry));
#endif
			} else if(strcmp(options_key, "code") == 0) {
#if (PHP_VERSION_ID < 70000)
				privileges[privileges_index]->code = Z_LVAL_PP(each_privilege_entry);
#else
				privileges[privileges_index]->code = Z_LVAL_P(each_privilege_entry);
#endif
			} else {
                DEBUG_PHP_EXT_DEBUG("Privilege key should be either code, ns or set");
                PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR,
                        "Privilege key should be either code, ns or set");
                goto exit;
			}
        }
		privileges_index++;
    }
exit:
    return error_p->code;
}
/*
 *******************************************************************************************************
 * Internal function to convert roles into zval array from as_user_roles.
 *
 * @param roles_p               The zval array to be populated by this method with the roles.
 * @param user_object_p          The as_user to be converted into a zval
 *                              array.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 *******************************************************************************************************
 */
static as_status
aerospike_security_operations_convert_user_to_zval(zval *roles_p,
        as_user *user_object_p, as_error *error_p TSRMLS_DC)
{
    int                         roles_index = 0;

    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, "");

    if (!user_object_p) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Invalid as_user_roles");
        DEBUG_PHP_EXT_DEBUG("Invalid as_user_roles");
        goto exit;
    }

    for (roles_index = 0; roles_index < user_object_p->roles_size; roles_index++) {
        //if (user_object_p->roles[roles_index] && add_next_index_stringl(roles_p,
        if (user_object_p->roles[roles_index] && AEROSPIKE_ADD_NEXT_INDEX_STRINGL(roles_p,
                        user_object_p->roles[roles_index],
                        strlen(user_object_p->roles[roles_index]), 1)) {
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
 * Internal function to convert privileges into zval array.
 *
 * @param privileges_p              The zval array to be populated by this method with the roles.
 * @param role_object_p          	The as_user to be converted into a zval array.
 * @param error_p               	The as_error to be populated by the function
 *                              	with the encountered error if any.
 *******************************************************************************************************
 */
static as_status
aerospike_security_operations_convert_privileges_to_zval(zval **privileges_p,
        as_privilege privileges[], int privileges_size, as_error *error_p TSRMLS_DC)
{
	int i = 0;
	for(i = 0; i < privileges_size; i++) {
#if (PHP_VERSION_ID < 70000)
		zval*	each_privilege_p = NULL;
		MAKE_STD_ZVAL(each_privilege_p);
		array_init(each_privilege_p);
#else
		zval	each_privilege_p;
		array_init(&each_privilege_p);
#endif

#if (PHP_VERSION_ID < 70000)
		if(0 != AEROSPIKE_ADD_ASSOC_STRINGL(each_privilege_p, "ns", privileges[i].ns, strlen(privileges[i].ns), 1))
#else
		if(0 != AEROSPIKE_ADD_ASSOC_STRINGL(&each_privilege_p, "ns", privileges[i].ns, strlen(privileges[i].ns), 1))
#endif
        {
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to get ns");
        	DEBUG_PHP_EXT_DEBUG("Unable to get ns");
        	goto exit;	
		}
#if (PHP_VERSION_ID < 70000)
		if(0 != AEROSPIKE_ADD_ASSOC_STRINGL(each_privilege_p, "set", privileges[i].set, strlen(privileges[i].set), 1))
#else
		if(0 != AEROSPIKE_ADD_ASSOC_STRINGL(&each_privilege_p, "set", privileges[i].set, strlen(privileges[i].set), 1))
#endif
        {
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to get set");
        	DEBUG_PHP_EXT_DEBUG("Unable to get set");
        	goto exit;	
		}
#if (PHP_VERSION_ID < 70000)
		if(0 != add_assoc_long(each_privilege_p, "code", privileges[i].code))
#else
		if(0 != add_assoc_long(&each_privilege_p, "code", privileges[i].code))
#endif
        {
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to get code");
        	DEBUG_PHP_EXT_DEBUG("Unable to get code");
        	goto exit;	
		}
#if (PHP_VERSION_ID < 70000)
		add_index_zval(*privileges_p, i, each_privilege_p);
#else
		add_index_zval(*privileges_p, i, &each_privilege_p);
#endif

	}
exit:
	return error_p->code;
}

/*
 *******************************************************************************************************
 * Internal function to convert role into zval array from as_role.
 *
 * @param privileges_p              The zval array to be populated by this method with the roles.
 * @param role_object_p          	The as_roles to be converted into a zval array.
 * @param error_p               	The as_error to be populated by the function
 *                              	with the encountered error if any.
 *******************************************************************************************************
 */
static as_status
aerospike_security_operations_convert_role_to_zval(zval *role_p,
        as_role *role_object_p, as_error *error_p TSRMLS_DC)
{
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, "");

    if (!role_object_p) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Invalid as_role");
        DEBUG_PHP_EXT_DEBUG("Invalid as_role");
        goto exit;
    }

	if(AEROSPIKE_OK != aerospike_security_operations_convert_privileges_to_zval(&role_p, role_object_p->privileges, 
							role_object_p->privileges_size, error_p TSRMLS_CC)) {
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to parse privileges");
        	DEBUG_PHP_EXT_DEBUG("Unable to parse privileges");
        	goto exit;	
	}

exit:
    return error_p->code;
}

/*
 *******************************************************************************************************
 * Internal function to convert roles into zval array from as_role.
 *
 * @param privileges_p              The zval array to be populated by this method with the roles.
 * @param role_object_p          	The as_roles to be converted into a zval array.
 * @param roles_size				The number of roles in the aerospike database.
 * @param error_p               	The as_error to be populated by the function
 *                              	with the encountered error if any.
 *******************************************************************************************************
 */
static as_status
aerospike_security_operations_convert_roles_to_zval(zval *role_p,
        as_role **role_object_p, int roles_size, as_error *error_p TSRMLS_DC)
{
	int i = 0;
    PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_OK, "");

    if (!role_object_p) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Invalid as_role");
        DEBUG_PHP_EXT_DEBUG("Invalid as_role");
        goto exit;
    }

	for (i = 0; i < roles_size; i++) {
#if (PHP_VERSION_ID < 70000)
		zval*	privileges_p = NULL;
		MAKE_STD_ZVAL(privileges_p);
		array_init(privileges_p);
#else
		zval	privileges_p;
		array_init(&privileges_p);
#endif

#if (PHP_VERSION_ID < 70000)
		if( 0 != aerospike_security_operations_convert_privileges_to_zval(&privileges_p, role_object_p[i]->privileges, 
																role_object_p[i]->privileges_size, error_p TSRMLS_CC))
#else
		if( 0 != aerospike_security_operations_convert_privileges_to_zval((zval **) &privileges_p, role_object_p[i]->privileges, 
																role_object_p[i]->privileges_size, error_p TSRMLS_CC))
#endif
        {
			PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT, "Unable to parse privileges");
        	DEBUG_PHP_EXT_DEBUG("Unable to parse privileges");
        	goto exit;	
		}

#if (PHP_VERSION_ID < 70000)
		add_assoc_zval(role_p, role_object_p[i]->name, privileges_p);
#else
		add_assoc_zval(role_p, role_object_p[i]->name, &privileges_p);
#endif
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
                roles_array_p, &roles_count, error_p TSRMLS_CC)) {
        DEBUG_PHP_EXT_DEBUG("Unable to parse roles");
        goto exit;
    }

	aerospike_create_user(as_object_p, error_p, &admin_policy, user_p, 
					password_p, (const char **) roles_array_p, roles_count);
    if (AEROSPIKE_OK != error_p->code) {
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
   
	aerospike_drop_user(as_object_p, error_p, &admin_policy, user_p);
    if (AEROSPIKE_OK != error_p->code) {
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
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to change password");
        DEBUG_PHP_EXT_DEBUG("Unable to change password");
        goto exit;
    }

    set_policy_admin(&admin_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

	aerospike_change_password(as_object_p, error_p, &admin_policy, user_p, password_p);
    if (AEROSPIKE_OK != error_p->code) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to change password");
        DEBUG_PHP_EXT_DEBUG("Unable to change password");
        goto exit;
    }

exit:
    return(error_p->code);
}

/*
 *******************************************************************************************************
 * Wrapper function to set password of an existing user on aerospike server.
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
aerospike_security_operations_set_password(aerospike* as_object_p,
        as_error *error_p, char* user_p, char* password_p,
        zval* options_p TSRMLS_DC)
{
    as_policy_admin             admin_policy;

    if ((!error_p) || (!as_object_p) || (!user_p) || (!password_p)) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to set password");
        DEBUG_PHP_EXT_DEBUG("Unable to set password");
        goto exit;
    }

    set_policy_admin(&admin_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }
	aerospike_set_password(as_object_p, error_p, &admin_policy, user_p, password_p);
    if (AEROSPIKE_OK != error_p->code) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to set password");
        DEBUG_PHP_EXT_DEBUG("Unable to set password");
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
                roles_array_p, &roles_count, error_p TSRMLS_CC)) {
        DEBUG_PHP_EXT_DEBUG("Unable to parse roles");
        goto exit;
    }

	aerospike_grant_roles(as_object_p, error_p, &admin_policy, user_p, (const char **) roles_array_p, roles_count);
    if (AEROSPIKE_OK != error_p->code) {
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
                roles_array_p, &roles_count, error_p TSRMLS_CC)) {
        DEBUG_PHP_EXT_DEBUG("Unable to parse roles");
        goto exit;
    }

	aerospike_revoke_roles(as_object_p, error_p, &admin_policy, user_p, (const char **) roles_array_p, roles_count);
    if (AEROSPIKE_OK != error_p->code) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to revoke roles");
        DEBUG_PHP_EXT_DEBUG("Unable to revoke roles");
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
    as_user*              		user_object_p = NULL;

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

	aerospike_query_user(as_object_p, error_p, &admin_policy, user_p, &user_object_p);
    if (AEROSPIKE_OK != error_p->code) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to query user");
        DEBUG_PHP_EXT_DEBUG("Unable to query user");
        goto exit;
    }

    if (AEROSPIKE_OK !=
            aerospike_security_operations_convert_user_to_zval(roles_p,
                user_object_p, error_p TSRMLS_CC)) {
        DEBUG_PHP_EXT_DEBUG("Unable to parse as_user");
        goto exit;
    }

exit:
    if (user_object_p) {
        as_user_destroy(user_object_p);
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
    as_user**             		all_users_pp = NULL;
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
   
	aerospike_query_users(as_object_p, error_p, &admin_policy, &all_users_pp, &user_count);
    if (AEROSPIKE_OK != error_p->code) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to query users");
        DEBUG_PHP_EXT_DEBUG("Unable to query users");
        goto exit;
    }

    for (user_index = 0; user_index < user_count; user_index++) {
#if (PHP_VERSION_ID < 70000)
        zval *user_p = NULL;
        MAKE_STD_ZVAL(user_p);
        array_init(user_p);
#else
        zval user_p;
        array_init(&user_p);
#endif

#if (PHP_VERSION_ID < 70000)
        if (AEROSPIKE_OK !=
                aerospike_security_operations_convert_user_to_zval(user_p,
                    all_users_pp[user_index], error_p TSRMLS_CC))
#else
        if (AEROSPIKE_OK !=
                aerospike_security_operations_convert_user_to_zval(&user_p,
                    all_users_pp[user_index], error_p TSRMLS_CC))
#endif
        {
            zval_ptr_dtor(&user_p);
            DEBUG_PHP_EXT_DEBUG("Unable to parse as_user");
            goto exit;
        }

#if (PHP_VERSION_ID < 70000)
        if (all_users_pp[user_index]->name &&
                (0 != add_assoc_zval(roles_p, all_users_pp[user_index]->name,
                                     user_p)))
#else

        if (all_users_pp[user_index]->name &&
                (0 != add_assoc_zval(roles_p, all_users_pp[user_index]->name,
                                     &user_p)))
#endif
        {
            PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_CLIENT,
                    "Unable to add user to all users array");
            DEBUG_PHP_EXT_DEBUG("Unable to add user to all users array");
            goto exit;
        }
    }

exit:
    return(error_p->code);
}

/*
 *******************************************************************************************************
 * Wrapper function to create a new role on aerospike server.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param role_p                The user defined role to be created.
 * @param privileges_ht_p       The HashTable for the privileges array.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_security_operations_create_role(aerospike* as_object_p, as_error *error_p,
        char* role_p, HashTable* privileges_ht_p, zval* options_p TSRMLS_DC)
{
    as_policy_admin             admin_policy;
	as_privilege** 				privileges = NULL;
    int                         privileges_size = 0;
	int 						i = 0;

    if ((!error_p) || (!as_object_p) || (!role_p)) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to create role");
        DEBUG_PHP_EXT_DEBUG("Unable to create role");
        goto exit;
    }

    set_policy_admin(&admin_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

	if( (privileges_size = zend_hash_num_elements(privileges_ht_p)) == 0) {
    	PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "No privileges specified");
        DEBUG_PHP_EXT_DEBUG("No privileges specified");
        goto exit;
	}

	privileges = (as_privilege **)alloca(sizeof(as_privilege *) * privileges_size);

    if (AEROSPIKE_OK !=
            aerospike_security_operations_convert_privileges_from_zval(privileges_ht_p,
                privileges, privileges_size, error_p TSRMLS_CC)) {
        DEBUG_PHP_EXT_DEBUG("Unable to parse privileges");
        goto exit;
    }

	aerospike_create_role(as_object_p, error_p, &admin_policy, role_p, privileges, privileges_size);
    if (AEROSPIKE_OK != error_p->code) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to create role");
        DEBUG_PHP_EXT_DEBUG("Unable to create role");
        goto exit;
    }

exit:
	if(privileges) {
        for(i = 0; i < privileges_size; i++) {
            if( privileges[i] != NULL)
                cf_free(privileges[i]);
        }
    }

    return(error_p->code);
}
/*
 *******************************************************************************************************
 * Wrapper function to drop an existing role in the aerospike server.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param role_p                The role to be dropped.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_security_operations_drop_role(aerospike* as_object_p,
        as_error *error_p, char* role_p, zval* options_p TSRMLS_DC)
{
    as_policy_admin             admin_policy;

    if ((!error_p) || (!as_object_p) || (!role_p)) {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to drop role");
        DEBUG_PHP_EXT_DEBUG("Unable to drop role");
        goto exit;
    }

    set_policy_admin(&admin_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }
   
	aerospike_drop_role(as_object_p, error_p, &admin_policy, role_p);
    if (AEROSPIKE_OK != error_p->code) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to drop role");
        DEBUG_PHP_EXT_DEBUG("Unable to drop role");
        goto exit;
    }

exit:
    return(error_p->code);
}

/*
 *******************************************************************************************************
 * Wrapper function to grant privileges to an existing user on aerospike server.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param roles_p               The role to which privileges are to be granted.
 * @param privileges_ht_p       The HashTable for the privileges array.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_security_operations_grant_privileges(aerospike* as_object_p, as_error *error_p,
        char* role_p, HashTable* privileges_ht_p, zval* options_p TSRMLS_DC)
{
    as_policy_admin             admin_policy;
	as_privilege** 				privileges = NULL;
    int                         privileges_size = 0;
	int 						i = 0;

    if ((!error_p) || (!as_object_p) || (!role_p))  {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to grant privileges");
        DEBUG_PHP_EXT_DEBUG("Unable to grant privileges");
        goto exit;
    }

    set_policy_admin(&admin_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }
    
	if( (privileges_size = zend_hash_num_elements(privileges_ht_p)) == 0) {
    	PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "No privileges specified");
        DEBUG_PHP_EXT_DEBUG("No privileges specified");
        goto exit;
	}

	privileges = (as_privilege **)alloca(sizeof(as_privilege *) * privileges_size);

    if (AEROSPIKE_OK !=
            aerospike_security_operations_convert_privileges_from_zval(privileges_ht_p,
                privileges, privileges_size, error_p TSRMLS_CC)) {
        DEBUG_PHP_EXT_DEBUG("Unable to parse privileges");
        goto exit;
    }

	aerospike_grant_privileges(as_object_p, error_p, &admin_policy, role_p, privileges, privileges_size);

    if (AEROSPIKE_OK != error_p->code) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to grant privileges");
        DEBUG_PHP_EXT_DEBUG("Unable to grant privileges");
        goto exit;
    }
exit:
	if(privileges) {
        for(i = 0; i < privileges_size; i++) {
            if( privileges[i] != NULL)
                cf_free(privileges[i]);
        }
    }

    return(error_p->code);
}
/*
 *******************************************************************************************************
 * Wrapper function to revoke privileges from an existing user on aerospike server.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param roles_p               The role from which privileges are to be revoked.
 * @param privileges_ht_p       The HashTable for the privileges array.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_security_operations_revoke_privileges(aerospike* as_object_p, as_error *error_p,
        char* role_p, HashTable* privileges_ht_p, zval* options_p TSRMLS_DC)
{
    as_policy_admin             admin_policy;
	as_privilege** 				privileges = NULL;
    int                         privileges_size = 0;
	int 						i = 0;

    if ((!error_p) || (!as_object_p) || (!role_p))  {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to revoke privileges");
        DEBUG_PHP_EXT_DEBUG("Unable to revoke privileges");
        goto exit;
    }

    set_policy_admin(&admin_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }
    
	if( (privileges_size = zend_hash_num_elements(privileges_ht_p)) == 0) {
    	PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR_PARAM, "No privileges specified");
        DEBUG_PHP_EXT_DEBUG("No privileges specified");
        goto exit;
	}

	privileges = (as_privilege **)alloca(sizeof(as_privilege *) * privileges_size);

    if (AEROSPIKE_OK !=
            aerospike_security_operations_convert_privileges_from_zval(privileges_ht_p,
                privileges, privileges_size, error_p TSRMLS_CC)) {
        DEBUG_PHP_EXT_DEBUG("Unable to parse privileges");
        goto exit;
    }

	aerospike_revoke_privileges(as_object_p, error_p, &admin_policy, role_p, privileges, privileges_size);

    if (AEROSPIKE_OK != error_p->code) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to revoke privileges");
        DEBUG_PHP_EXT_DEBUG("Unable to revoke privileges");
        goto exit;
    }
exit:
	if(privileges) {
        for(i = 0; i < privileges_size; i++) {
            if( privileges[i] != NULL)
                cf_free(privileges[i]);
        }
    }

    return(error_p->code);
}

/*
 *******************************************************************************************************
 * Wrapper function to query a role on aerospike server to retrieve the role's privileges.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param role_p                The role from which privileges are to be retrieved.
 * @param privileges_p          The zval privileges array to be populated by this method.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_security_operations_query_role(aerospike* as_object_p, as_error *error_p,
        char* role_p, zval* roles_p, zval* options_p TSRMLS_DC)
{
    as_policy_admin             admin_policy;
    as_role*              		role_object_p = NULL;

    if ((!error_p) || (!as_object_p) || (!role_p))  {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to query role");
        DEBUG_PHP_EXT_DEBUG("Unable to query role");
        goto exit;
    }

    set_policy_admin(&admin_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

	aerospike_query_role(as_object_p, error_p, &admin_policy, role_p, &role_object_p);
    if (AEROSPIKE_OK != error_p->code) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to query role");
        DEBUG_PHP_EXT_DEBUG("Unable to query role");
        goto exit;
    }

    if (AEROSPIKE_OK !=
            aerospike_security_operations_convert_role_to_zval(roles_p,
                role_object_p, error_p TSRMLS_CC)) {
        DEBUG_PHP_EXT_DEBUG("Unable to parse as_role");
        goto exit;
    }

exit:
    if (role_object_p) {
        as_role_destroy(role_object_p);
    }
    return(error_p->code);
}

/*
 *******************************************************************************************************
 * Wrapper function to query roles on aerospike server to retrieve the all role's privileges.
 *
 * @param as_object_p           The C client's aerospike object.
 * @param error_p               The as_error to be populated by the function
 *                              with the encountered error if any.
 * @param privileges_p          The zval privileges array to be populated by this method.
 * @param options_p             The user's optional policy options to be used if set, else defaults.
 *
 *******************************************************************************************************
 */
extern as_status
aerospike_security_operations_query_roles(aerospike* as_object_p, as_error *error_p,
        zval* roles_p, zval* options_p TSRMLS_DC)
{
    as_policy_admin             admin_policy;
    as_role**             		role_object_p = NULL;
	int							roles_size = 0;

    if ((!error_p) || (!as_object_p))  {
        PHP_EXT_SET_AS_ERR(error_p, AEROSPIKE_ERR, "Unable to query roles");
        DEBUG_PHP_EXT_DEBUG("Unable to query roles");
        goto exit;
    }

    set_policy_admin(&admin_policy, options_p, error_p TSRMLS_CC);

    if (AEROSPIKE_OK != (error_p->code)) {
        DEBUG_PHP_EXT_DEBUG("Unable to set policy");
        goto exit;
    }

	aerospike_query_roles(as_object_p, error_p, &admin_policy, &role_object_p, &roles_size);
    if (AEROSPIKE_OK != error_p->code) {
        PHP_EXT_SET_AS_ERR(error_p, error_p->code, "Unable to query all roles");
        DEBUG_PHP_EXT_DEBUG("Unable to query all roles");
        goto exit;
    }

    if (AEROSPIKE_OK !=
            aerospike_security_operations_convert_roles_to_zval(roles_p,
                role_object_p, roles_size, error_p TSRMLS_CC)) {
        DEBUG_PHP_EXT_DEBUG("Unable to parse as_roles");
        goto exit;
    }

exit:
    if (role_object_p) {
        as_roles_destroy(role_object_p, roles_size);
    }
    return(error_p->code);
}
