#!/bin/bash
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

CWD=`dirname $0`
AEROSPIKE_EXT_PATH=${CWD}/..
cd ${AEROSPIKE_EXT_PATH}
if [ ! -f php_aerospike.h ]; then
    echo "Cannot find the extension source directory (src/aerospike)"
    exit 1
fi
if [ -x git ]; then
  git clean -fd
else
  rm -f ${AEROSPIKE_EXT_PATH}/Makefile*
  rm -f ${AEROSPIKE_EXT_PATH}/ac*m4
  rm -rf ${AEROSPIKE_EXT_PATH}/autom4te.cache/
  rm -rf ${AEROSPIKE_EXT_PATH}/build/
  rm -f ${AEROSPIKE_EXT_PATH}/config.guess
  rm -f ${AEROSPIKE_EXT_PATH}/config.h*
  rm -f ${AEROSPIKE_EXT_PATH}/config.log
  rm -f ${AEROSPIKE_EXT_PATH}/config.nice
  rm -f ${AEROSPIKE_EXT_PATH}/config.status
  rm -f ${AEROSPIKE_EXT_PATH}/config.sub
  rm -f ${AEROSPIKE_EXT_PATH}/configure*
  rm -f ${AEROSPIKE_EXT_PATH}/.deps
  rm -f ${AEROSPIKE_EXT_PATH}/install-sh
  rm -f ${AEROSPIKE_EXT_PATH}/libtool
  rm -f ${AEROSPIKE_EXT_PATH}/ltmain.sh
  rm -f ${AEROSPIKE_EXT_PATH}/missing
  rm -f ${AEROSPIKE_EXT_PATH}/mkinstalldirs
  rm -f ${AEROSPIKE_EXT_PATH}/run-tests.php
  rm -f ${AEROSPIKE_EXT_PATH}/tmp-php.ini
  rm -f ${AEROSPIKE_EXT_PATH}/aerospike*l?
  rm -rf ${AEROSPIKE_EXT_PATH}/.libs/
fi
