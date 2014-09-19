#!/bin/bash

function install_server() {
    wget -O aerospike-server.tgz http://aerospike.com/download/server/latest/artifact/tgz
    if [ $? != 0 ]; then
        return 1
    fi
    tar xvzf aerospike-server.tgz
    cp -f .travis/aerospike.conf ./aerospike-server/share/etc
    cd aerospike-server
    return 0
}

function start_server() {
    mkdir instance$1
    ./bin/aerospike init --home instance$1 --instance $1 --service-port $2
    cd instance$1
    sudo ./bin/aerospike start
    cd ..
    sleep 2
}

echo "Installing the Aerospike server"
install_server || exit 1

echo "Starting a two node cluster:"
echo "Instance 1 [127.0.0.1:3000]"
start_server 1 3000

echo "Instance 2 [127.0.0.1:3010]"
start_server 2 3010

