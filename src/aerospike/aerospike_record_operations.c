#include "php.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_error.h"
#include "aerospike/as_record.h"

#include "aerospike_common.h"
#include "aerospike_policy.h"

extern as_status aerospike_record_operations_exist(aerospike* as_object_p,
                                 as_key* as_key_p,
                                 as_error *error_p)
{
    as_status                   status = AEROSPIKE_OK;
    as_policy_read              read_policy;
    as_record*                  record_p;

    if ( (!as_key_p) || (!error_p) ||
         (!as_object_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    if (AEROSPIKE_OK != (status = set_policy(&read_policy, NULL, NULL))) {
        status = AEROSPIKE_ERR;
        goto exit;
    }
  
    if (AEROSPIKE_OK != (status = aerospike_key_exists(as_object_p, error_p, NULL, as_key_p, &record_p))) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

exit:
    return(status);   
}


extern as_status aerospike_record_operations_delete(aerospike* as_object_p,
                                 as_key* as_key_p,
                                 as_error *error_p)
{
    as_status                   status = AEROSPIKE_OK;

    if ( (!as_key_p) || (!error_p) ||
         (!as_object_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    if (AEROSPIKE_OK != (status = aerospike_key_remove(as_object_p, error_p, NULL, as_key_p))) {
        goto exit;
    }

exit: 
    return(status);
}

extern as_status
aerospike_record_operations_ops(aerospike* as_object_p,
                                as_key* as_key_p,
                                zval* options_p,
                                as_error* error_p,
                                int8_t* bin_name_p,
                                int8_t* str,
                                u_int64_t offset,
                                u_int64_t initial_value,
                                u_int64_t time_to_live,
                                u_int64_t operation)
{
    as_status           status = AEROSPIKE_OK;
    as_policy_operate   operate_policy;
    as_record*          get_rec = NULL;
    as_operations       ops;
    as_val* value_p = NULL;
    const char *select[] = {bin_name_p, NULL};

    as_operations_inita(&ops, 1);
    as_policy_operate_init(&operate_policy);

    if ((!as_object_p) || (!error_p) || (!as_key_p)) {
        status = AEROSPIKE_ERR;
        goto exit;
    }

    if (AEROSPIKE_OK != (status = set_policy_operations(&operate_policy, options_p))) {
        goto exit;
    }

    switch(operation) {
        case AS_OPERATOR_APPEND:
            as_operations_add_append_str(&ops, bin_name_p, str);
            break;
        case AS_OPERATOR_PREPEND:
            as_operations_add_prepend_str(&ops, bin_name_p, str);
            break;
        case AS_OPERATOR_INCR:
            if (aerospike_key_select(as_object_p, error_p, NULL, as_key_p, select, &get_rec) == AEROSPIKE_OK) {
                if (NULL != (value_p = (as_val *) as_record_get (get_rec, bin_name_p))) {
                   if (AS_NIL == value_p->type) {
                       as_integer initial_int_val;
                       as_integer_init(&initial_int_val, initial_value);
                       as_operations_add_write(&ops, bin_name_p, (as_bin_value*) &initial_int_val);
                       as_integer_destroy(&initial_int_val);
                   } else {
                       as_operations_add_incr(&ops, bin_name_p, offset);
                   }
                } else {
                    status = AEROSPIKE_ERR;
                    goto exit;
                }
            }
            break;
        case AS_OPERATOR_TOUCH:
            ops.ttl = time_to_live;
            as_operations_add_touch(&ops);
            break;
        default:
            status = AEROSPIKE_ERR;
            goto exit;
            break;
    }

    if (AEROSPIKE_OK != (status = aerospike_key_operate(as_object_p, error_p, &operate_policy, as_key_p, &ops, &get_rec))) {
        goto exit;
    }

exit:
     as_operations_destroy(&ops);
     if (get_rec) {
         as_record_destroy(get_rec);
     }
     return status;
}

