#ifndef __AEROSPIKE_COMMON_H__
#define __AEROSPIKE_COMMON_H__

#define foreach_hashtable(index, position, datavalue)               \
    for (zend_hash_internal_pointer_reset_ex(index, &position);     \
         zend_hash_get_current_data_ex(index,                       \
                (void **) &datavalue, &position) == SUCCESS;        \
         zend_hash_move_forward_ex(index, &position))

#endif
