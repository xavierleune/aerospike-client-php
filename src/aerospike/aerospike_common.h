#ifndef __AEROSPIKE_COMMON_H__
#define __AEROSPIKE_COMMON_H__

#define foreach_hashtable(ht, position, datavalue)               \
    for (zend_hash_internal_pointer_reset_ex(ht, &position);     \
         zend_hash_get_current_data_ex(ht,                       \
                (void **) &datavalue, &position) == SUCCESS;        \
         zend_hash_move_forward_ex(ht, &position))

extern as_status
aerospike_transform_iterate_for_put_key_params(HashTable* ht_p, int8_t** namespace_pp, int8_t** set_pp, zval** key_pp);

extern as_status
aerospike_transform_iteratefor_name_port(HashTable* ht_p, as_config* as_config_p);

extern as_status
aerospike_transform_iteratefor_hostkey(HashTable* ht_p, zval** retdata_pp);


#endif
