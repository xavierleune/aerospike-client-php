APACHE INSTALLATION
Apache can be installed either from source or by directly downloading the
package using apt-get

1. For directly downloading the package it is preferred to get the LAMP server
which will integrate your apache with php. LAMP server package can be downloaded
by the following command
sudo apt-get install lamp-server^

2. Installation from source
Apache has several dependencies. One has to install apr, apr-util, pecr.

a.To install apr. Download the tar source and then perform the following commands:
tar -xvf apr.x.x.x.gz
cd apr.x.x.x
./configure --prefix=/usr/local/apr
make
make install

To install apr-util. Download the tar source and then perform the following commands:
tar -xvf apr-util.x.x.x.gz
cd apr-util.x.x.x
./configure --prefix=/usr/local/apr-util --with-apr=/usr/local/apr
make
make install

To install apache. Download the tar source and then perform the following commands:
tar -xvf apache.x.x.x.gz
cd apache.x.x.x
./configure --enable-so --with-apr=/usr/local/apr --with-apr-util=/usr/local/apr-util
make
make install

Installing and configuring php
Download and unpack php
cd php.x.x.xx
./configure --with-apxs2=/pathtoapxs
make
make install
cp php.ini-developemnt /usr/local/lib/php.ini


NGINX INSTALLATION
Nginx can be installed either from source or by directly downloading the package
using apt-get

1. For directly downloading the package perform the following command
sudo apt-get install nginx

Nginx can be started by the following command
service nginx start

PHP can be installed using the following command
sudo apt-get install php5-fpm

Configure PHP by these steps:
vim /etc/php5/fpm/php.ini

Find the line, cgi.fix_pathinfo=1, and change the 1 to 0.
cgi.fix_pathinfo=0

Open up www.conf:
sudo nano /etc/php5/fpm/pool.d/www.conf
Find the line, listen = 127.0.0.1:9000, and change the 127.0.0.1:9000 to
/var/run/php5-fpm.sock.
listen = /var/run/php5-fpm.sock

Restart php-fpm:
sudo service php5-fpm restart

Configure nginx
sudo nano /etc/nginx/sites-available/default

2. Installation from source
Donwload the tar source and perform the following commands
tar -xvf nginx...gz
cd nginx../
./configure
make
make install

To install php perform the following commands
Obtain and unpack the source
tar zxf php-x.x.x

Configure and build php
cd ../php-x.x.x
./configure --enable-fpm --with-mysql
make
sudo make install

Obtain and move configuration files to their correct locations
cp php.ini-development /usr/local/php/php.ini
cp /usr/local/etc/php-fpm.conf.default /usr/local/etc/php-fpm.conf
cp sapi/fpm/php-fpm /usr/local/bin

Locate up php.ini
Locate cgi.fix_pathinfo= and modify it as follows:
cgi.fix_pathinfo=0

php-fpm.conf must be modified to specify that php-fpm must run as the user
www-data and the group www-data before we can start the service:

vim /usr/local/etc/php-fpm.conf

Find and modify the following:
; Unix user/group of processes
; Note: The user is mandatory. If the group is not set, the default user's
group will be used.
user = www-data
group = www-data

The php-fpm service can now be started:
/usr/local/bin/php-fpm

Nginx must now be configured to support the processing of PHP applications:
vim /usr/local/nginx/conf/nginx.conf

Modify the default location block to be aware it must attempt to serve .php files:
location / {
    root   html;
    index  index.php index.html index.htm;
}

The next step is to ensure that .php files are passed to the PHP-FPM backend,
blow the commented default PHP location block, enter the following:
location ~* \.php$ {
    fastcgi_index   index.php;
    fastcgi_pass    127.0.0.1:9000;
    include         fastcgi_params;
    fastcgi_param   SCRIPT_FILENAME
    $document_root$fastcgi_script_name;
    fastcgi_param   SCRIPT_NAME
    $fastcgi_script_name;
}

Restart Nginx.
sudo /usr/local/nginx/sbin/nginx -s stop
sudo /usr/local/nginx/sbin/nginx

PHALCON INSTALLATION
Prerequisite packages for phalcon should be installed by the following command
#Ubuntu
sudo apt-get install php5-dev libpcre3-dev gcc make php5-mysql

#Suse
sudo yast -i gcc make autoconf2.13 php5-devel php5-pear php5-mysql

#CentOS/RedHat/Fedora
sudo yum install php-devel pcre-devel gcc make

#Solaris
pkg install gcc-45 php-53 apache-php53


Compilation and creation of the extension
git clone --depth=1 git://github.com/phalcon/cphalcon.git
cd cphalcon/build
sudo ./install

Add the extension to your php configuration
#Suse: Add this line in your php.ini
extension=phalcon.so


#Centos/RedHat/Fedora: Add a file called phalcon.ini in /etc/php.d/ with this
content:
extension=phalcon.so

#Ubuntu/Debian: Add a file called 30-phalcon.ini in /etc/php.d/ with this
content:
extension=phalcon.so

#Debian with php5-fpm: Add a file called 30-phalcon.ini in
/etc/php5/fpm/conf.d/30-phalcon.ini with this content:
extension=phalcon.so


Restart the webserver
sudo service php5-fpm restart


HTTPERF INSTALLATION
Httperf can be installed from source or directly using package

1. To install using package
sudo apt-get install httperf

2. To download from source
tar -xvf httperf.x.x.gz
cd httperf.x.x
./configure
make
make install

Usage

Basic request
httperf --server localhost --port 80 --uri /

To print response
httperf --server localhost --port 80 --uri /  --print-reply

To send multiple connections
httperf --server localhost --port 80 --uri /  --print-reply --num-conn 100

To send multiple concurrent connections
httperf --server localhost --port 80 --uri /  --print-reply --num-conn 100 --rate 6

To send requests of different methods
httperf --server localhost --port 80 --uri /  --print-reply --num-conn 100
--rate 6 --method POST
