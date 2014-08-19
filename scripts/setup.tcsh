setenv CLIENTREPO_3X ${PWD}/aerospike-client-c
setenv AEROSPIKE_C_CLIENT 3.0.72
setenv PATH ${PATH}:${PWD}/scripts

echo "Using Aerospike C API version: $AEROSPIKE_C_CLIENT"

if ( ! -d $CLIENTREPO_3X ) then
   echo "Downloading Aerospike C Client SDK..."
   scripts/aerospike-client-c.sh
endif

echo "Aerospike C Client SDK is present."

cd src/aerospike
