#include "php.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_error.h"
#include "aerospike/as_record.h"

#include "aerospike_common.h"
#include "aerospike_policy.h"
extern as_status aerospike_rec_opp_exist(aerospike* as_object_p,
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


extern as_status aerospike_rec_opp_delete(aerospike* as_object_p,
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
        status = AEROSPIKE_ERR;
        goto exit;
    }

exit: 

     return(status);

}
