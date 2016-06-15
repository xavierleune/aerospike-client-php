from php:5.6-cli

run apt-get update && apt-get install -y wget logrotate && cd /tmp && \
    wget -O aerospike.tgz http://aerospike.com/download/server/latest/artifact/ubuntu14 && \
    tar -xvf /tmp/aerospike.tgz && cd aerospike-server-community-*-ubuntu14* && \
    ./asinstall && \
    service aerospike start

run apt-get install -y python-dev gcc

run wget -O aerospike-amc.deb http://www.aerospike.com/download/amc/3.6.8/artifact/ubuntu12 && \
    dpkg -i aerospike-amc.deb && \
    /etc/init.d/amc start

run echo "namespace session { \
    replication-factor 2 \
    memory-size 4G \
    default-ttl 30d \
    storage-engine memory \
    }" >> /etc/aerospike/aerospike.conf && service aerospike start && \
    ln -s /root/php/src/aerospike/modules/aerospike.so /usr/local/lib/php/extensions/no-debug-non-zts-20131226/aerospike.so && \
    echo "extension=aerospike.so" > /usr/local/etc/php/conf.d/aerospike.ini

EXPOSE 80