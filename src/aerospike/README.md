# Aerospike PHP Client API Zend Engine Extension

This directory contains the source code for the Aerospike PHP Client API
Zend engine extension.

## Prerequisites

To build the Aerospike PHP Client API extension, the PHP and PHP
development packages must be installed.

The Aerospike C Client SDK must also be installed.

## Build Instructions

Process the `config.m4` file to generate the configuration scripts:

	$ phpize

Configure the extension and create the `Makefile`:

	$ ./configure "CFLAGS=-g -O3" --enable-aerospike

Build the `modules/aerospike.so` Zend extension shared library:

	$ make

## Installation Instructions

Add the following lines to `/etc/php.ini` (or else to your own `php.ini`
file to be used via the `--php_ini` command-line option to `php`):

	enable_dl=On
	extension=modules/aerospike.so

To install the Aerospike PHP Client API extension shared library, do the
following:

	$ sudo cp -p modules/aerospike.so `php-config --extension-dir`

[_Note:_  On CentOS, the PHP extension directory is usually
`/usr/lib64/php/modules/`, while on Debian / Ubuntu, it is usually
`/usr/lib/php5/extensions/`.]
