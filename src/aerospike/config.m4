PHP_ARG_ENABLE(aerospike, whether to enable Aerospike support, [ --enable-aerospike Enable Aerospike support])

if test "$PHP_AEROSPIKE" = "yes"; then
  AC_DEFINE(HAVE_AEROSPIKE, 1, [Whether you have Aerospike])
  PHP_NEW_EXTENSION(aerospike, aerospike.c aerospike_policy.c aerospike_transform.c aerospike_helper.c aerospike_record_operations.c aerospike_udf.c aerospike_scan.c aerospike_query.c aerospike_index_operations.c, $ext_shared)
fi
