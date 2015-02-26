#! /bin/bash
################################################################################
# Copyright 2013-2015 Aerospike, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

LUA_SYSPATH_PREFIX=$1
AEROSPIKE_C_CLIENT=$2
if [ ! $LUA_SYSPATH_PREFIX ] || [ ! $AEROSPIKE_C_CLIENT ]; then
    printf "Usage:\nlua-paths.sh /opt/aerospike 3.0.84\n"
    exit 1
fi

LUA_SYSPATH=${LUA_SYSPATH_PREFIX}/client-php/${AEROSPIKE_C_CLIENT}/sys/udf/lua
LUA_USRPATH=${LUA_SYSPATH_PREFIX}/client-php/${AEROSPIKE_C_CLIENT}/usr/udf/lua

mkdir -p ${LUA_SYSPATH}
if [ $? -gt 0 ] ; then
    exit 2
fi
chmod 0755 ${LUA_SYSPATH}
rm -f ${LUA_SYSPATH_PREFIX}/client-php/sys-lua
ln -s ${LUA_SYSPATH} ${LUA_SYSPATH_PREFIX}/client-php/sys-lua

mkdir -p ${LUA_USRPATH}
if [ $? -gt 0 ] ; then
    exit 3
fi
chmod 0777 ${LUA_USRPATH}
rm -f ${LUA_SYSPATH_PREFIX}/client-php/usr-lua
ln -s ${LUA_USRPATH} ${LUA_SYSPATH_PREFIX}/client-php/usr-lua

