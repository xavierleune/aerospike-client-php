#!/bin/bash

CWD=`dirname $0`
diff_path=${CWD}/..
cd ${diff_path}
count=`find ${diff_path}/tests/phpt -name "*\.diff"|wc -l`
# remove whitespace
count=${count// /}
if [[ $count != '0' && $TRAVIS_PHP_VERSION == '5.4' ]]; then
    ls /opt/aerospike/client-php/sys-lua | ./scripts/slacktee.sh -t "$TRAVIS_REPO_SLUG $TRAVIS_JOB_NUMBER client-php/sys-lua"
    ls /opt/aerospike/client-php/usr-lua | ./scripts/slacktee.sh -t "$TRAVIS_REPO_SLUG $TRAVIS_JOB_NUMBER client-php/usr-lua"
    ls ../../aerospike-server/instance1/share/udf/lua | ./scripts/slacktee.sh -t "$TRAVIS_REPO_SLUG $TRAVIS_JOB_NUMBER i1:share/udf/lua"
    ls ../../aerospike-server/instance1/var/udf/lua | ./scripts/slacktee.sh -t "$TRAVIS_REPO_SLUG $TRAVIS_JOB_NUMBER i1:var/udf/lua"
    ls ../../aerospike-server/instance2/share/udf/lua | ./scripts/slacktee.sh -t "$TRAVIS_REPO_SLUG $TRAVIS_JOB_NUMBER i2:share/udf/lua"
    ls ../../aerospike-server/instance2/var/udf/lua | ./scripts/slacktee.sh -t "$TRAVIS_REPO_SLUG $TRAVIS_JOB_NUMBER i2:var/udf/lua"
    cat ../../aerospike-server/instance1/var/log/aerospike.log | ./scripts/slacktee.sh -t "$TRAVIS_REPO_SLUG $TRAVIS_JOB_NUMBER ($TRAVIS_COMMIT) i1" -f
    cat ../../aerospike-server/instance2/var/log/aerospike.log | ./scripts/slacktee.sh -t "$TRAVIS_REPO_SLUG $TRAVIS_JOB_NUMBER ($TRAVIS_COMMIT) i2" -f
fi

