# Docker image for aerospike extension

The build has to be done once. Other commands should build done every time.

## Build the container

    sudo docker build -t aerospike .
    
## Run a previously build container

    sudo docker run -t --entrypoint=/bin/bash -i -v `realpath ./`:/root/php -p 8081:8081 -p 8082:80 aerospike

## Start services

    /etc/init.d/aerospike start && /etc/init.d/amc start
    
## Rebuild the extension

    cd /root/php/src/aerospike && ./build.sh --loglevel TRACE

## Launch php built-in webserver

    php -S 0.0.0.0:80 -t /root/php

You can check the result by accessing http://127.0.0.1:8082/session_test.php

